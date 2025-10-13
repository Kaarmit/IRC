/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistierl <aistierl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:05:18 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/13 17:12:23 by aistierl         ###   ########.fr       */
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
		bool 						_registered;
		message*					_msg;
		std::string					_serverName;
		std::string					_host;
		std::string					_nick;
		std::string   				_user;
		std::string   				_real;
		std::vector<std::string> 	_channels;
		
		client();

	public :

		client(int clientFd);
		~client();

		int							getFd(void) const;
		bool						getRegistered(void) const;
		void						setRegistered(bool status);
		message*					getMessage(void) const;
		std::string					getServerName(void) const;
		void						setServerName(std::string name);
		std::string					getHost(void) const;
		void						setHost(std::string name);
		std::string					getNick(void) const;
		void						setNick(std::string name);
		std::string					getUser(void) const;
		void						setUser(std::string name);
		std::string					getReal(void) const;
		void						setReal(std::string name);
		std::string					getChannel(void) const;
		void						setChannel(std::string name);	

};

#endif
