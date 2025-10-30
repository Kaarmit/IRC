/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:42:59 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/30 15:57:40 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handlePrivmsg(client* cli, message& msg)
{

	if (!cli->getRegistered())
	{
		std::string error = ":server 451 * : You have not registered\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	std::vector<std::string>	params = msg.getParams();
	//erreur 411 si pas d'args
	if (params.empty())
	{
		std::string error = ":server 411 " + cli->getNick() + ":No recipient given (PRIVMSG)\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	//ereur 412 si pas de message
	if (params.size() < 2)
	{
		std::string error = ":server 412 " + cli->getNick() + ":No text to send\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	/*client to client*/
	if (!isChannel(msg.getParams()[0]))
	{
		//erreur 401 si utilisateur dest inconnu
		client* destClient = NULL;
		for (std::list<client*>::iterator	it = this->_clients.begin(); it != this->_clients.end(); it++)
		{
			if ((*it)->getNick() == params[0])
			{
				destClient = (*it);
				break;
			}
		}
		if (!destClient)
		{
			std::string error = ":server 401 " + cli->getNick() + " " + params[0] + ":No such nick\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}
		//etape 4: construiruction du message
		std::string	toSend = userPrefix(cli) + " " + msg.getCommand() + " " + params[0] + " :" + params[1] + "\r\n";
		//etape 5: envoie du message au destinataire
		destClient->enqueueLine(toSend);
		polloutActivate(destClient);
	}
	else
	{
		/*client to channel*/
		//erreur 403 si channel n'existe pas
		channel*	destChannel = NULL;
		for (std::list<channel*>::iterator	it = this->_channels.begin(); it != this->_channels.end(); it++)
		{
			if ((*it)->getChannelName() == params[0])
			{
				destChannel = (*it);
				break;
			}
		}
		if (!destChannel)
		{
			std::string error = ":server 403 " + cli->getNick() + ":No such channel\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}
		//erreur 404 si ce n'est pas le cas et si channel en mode +m et user not operateur
		if (!destChannel->isMember(cli))
		{
			std::string error = ":server 404 " + cli->getNick() + " :Cannot send to channel " + params[0] + "\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}
		//construiruction du message
		std::string	toSend = userPrefix(cli) + " " + msg.getCommand() + " " + params[0] + " " + " :" + params[1] + "\r\n";
		//envoie du message au destinataire
		//on boucle sur tous les membres du channel
		for (std::list<client*>::iterator it = destChannel->getClientList().begin(); it != destChannel->getClientList().end(); it++)
		{
			//on envoie a tous sauf le cli
			if ((*it)->getFd() == cli->getFd())
				continue;
			(*it)->enqueueLine(toSend);
			polloutActivate(*it);
		}
	}
	return true;
}
