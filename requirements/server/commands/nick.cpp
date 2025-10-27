/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlencik.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:39:58 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 11:40:30 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool server::handleNick(client* cli, message& msg)
{
    const std::string target = cli->getNick().empty() ? "*" : cli->getNick();

	
	if (!basicChecks(cli, msg))
		return false;
    const std::string requested = msg.getParams().back();
	

    if (!isValidNick(requested))
    {
        std::string line = ":" + _serverName + " 432 " + target + " " + requested + " :Erroneous nickname\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans nick() a la verif 2" << std::endl;
        return false;
    }
    if (isNickTaken(requested))
    {
        std::string line = ":" + _serverName + " 433 " + target + " " + requested + " :Nickname is already in use\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans nick() a la verif 3" << std::endl;
        return false;
    }

    const bool hadNick = !cli->getNick().empty();
    const std::string oldNick = cli->getNick();
    cli->setNick(requested);

    if (cli->getRegistered() && hadNick && oldNick != requested)
        broadcastNickChange(cli, oldNick, requested);

    if (!cli->getRegistered() && !cli->getNick().empty() && !cli->getUser().empty())
		sendWelcomeIfRegistrationComplete(cli);

	// std::cout << "LOG: TA GRAND MERE PASSe DANS NICK" << std::endl;
    return true;
}