#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "../client/client.hpp"
#include "../channel/channel.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

class server
{
	private:
		std::vector<struct pollfd>	_fds;
		std::vector<client>	_clients;
		std::map<std::string, channel>	_channels;
		std::string	_passWord;
		int	_serverFd;
		int	_port;

	public:
		server(std::string port, std::string mdp);
		~server();

		void	initSocket(int port);
		void	run();

		std::string	getPassWord() const;
		int	getFd() const;
		int	getPort() const;
};

#endif
