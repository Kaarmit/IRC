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
#include "../message/message.hpp"

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
		std::list<client*>											_clients;
		std::list<channel*>											_channels;
		std::map<std::string, bool (server::*)(client*, message&)>	_cmdList;
		std::string													_passWord;
		int															_serverFd;
		char*														_port;
		std::string													_serverName;

		/*Utils*/
		void	polloutActivate(client* cli);

		bool	isNickTaken(const std::string& nick);
		void 	sendWelcomeIfRegistrationComplete(client* cli);
		void 	broadcastNickChange(client* cli, const std::string& oldNick, const std::string& newNick);

		void	broadcastJoin(client* cli, channel& chan);

		bool	isChannel(std::string str) const;
		bool	basicChecks(client* cli, message& msg);
		/*---*/

		/*cmd*/
		void	initCmdServer();

		bool	handleInvite(client* cli, message& msg);
		bool	handleJoin(client* cli, message& msg);
		bool	handleKick(client* cli, message& msg);
		bool	handleLog(client* cli, message& msg);
		bool	handleMode(client* cli, message& msg);
		bool	handleNick(client* cli, message& msg);
		bool	handlePart(client* cli, message& msg);
		bool	handlePass(client* cli, message& msg);
		bool	handlePrivmsg(client* cli, message& msg);
		bool	handleQuit(client* cli, message& msg);
		bool	handleTopic(client* cli, message& msg);
		bool	handleUser(client* cli, message& msg);
		/*---*/

		/*signaux*/
		void	initStopSignal(void);
		/*-------*/

		server(void);
		server(const server& copy);
		server&			operator=(const server& rhs);

	public:

		server(char* port, char* pwd);
		~server();

		bool			initServSocket(char* port);
		void			run();

		std::vector<struct pollfd>		getFds() const;
		std::vector<struct pollfd>&		getFds();

		std::list<client*>&				getClients();
		const std::list<client*>&		getClients() const;

		std::list<channel*>&			getChannels();
		const std::list<channel*>&		getChannels() const;

		int								getServerFd() const;
		std::string						getPassWord() const;
		char*							getPort() const;

		client* getClientByF(int fd);
		client* getClientByNick(const std::string& nick);
		channel* getChannelByName(const std::string& name);

		void broadcastToChannel(channel* ch, const std::string& line);
    	//void removeClientFromChannel(channel* ch, client* cli);
    	bool channelEmpty(channel* ch) const;
    	void deleteChannel(channel* ch);
};

bool isOP(client* cli, channel* ch);
bool isSpecial(char c);
bool isNickFirst(char c);
bool isNickRest(char c);
bool isValidNick(const std::string& s);
bool isSpecialUser(char c);
bool isValidUsername(const std::string& s);
bool channelHasFd(const channel* ch, int fd);
bool sharesAChannelByFd(const client* a, const client* b,
                               const std::list<channel*>& channels);
std::string userPrefix(const client* c);

#endif
