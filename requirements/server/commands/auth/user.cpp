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

bool	server::handleUser(client* cli, message& msg)
{
	const std::string target = cli->getNick().empty() ? "*" : cli->getNick();

	if (!basicChecks(cli, msg))
		return false;


	const std::string username = msg.getParams()[0];
	// std::cout << "LOG: username: " << username << std::endl;
	// std::cout << "LOG: hostname: " << msg.getParams()[1] << std::endl;
	// std::cout << "LOG: servername: " << msg.getParams()[2] << std::endl;
	// std::cout << "LOG: realname: " << msg.getParams()[3] << std::endl;

    if (!isValidUsername(username))
    {
        std::string line = ":" + _serverName + " 468 " + target +  " :Erroneous username\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans user() a la verif 3" << std::endl;
        return false;
    }

	//const std::string hostname = cli->getHost();
	//const std::string servername = this->_serverName;
	std::string realname = msg.getParams()[3];

	for (size_t i = 4; i < msg.getParams().size(); i++)
	{
		realname = realname + " " + msg.getParams()[i];
	}
	if (!realname.empty() && realname[0] == ':')
		realname.erase(0,1);
	cli->setUser(username);
	cli->setReal(realname);

	if (!cli->getRegistered() && !cli->getNick().empty() && !cli->getUser().empty() && !cli->getPass().empty())
		sendWelcomeIfRegistrationComplete(cli);
	// std::cout << "LOG: TA GRAND MERE PASSe DANS USER" << std::endl;
    return true;

}
