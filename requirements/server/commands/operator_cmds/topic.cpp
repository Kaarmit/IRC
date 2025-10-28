/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:45:23 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 18:36:44 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handleTopic(client* cli, message& msg)
{
	//cli enregistré?
	if (!cli->getRegistered())
	{
		std::string error = ":" + this->_serverName + " 451 " + cli->getNick() + " :You have not registered\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}
	//au moins un param?
	if (msg.getParams().empty())
	{
		std::string error = ":" + this->_serverName + " 461 " + cli->getNick() + " TOPIC :Not enough parameters\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	const std::string	strChan = msg.getParams()[0];

	//le chan existe?
	channel*	chan = this->getChannelByName(strChan);
	if (!chan)
	{
		std::string error = ":" + this->_serverName + " 403 "  + cli->getNick() + " " + strChan + " :No such channel\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	//cli membre de chan?
	if (!chan->isMember(cli))
	{
		std::string error = ":" + this->_serverName + " 442 " + cli->getNick() + " " + strChan + " :You're not on that channel\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	//display topic
	if (msg.getParams().size() == 1)
	{
		std::string	topic = chan->getTopic();
		if (topic.empty())
		{
			std::string str = ":" + this->_serverName + " 331 " + cli->getNick() + " " + strChan + " :No topic is set\r\n";
			cli->enqueueLine(str);
			polloutActivate(cli);
			return true;
		}
		else
		{
			std::string str = ":" + this->_serverName + " 332 " + cli->getNick() + " " + strChan + " :Topic is: " + topic + "\r\n";
			std::string line = ":" + this->_serverName + " 333 " + cli->getNick() + " " + strChan + " " + chan->getTopicAuthor() + " " + chan->getTopicTimestampStr() + "\r\n";
			cli->enqueueLine(str);
			cli->enqueueLine(line);
			polloutActivate(cli);
			return true;
		}
	}
	else if (msg.getParams().size() == 2)
	{
		//mode +t et cli == op
		if (chan->isRestrictedTopic() && !chan->isOperator(cli))
		{
			std::string error = ":" + this->_serverName + " 482 " + cli->getNick() + " " + strChan + " :You're not channel operator\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}

		//set topic
		const std::string	strTopic = msg.getParams()[1];
		if (strTopic.empty())
			chan->setTopic("");
		else
			chan->setTopic(strTopic);

		chan->setTopicTimestamp();
		chan->setTopicAuthor(cli->getNick());
		std::string str = userPrefix(cli) + " TOPIC " + strChan + " :" + strTopic + "\r\n";

		for (std::list<client*>::iterator it = chan->getClientList().begin(); it != chan->getClientList().end(); ++it)
		{
			(*it)->enqueueLine(str);
			polloutActivate((*it));
		}
	}

	return true;
}
