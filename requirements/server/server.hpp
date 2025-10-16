
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <locale>
#include <cerrno>
#include <algorithm>

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
#include <csignal>
#include <time.h>

class server
{
	private:
		std::vector<struct pollfd>		_fds;
		std::vector<client>				_clients;
		std::map<std::string, channel>	_channels;
		std::map<std::string, void (server::*)(client*, message&)>	_cmdList;
		std::string						_passWord;
		int								_serverFd;
		char*							_port;

		/*cmd*/
		void	handlePass(client* cli, message& msg);
		void	handleNick(client* cli, message& msg);
		void	handleUser(client* cli, message& msg);
		void	handleJoin(client* cli, message& msg);
		void	handlePart(client* cli, message& msg);
		void	handlePrivmsg(client* cli, message& msg);
		void	handleKick(client* cli, message& msg);
		void	handleInvite(client* cli, message& msg);
		void	handleTopic(client* cli, message& msg);
		void	handleMode(client* cli, message& msg);
		void	handlePing(client* cli, message& msg);
		void	handleQuit(client* cli, message& msg);
		void	handleWho(client* cli, message& msg);
		/*---*/

		void	initCmdServer();

	public:
		server(char* port, char* pwd);
		~server();

		void	initServSocket(char* port);
		void	run();

		std::string	getPassWord() const;
		int			getFd() const;
		char*		getPort() const;
};

#endif
