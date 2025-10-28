/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:46:42 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 15:13:36 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handleQuit(client* cli, message& msg)
{
	
	if (!cli)
		return false;
	if(msg.getParams().empty())
	{
		std::string	error = ":" + this->_serverName + " 461 * :Not enough parameters\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		// std::cout << "LOG: return false dans pass() a la verif 1" << std::endl;
		return false;
	}
	
	std::string reason;
	if (msg.getParams().size() >= 2)
	{
		std::string message = msg.getParams()[1];
		for (size_t i = 1; i < msg.getParams().size(); i++)
		{
			message += " " + msg.getParams()[i];
			if (!message.empty() && message[0] == ':')
                message.erase(0, 1);

            while (!message.empty() && (message[0] == ' ' || message[0] == '\t')) message.erase(0,1);

            if (!message.empty())
            reason = " :" + message;
		}
	}

	for (std::list<channel*>::const_iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		(*it)->remove(cli);
		std::string msgToChan;
		if (reason.size() > 0)
			msgToChan = userPrefix(cli) + "QUIT" + reason + "\r\n";
		else 
			msgToChan = userPrefix(cli) + "QUIT" + "\r\n";
		broadcastToChannel((*it), msgToChan);
		std::string msgToClient;
		msgToClient = "ERROR :Closing Link: " + cli->getNick() + reason + "\r\n";

		
		
		//retirer liste MOD
		//supprimer channel si vide 
	}
	
	
	

	//reset le channelAuthor pointeur a NULL
	//deco le client du server
	(void)cli; (void)msg;
	return true;
}
