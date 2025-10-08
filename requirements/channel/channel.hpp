/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:47:06 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/07 16:23:08 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>

#include "../client/client.hpp"

class channel
{
	private:
		std::string	_name;
		std::vector<client*>	_clients;
		client*	_operateur;

	public:
		channel();
		~channel();
};


#endif
