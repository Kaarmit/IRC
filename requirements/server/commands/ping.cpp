/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:46:21 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 11:46:31 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

bool	server::handlePing(client* cli, message& msg)
{
	(void)cli; (void)msg;
	std::string	pong = "pong";
	cli->enqueueLine(pong);
	polloutActivate(cli);
	return true;
}