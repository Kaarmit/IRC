/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistierl <aistierl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:05:18 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/12 15:48:35 by aistierl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include "../message/message.hpp"

class client
{
	private :

		int 						_fd;
		std::string					_nick;
		std::string   				_username;
		std::string   				_realname;
		message*					_msg;
		std::vector<std::string> 	_channels;
		bool 						_registered;
		
		client();

	public :

		client(int clientFd);
		~client();

		message*					getMessage(void) const;

};

#endif
