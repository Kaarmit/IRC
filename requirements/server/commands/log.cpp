/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:47:31 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 11:47:40 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handleLog(client* cli, message& msg)
{
	(void)cli; (void)msg;
	std::cout << "----LOG----" << std::endl;
	std::cout << "Clients enregistres: " << std::endl;
	for (std::list<client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		std::cout << (*it)->getNick() << std::endl;
	}
	return true;
}