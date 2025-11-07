/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistierl <aistierl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:46:42 by aarmitan          #+#    #+#             */
/*   Updated: 2025/11/06 13:57:57 by aistierl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handleQuit(client* cli, message& msg)
{
	
	if (!cli)
		return false;	
	std::string reason = "Client Quit";

	if (!msg.getParams().empty()) 
	{
		std::string r = msg.getParams()[0];

		// Enlève le ':' de début (trailing param)
		if (!r.empty() && r[0] == ':')
			r.erase(0, 1);

		// Trim des espaces/tabs au début
		while (!r.empty() && (r[0] == ' ' || r[0] == '\t'))
			r.erase(0, 1);

		// Supprime les \r et \n internes
		r.erase(std::remove(r.begin(), r.end(), '\r'), r.end());
		r.erase(std::remove(r.begin(), r.end(), '\n'), r.end());

		// Si après tout ça il reste quelque chose
		if (!r.empty())
			reason = r;
	}


	const std::string message = ":" + userPrefix(cli) + " QUIT :" + reason + "\r\n";
	
	std::list<client*> membersToSend;
	std::vector<channel*> toDelete;
	
	for (std::list<channel*>::const_iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		channel* ch = *it;
		if (!ch)
			continue;
			
		if (!channelHasFd((ch), cli->getFd()))
			continue;
			
		const std::list<client*>& members = ch->getClientList();
		for (std::list<client*>::const_iterator itC = members.begin(); itC != members.end(); itC++)
		{
			client * c = *itC;
			if (c && c->getFd() != cli->getFd() && (std::find(membersToSend.begin(), membersToSend.end(), c) == membersToSend.end())
)
				membersToSend.push_back(c);
		}
		(ch)->remove(cli);
		if (ch->empty())
			toDelete.push_back(ch);	
	}
	
	for (std::list<client*>::const_iterator itC = membersToSend.begin(); itC != membersToSend.end(); itC++)
	{
		(*itC)->enqueueLine(message);
		polloutActivate(*itC);
	}
	
	for (std::size_t i = 0; i < toDelete.size(); ++i) 
        deleteChannel(toDelete[i]);
		
	// std::string err = "ERROR :Closing Link: " + cli->getNick() + " (" + reason + ")\r\n";
	// cli->enqueueLine(err);
	// polloutActivate(cli);
    cli->setToRemove(true);
	return true;
}
