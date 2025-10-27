/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:46:42 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 11:46:54 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handleQuit(client* cli, message& msg)
{
	// verif si param.empty
	// iterer sur cli channelList
	// it->getClientList().remove(*cli);
	// it->getOpList().remove(*cli);
	// cli->channelList().remove(it);
	//verifier si ct le dernier client du channel, si oui enlever le channel de la liste de chan serv
	//reset le channelAuthor pointeur a NULL
	//deco le client du server
	(void)cli; (void)msg;
	return true;
}