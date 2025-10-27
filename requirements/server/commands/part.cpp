/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:42:15 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 14:52:10 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool server::handlePart(client *cli, message &msg)
{
	const std::string target = cli->getNick().empty() ? "*" : cli->getNick();

	if (!cli->getRegistered())
	{
        std::string line = ":" + this->_serverName + " 451 " + target + " :You are not registered\r\n";
        cli->enqueueLine(line);
		polloutActivate(cli);
		return false;
	}
    // 2) Pas de paramètre → erreur 461
	if (msg.getParams().empty() || msg.getParams()[0].empty())
    {
        std::string line = ":" + _serverName + " 461 " + target + " PART :Not enough parameters\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
    }

    // --- 2) Parser la liste <#a,#b,#c> avec trim + dédup
    std::vector<std::string> chanGiven;
    {
        std::stringstream ss(msg.getParams()[0]);
        std::string item;
        while (std::getline(ss, item, ',')) {
            // trim des espaces autour
            // (mini-trim inline sans helper)
            while (!item.empty() && (item[0] == ' ' || item[0] == '\t')) item.erase(0, 1);
            while (!item.empty() && (item[item.size()-1] == ' ' || item[item.size()-1] == '\t')) item.erase(item.size()-1);

            if (item.empty()) continue;

            // dédup
            if (std::find(chanGiven.begin(), chanGiven.end(), item) == chanGiven.end())
                chanGiven.push_back(item);
        }
    }

    if (chanGiven.empty())
    {
        std::string line = ":" + _serverName + " 461 " + target + " PART :Not enough parameters\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
    }

    // --- 3) Valider chaque channel + appartenance
    std::vector<std::string> validChan;
    for (size_t i = 0; i < chanGiven.size(); ++i)
    {
        const std::string& chname = chanGiven[i];

        if (!server::isChannel(chname))
        {
            std::string line = ":" + _serverName + " 476 " + target + " " + chname + " :Bad channel mask\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }

        channel* ch = this->getChannelByName(chname);
        if (!ch)
        {
            std::string line = ":" + _serverName + " 403 " + target + " " + chname + " :No such channel\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }

        if (!channelHasFd(ch, cli->getFd()))
        {
            std::string line = ":" + _serverName + " 442 " + target + " " + chname + " :You're not on that channel\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }

        validChan.push_back(chname);
    }

    if (validChan.empty())
        return false; // Rien à faire (tout a été répondu par erreurs)

    // --- 4) Construire la "reason" optionnelle (trailing param)
    // RFC: le trailing commence par ':' côté wire. Pour l’émission on normalise à " :reason".
    std::string trailing;
    if (msg.getParams().size() >= 2)
    {
        // Join params[1..end] avec espaces
        std::string joined = msg.getParams()[1];
        for (size_t i = 2; i < msg.getParams().size(); ++i)
            joined += " " + msg.getParams()[i];

        if (!joined.empty() && joined[0] == ':')
            joined.erase(0, 1);          // retire ':' initial si présent

        // trim léger en tête si besoin
        while (!joined.empty() && (joined[0] == ' ' || joined[0] == '\t')) joined.erase(0,1);

        if (!joined.empty())
            trailing = " :" + joined;    // ajoute exactement un " :"
    }

    // --- 5) Broadcast PART, retirer le client, supprimer le salon si vide
    const std::string prefix = userPrefix(cli);

    for (size_t i = 0; i < validChan.size(); ++i)
    {
        const std::string& chname = validChan[i];
        channel* ch = this->getChannelByName(chname);
        if (!ch) // Par sécurité (peu probable entre la validation et ici)
            continue;

        // a) Diffuser à TOUS les membres (y compris l’émetteur)
        std::string line = prefix + " PART " + chname + trailing + "\r\n";
        broadcastToChannel(ch, line); // -> helper à avoir; cf. note ci-dessous

        // b) Retirer le client du salon (membre, op, voice, etc.)
        removeClientFromChannel(ch, cli); // -> helper à avoir; cf. note ci-dessous

        // c) Si salon vide, suppression côté serveur
        if (channelEmpty(ch))
            deleteChannel(ch); // -> helper : enlève de _channels et delete*
    }

    return true;
}