/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:47:31 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 15:43:05 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handleLog(client* cli, message& msg)
{
	(void)cli; (void)msg;
	if( this->_clients.empty())
		return false;
	std::cout << "----LOG----" << std::endl;
	std::cout << "Clients enregistres: " << std::endl;
	for (std::list<client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		std::cout << "---NICK: " << (*it)->getNick()
					<< ", USER: " << (*it)->getUser()
					<< " in channels: ";
		if (!(*it)->getChannelList().empty())
			for (std::list<std::string>::iterator i = (*it)->getChannelList().begin(); i != (*it)->getChannelList().end(); i++)
				std::cout << (*i) << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
	if( this->_channels.empty())
		return false;
	std::cout << "Channels crees: " << std::endl;
	for (std::list<channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		std::cout << "---Name: " << (*it)->getChannelName() << ", clients: ";
		for (std::list<client*>::iterator i = (*it)->getClientList().begin(); i != (*it)->getClientList().end(); ++i)
			std::cout << (*i)->getNick() << " ";
		std::cout << std::endl;
	}
	return true;
}
