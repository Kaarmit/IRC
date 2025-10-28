/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:45:55 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/28 19:24:31 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../server.hpp"

bool	server::handleMode(client* cli, message& msg)
{
		if (!cli->getRegistered())
	{
		std::string error = ":server 451 * : You have not registered\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}
	if (msg.getParams().empty())
	{
		std::string error = ":" + this->_serverName + " 461 " + cli->getNick() + " MODE :Not enough parameters\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		return false;
	}

	//le chan existe?
	const std::string	strChan = msg.getParams()[0];
	channel*			chan = this->getChannelByName(strChan);
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
	//si on a que <cahnnel> on affiche ses infos -> modes actifs + params des modes siil y en a(key, limit..)
	if(msg.getParams().size() == 1)
	{
		//exemple
		//:server 324 <nick> <chan> +itkl key limit
		return false;//pour la compile
	}
	//sion a des params -> apporter le changement du mode au chan
	else if (msg.getParams().size() > 1)
	{
		//cli op du chan?
		if (!chan->isOperator(cli))
		{
			std::string error = ":" + this->_serverName + " 482 " + cli->getNick() + " " + strChan + " :You're not channel operator\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}

		//lire le signe +/-
		const std::string	strMode = msg.getParams()[1];
		char	sign = strMode[0];
		if (sign != '-' || sign != '+')
		{
			std::string error = ":" + this->_serverName + " 472 " + cli->getNick() + " " + strChan + " " + sign +" :Bad sign\r\n";
			cli->enqueueLine(error);
			polloutActivate(cli);
			return false;
		}

		int		paramsIndex = 0;
		for (size_t i = 1; i < strMode.size(); i++)
		{
			char	letter = strMode[i];

			//process les modes
			if (letter == 'i')
				inviteMode(chan, sign);
			else if (letter == 't')
				topicMode(chan, sign);
			else  if(letter == 'k')
				keyMode(chan, sign, msg.getParams(), paramsIndex);
			else if (letter == 'o')
				operatorMode(chan, sign, msg.getParams(), paramsIndex);
			else if (letter == 'l')
				limitMode(chan, sign, msg.getParams(), paramsIndex);
			else
			{
				std::string error = ":" + this->_serverName + " 472 " + cli->getNick() + " " + strChan + " " + letter +" :is unknow mode char\r\n";
				cli->enqueueLine(error);
				polloutActivate(cli);
				return false;
			}

		//broadcast le changement dans le channel
		std::string str = userPrefix(cli) + " MODe " + strChan + " :" + strMode + "\r\n";

		for (std::list<client*>::iterator it = chan->getClientList().begin(); it != chan->getClientList().end(); ++it)
		{
			(*it)->enqueueLine(str);
			polloutActivate((*it));
		}
		}
	}
	return true;
}
