/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlepass.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:39:27 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 11:39:37 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handlePass(client* cli, message& msg)
{
	//verif si cli deja enregistrer
	// if (!cli->getRegistered())
	// {
	// 	std::string	error = ":" + this->_serverName + " 462 " + cli->getNick() + " :You may not register\r\n";
	// 	cli->enqueueLine(error);
	// 	polloutActivate(cli);
	// 	return false;
	// }

	//verif si NICK ou USER a ete envoye avant pass
	// if (!cli->getNick().empty() || !cli->getUser().empty())
	// {
	// 	std::string	error = ":" + this->_serverName + " 462 * :PASS must be sent before NICK/USER \r\n";
	// 	cli->enqueueLine(error);
	// 	polloutActivate(cli);
	// 	return false;
	// }
	//verif si on a tout les params dans PASS
	if(msg.getParams().empty())
	{
		std::string	error = ":" + this->_serverName + " 461 * :Not enough parameters\r\n";
		cli->enqueueLine(error);
		polloutActivate(cli);
		// std::cout << "LOG: return false dans pass() a la verif 1" << std::endl;
		return false;
	}
	//verif du mdp
	if (msg.getParams()[0] != this->getPassWord())
	{
		std::string	error = ":" + this->_serverName + " 464 * :Password incorrect\r\n";
		cli->enqueueLine(error);
		// std::cout << "LOG: return false dans pass() a la verif 2" << std::endl;
		polloutActivate(cli);
		return false;
	}
	//si tout opk, on set le pass a true ou return true
	cli->setPass(msg.getParams()[0]);//enlever?
	std::string passSet = ":" + this->_serverName + " User password has been set\r\n";
	cli->enqueueLine(passSet);
	polloutActivate(cli);
	// std::cout << "LOG: TA GRAND MERE PASSe DANS PASSE" << std::endl;
	return true;
}