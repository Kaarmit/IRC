/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:41:37 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 18:36:07 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handleJoin(client* cli, message& msg)
{
	if (!cli->getRegistered())
	{
		polloutActivate(cli);
        std::string line = ":" + this->_serverName + "451 * :You are not registered\r\n";
        cli->enqueueLine(line);
		return false;
	}
	std::vector<std::string> params = msg.getParams();
	//check if enough parameters are provided
	if (params.empty())
	{
		polloutActivate(cli);
        std::string line = ":" + this->_serverName + " 461 " + cli->getNick() + " " + msg.getCommand() + " :Not enough parameters\r\n";
        cli->enqueueLine(line);
		return false;
	}

	if (params.size() > 2)
	{
		polloutActivate(cli);
        std::string line = ":" + this->_serverName + " 461 " + cli->getNick() + " " + msg.getCommand() + " :Too many parameters\r\n";
        cli->enqueueLine(line);
		return false;
	}
	// check if "join 0" = leave all channels;
	if (params[0] == "0" && params.size() == 1)
	{
		//remove client from all channels' client lists
		for (std::list<channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
		{
			message	msg;
			msg.setParams((*it)->getChannelName());
			(this->*_cmdList["PART"])(cli, msg);
		}
		//clear client chan list
		cli->getChannelList().clear();
		return true;
	}
	//avec une commande /join #chan1,#chan2 key1,key2
	//subdiviser params en deux vect chanName, et mdp
	std::vector<std::string> chanGiven;
	chanGiven.clear();
	std::vector<std::string> keysGiven;
	keysGiven.clear();
	std::stringstream ss(params[0]);
	std::string item;
	while (std::getline(ss, item, ','))
		chanGiven.push_back(item);
	if (params.size() == 2)
	{
		std::string item2;
		std::stringstream ss2(params[1]);
		while (std::getline(ss2, item2, ','))
			keysGiven.push_back(item2);
	}
	if (params.size() > chanGiven.size() + keysGiven.size())
	{
		polloutActivate(cli);
        std::string line = ":" + this->_serverName + " 476 " + cli->getNick() + " " + params[0] + " :Bad channel mask\r\n";
        cli->enqueueLine(line);
		return false;
	}
	for (size_t i = 0; i < chanGiven.size(); ++i)
	{
		if (server::isChannel(chanGiven[i]) == false)
		{
			polloutActivate(cli);
        	std::string line = ":" + this->_serverName + " 476 " + cli->getNick() + " " + chanGiven[i] + " :Bad channel mask\r\n";
        	cli->enqueueLine(line);
			continue;
		}
		//search for channel name
		channel* found = this->getChannelByName(chanGiven[i]);
		if (!found)
		{
			//create the channel if itChan doesn't exist
			//add the client to the channel's client list and op list (channel constructor)
			channel* newChannel = new channel(chanGiven[i], cli);
			//add new channel to the server's channel map
			this->_channels.push_back(newChannel);
			//add the channel to the client's channel list
			cli->getChannelList().push_back(chanGiven[i]);
		}
		else
		{
			//check if already in the channel
			if (channelHasFd(found, cli->getFd()) == false)
			{
				//check if client is invited
				if (found->isInviteOnly())
				{
					std::list<client*>& invitedList = found->getInvitedList();
					if (std::find(invitedList.begin(), invitedList.end(), cli) == invitedList.end())
					{
						//not invited can't enter
						polloutActivate(cli);
        				std::string line = ":" + this->_serverName + " 473 " + cli->getNick() + " " + chanGiven[i] + " :You need an invite to join this channel\r\n";
        				cli->enqueueLine(line);
						continue;
					}
				}
				//check if theres a channel key/password
				if (!found->getKey().empty())
				{
					if (i >= keysGiven.size() || keysGiven.empty() || found->getKey().compare(keysGiven[i]) != 0)
					{
						polloutActivate(cli);
        				std::string line = ":" + this->_serverName + " 475 " + cli->getNick() + " " + chanGiven[i] + " :Invalid key\r\n";
        				cli->enqueueLine(line);
						continue;
					}
				}
				//check if theres a limit
				if (found->getLimit() != -1)
				{
					if (static_cast<size_t>(found->getLimit()) <= found->getClientList().size())
					{
						polloutActivate(cli);
        				std::string line = ":" + this->_serverName + " 471 " + cli->getNick() + " " + chanGiven[i] + " :Channel is full\r\n";
        				cli->enqueueLine(line);
						continue;
					}
				}
				// if client got invited, remove him from the invited list
				if (!found->getInvitedList().empty())
					found->getInvitedList().remove(cli);
				//add the channel to the client's channel list
				// cli->getChannelList().push_back(params[0]); DARYL:j'ai mis en com
				cli->getChannelList().push_back(chanGiven[i]);
				//add the client to the channel's client list
				found->getClientList().push_back(cli);
				broadcastJoin(cli, *found);
			}
		}
	}
	return (true);
}
