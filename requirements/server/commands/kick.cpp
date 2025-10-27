/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:44:42 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 11:49:57 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handleKick(client* cli, message& msg)
{
	
	// 1) registered ? 
	const std::string target = cli->getNick().empty() ? "*" : cli->getNick();

	if (!cli->getRegistered())
	{
        std::string line = ":" + this->_serverName + " 451 " + target + " :You are not registered\r\n";
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
	
	// 3) salon existe
	
	// 4) client est sur le salon
	
	// 5) client est operateur du salon
	
	// 6) utilisateur cible est sur le salon

}