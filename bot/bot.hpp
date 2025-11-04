#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <ctime>
#include <iomanip>

#include "../requirements/server/server.hpp"

class bot
{
public:
    bot(std::string ipAdrr, char* port);
    ~bot();

private:
	std::string		_ipAdrr;
	char*			_port;
	std::string		_name;

	int				_botFd;

	bool	initBot(char* port);
	int		createBotSocket(const char* port);
	void	runBot();
	std::string	getTime();
};

void	handleSigint(int);
