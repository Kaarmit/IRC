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
		message						_msg;
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
		message&					getMessage(void);//modif de Daryl: retourne un ref pour avoir levrai message et pas une cpy
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
		std::vector<std::string>	getChannel(void) const;

};

#endif
