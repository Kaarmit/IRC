/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:44:42 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 18:36:34 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handleKick(client* cli, message& msg)
{

	// 1) registered ?
	const std::string target = cli->getNick().empty() ? "*" : cli->getNick();

	if (!cli->getRegistered())
	{
        std::string line = ":" + this->_serverName + " 451 " + target + " :You have not registered\r\n";
        cli->enqueueLine(line);
		polloutActivate(cli);
		return false;
	}

	// 2) nombres de params insuffisant
	std::vector<std::string>	params = msg.getParams();
	if (params.size() != 2 && params.size() != 3)
	{
        std::string line = ":" + _serverName + " 461 " + target + " KICK :Not enough parameters\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
	}

	// 3) split params
    std::vector<std::string> chanGiven;
    std::vector<std::string> userToKick;
    {
        std::stringstream ss(msg.getParams()[0]);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            // trim des espaces autour
            // (mini-trim inline sans helper)
            while (!item.empty() && (item[0] == ' ' || item[0] == '\t')) item.erase(0, 1);
            while (!item.empty() && (item[item.size()-1] == ' ' || item[item.size()-1] == '\t')) item.erase(item.size()-1);

            if (item.empty()) continue;
            chanGiven.push_back(item);
        }
    }

    {
        std::stringstream ss(msg.getParams()[1]);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            // trim des espaces autour
            // (mini-trim inline sans helper)
            while (!item.empty() && (item[0] == ' ' || item[0] == '\t')) item.erase(0, 1);
            while (!item.empty() && (item[item.size()-1] == ' ' || item[item.size()-1] == '\t')) item.erase(item.size()-1);

            if (item.empty()) continue;
            userToKick.push_back(item);
        }
    }

    if (userToKick.size() != 1 && (userToKick.size() != chanGiven.size()))
    {
        std::string line = ":" + _serverName + " 461 " + target + " KICK :Not enough parameters\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
    }

    if (chanGiven.empty())
    {
        std::string line = ":" + _serverName + " 461 " + target + " KICK :Not enough parameters\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
    }


    std::string user;
    for (size_t i = 0; i < chanGiven.size(); i++)
    {
        const std::string& chname = chanGiven[i];

        if (userToKick.size() == 1)
            user = userToKick[0];
        else
            user = userToKick[i];

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

        if (!ch->isMember(cli))
        {
            std::string line = ":" + _serverName + " 442 " + target + " " + chname + " :You're not on that channel\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }

        if (!isOP(cli, ch))
        {
            std::string line = ":" + _serverName + " 482 " + target + " " + chname + " :You're not channel operator\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }

        client* tokick = this->getClientByNick(user);

        if (!tokick)
        {
            std::string line = ":" + _serverName + " 401 " + target + " " + user + " :No such nick\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }
        int fd = tokick->getFd();

        if (!channelHasFd(ch, fd))
        {
            std::string line = ":" + _serverName + " 441 " + target + " " + user + " " + chname + " :They aren't on that channel\r\n";
            cli->enqueueLine(line);
            polloutActivate(cli);
            continue;
        }

        std::string comment;
        if (params.size() > 2)
        {
            std::string joined = msg.getParams()[2];
            for (size_t i = 3; i < msg.getParams().size(); ++i)
                joined += " " + msg.getParams()[i];

            if (!joined.empty() && joined[0] == ':')
                joined.erase(0, 1);

            while (!joined.empty() && (joined[0] == ' ' || joined[0] == '\t')) joined.erase(0,1);

            if (!joined.empty())
            comment = " :" + joined;
        }
        else
            comment = " :" + target;

        const std::string prefix = userPrefix(cli);
        std::string line = prefix + " KICK " + chname + " " + user + comment + "\r\n";
        broadcastToChannel(ch, line);
        ch->remove(tokick);
        if (channelEmpty(ch))
            deleteChannel(ch);
    }
    return true;

}
