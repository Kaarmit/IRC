/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:41:02 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 18:36:03 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool server::handleUser(client* cli, message& msg)
{
    const std::string target = cli->getNick().empty() ? "*" : cli->getNick();

    if (cli->getRegistered())
	{
		std::string line = ":" + _serverName + " 462 " + target + " :You may not reregister\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans user() a la verif 1" << std::endl;
        return false;
	}
    if (msg.getParams().empty())
    {
        std::string line = ":" + _serverName + " 431 " + target + " :No username given\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans nick() a la verif 1" << std::endl;
        return false;
    }

    const std::vector<std::string>& p = msg.getParams();

    if (p.size() < 4) {
        std::string line = ":" + _serverName + " 461 " + target + " USER :Not enough parameters\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
    }

    const std::string& username = p[0];

    if (!isValidUsername(username)) {
        std::string line = ":" + _serverName + " 468 " + target + " :Erroneous username\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
        return false;
    }


    std::string realname = p[3];
    for (size_t i = 4; i < p.size(); ++i) {
        realname += " ";
        realname += p[i];
    }
    if (!realname.empty() && realname[0] == ':')
        realname.erase(0, 1);
    while (!realname.empty() && (realname[0] == ' ' || realname[0] == '\t'))
        realname.erase(0, 1);
    for (std::string::size_type i = 0; i < realname.size(); )
        if (realname[i] == '\r' || realname[i] == '\n') realname.erase(i, 1); else ++i;
    if (realname.empty()) realname = "unknown";

    cli->setUser(username);
    cli->setReal(realname);

    if (!cli->getRegistered() && !cli->getNick().empty() && !cli->getUser().empty() && !cli->getPass().empty())
        sendWelcomeIfRegistrationComplete(cli);

    return true;
}
