
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
		std::vector<struct pollfd>									_fds;
		std::list<client>											_clients;
		std::list<channel>											_channels;
		std::map<std::string, bool (server::*)(client*, message&)>	_cmdList;
		std::string													_passWord;
		std::string													_serverName;
		int															_serverFd;
		char*														_port;

		/*Utils*/
		void	polloutActivate(client* cli);
		
		bool	isNickTaken(const std::string& nick);
		void 	sendWelcomeIfRegistrationComplete(client* cli);
		void 	broadcastNickChange(client* cli, const std::string& oldNick, const std::string& newNick);
		
		void	broadcastJoin(client* cli, channel& chan);
		/*---*/
		
		/*cmd*/
		bool	handlePass(client* cli, message& msg);
		bool	handleNick(client* cli, message& msg);
		bool	handleUser(client* cli, message& msg);
		bool	handleJoin(client* cli, message& msg);
		bool	handlePart(client* cli, message& msg);
		bool	handlePrivmsg(client* cli, message& msg);
		bool	handleKick(client* cli, message& msg);
		bool	handleInvite(client* cli, message& msg);
		bool	handleTopic(client* cli, message& msg);
		bool	handleMode(client* cli, message& msg);
		bool	handlePing(client* cli, message& msg);
		bool	handleQuit(client* cli, message& msg);
		bool	handleWho(client* cli, message& msg);
		/*---*/


		void	initCmdServer();

		server(void);

	public:

		server(char* port, char* pwd);
		server(const server& copy);
		server&			operator=(const server& rhs);
		~server();

		void			initServSocket(char* port);
		void			run();

		std::vector<struct pollfd>		getFds() const;
		std::vector<struct pollfd>&		getFds();		
		std::list<client>				getClients() const;
		std::list<client>&				getClients();
		std::list<channel>				getChannels() const;
		std::list<channel>&				getChannels();
		int								getServerFd() const;
		std::string						getPassWord() const;
		char*							getPort() const;
};

#endif
