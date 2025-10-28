/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:45:03 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 18:36:31 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handleInvite(client* cli, message& msg)
{
	//cli enregistré?
	if (!cli->getRegistered())
	{
		std::string error = ":" + this->_serverName + " 451 " + cli->getNick() + " :You have not registered\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}
	//on a que deux params?
	if (msg.getParams().size() < 2)
	{
		std::string error = ":" + this->_serverName + " 461 " + cli->getNick() + " INVITE :Not enough parameters\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	const std::string&	nickToInvite = msg.getParams()[0];
	const std::string&	channelName = msg.getParams()[1];

	//nick existe?
	client* user = this->getClientByNick(nickToInvite);
	if (!user)
	{
		std::string error = ":" + this->_serverName + " 401 " + cli->getNick() + " " + nickToInvite + " :No such nick\r\n";
        cli->enqueueLine(error);
        polloutActivate(cli);
        return false;
	}

	//si le chan existe pas on envoie quand meme les invite
	channel* chan = this->getChannelByName(channelName);
	if (chan)
	{
		//cli dans le channel?
		if (!chan->isMember(cli))
		{
			std::string error = ":" + this->_serverName + " 442 " + cli->getNick() + " " + channelName + " :You're not on that channel\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}
		//cli operator du channel?
		if (chan->isInviteOnly() && !chan->isOperator(cli))
		{
			std::string error = ":" + this->_serverName + " 482 " + cli->getNick() + " " + channelName + " :You're not channel operator\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}
		//nick deja dans le channel?
		if (chan->isMember(user))
		{
			std::string error = ":" + this->_serverName + " 443 " + cli->getNick() + " " + nickToInvite + " " + channelName + " :is already on channel\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}
		chan->addToInviteList(user);
	}

	//on envoie la reponse au cli qui invite
	std::string	answer = ":" + this->_serverName + " 341 " + cli->getNick() + " " + nickToInvite + " " + channelName + "\r\n";
    cli->enqueueLine(answer);
	polloutActivate(cli);

	//on envoie l'invite au user
	std::string inviteMsg = userPrefix(cli) + " INVITE " + nickToInvite + " " + channelName + "\r\n";
    user->enqueueLine(inviteMsg);
    polloutActivate(user);
	return true;
}
