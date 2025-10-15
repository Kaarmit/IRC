
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

class server
{
	private:
		std::vector<struct pollfd>		_fds;
		std::vector<client>				_clients;
		std::map<std::string, channel>	_channels;
		std::map<std::string, void (server::*)(client*)>	_cmdList;
		std::string						_passWord;
		int								_serverFd;
		char*							_port;

		/*cmd*/
		void	handlePass(client* cli);
		void	handleNick(client* cli);
		void	handleUser(client* cli);
		void	handleJoin(client* cli);
		void	handlePart(client* cli);
		void	handlePrivmsg(client* cli);
		void	handleKick(client* cli);
		void	handleInvite(client* cli);
		void	handleTopic(client* cli);
		void	handleMode(client* cli);
		void	handlePing(client* cli);
		void	handleQuit(client* cli);
		/*---*/

		void	execute(int idx);
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
