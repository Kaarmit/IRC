#include "bot.hpp"

volatile bool	g_running = true;
void	handleSigint(int)
{
	std::cout << "\n[Bot] SIGINT recu, arret du bot..." << std::endl;
	g_running = false;
}

bot::bot(std::string ipAdrr, char* port) : _ipAdrr(ipAdrr), _port(port), _name("JARVIS")
{
	std::cout << "Bot initialization..." << std::endl;
	if (!this->initBot(_port))
		return ;
	this->runBot();
}

bot::~bot()
{
	std::cout << "Bot destruction" << std::endl;
}



/*----------CREATION DU SOCKET DU BOT--------------------------------------------------------------------*/

int	bot::createBotSocket(const char* port)
{
	int	status;
	int fd = -1;
	struct addrinfo	hints, *res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	status = getaddrinfo(this->_ipAdrr.c_str(), port, &hints, &res);
	if (status != 0)
	{
		std::cerr << "Erreur de getaddrinfo() dans le bot:" <<  gai_strerror(status) << std::endl;
		freeaddrinfo(res);
		return (-1);
	}

	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd < 0)
	{
		std::cerr << "Erreur de socket(): " << strerror(errno) << std::endl;
		freeaddrinfo(res);
		return (-2);
	}

	if (connect(fd, res->ai_addr, res->ai_addrlen) < 0)
	{
		std::cerr << "Erreur de connect() dans bot: " << strerror(errno) << std::endl;
		close(fd);
		freeaddrinfo(res);
		return (-3);
	}

	int	flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	freeaddrinfo(res);
	std::cout << "Bot is ready, connected to server: " << this->_ipAdrr << ":" << port << std::endl;
	return fd;
}

bool	bot::initBot(char* port)
{
	this->_botFd = createBotSocket(port);
	std::cout << "LOG, fd =" << _botFd << std::endl;
	if (this->_botFd < 0)
		return false;
	return true;
}
/*-------------------------------------------------------------------------------------------------------*/

std::string	bot::getTime()
{
	time_t	now = time(NULL);
	struct tm datetime = *localtime(&now);

	std::stringstream	oss;
	oss << std::setfill('0')
		<<std::setw(2) << datetime.tm_mday << "/"
		<< std::setw(2) << (datetime.tm_mon + 1) << "/"
		<< (datetime.tm_year + 1900) << " "
		<< std::setw(2) << datetime.tm_hour << ":"
		<< std::setw(2) << datetime.tm_min << ":"
		<< std::setw(2) << datetime.tm_sec;

	return oss.str();
}


void	bot::runBot()
{
	send(this->_botFd, "PASS mdp\r\n", strlen("PASS mdp\r\n"), 0);
	sleep(1);
	send(this->_botFd, "NICK Jarvis\r\n", strlen("NICK Jarvis\r\n"), 0);
	sleep(1);
	send(this->_botFd, "USER Jarvis Jarvis Jarvis :IRC Bot\r\n", strlen("USER Jarvis Jarvis Jarvis :IRC Bot\r\n"), 0);
	sleep(1);
	send(this->_botFd, "JOIN #general\r\n", strlen("JOIN #general\r\n"), 0);

	while (g_running)
	{
		char	buffer[513];
		int		n = recv(this->_botFd, buffer, sizeof(buffer) - 1, 0);

		switch (n)
		{
        	case (0): // déconnexion
                std::cout << "Deconnexion du server" << std::endl;
				g_running = false; break;
            case (-1): // erreur recv
				if (errno == EWOULDBLOCK || errno == EAGAIN)
				{
					usleep(100000);
					continue;
				}
                std::cerr << "Erreur recv() : " << strerror(errno) << std::endl;
				g_running = false; break;
            default: // message
				buffer[n] = '\0';
				std::string	msg(buffer);
				std::cout << "Recus: " << msg << std::endl;
				if (msg.find("!help") != std::string::npos)
					send(this->_botFd, "PRIVMSG #general :!help, !time, !ping\r\n", strlen("PRIVMSG #general :!help, !time, !ping\r\n"), 0);
				else if (msg.find("!time") != std::string::npos)
				{
					std::string reponse = "PRIVMSG #general :" + this->getTime() + "\r\n";
					send(this->_botFd, reponse.c_str(), reponse.length(),0);
				}
				else if (msg.find("!ping") != std::string::npos)
					send(this->_botFd, "PRIVMSG #general :pong\r\n", strlen("PRIVMSG #general :pong\r\n"), 0);
		}
	}
	send(this->_botFd, "PRIVMSG #general :Arret du bot...\r\n", strlen("PRIVMSG #general :Arret du bot...\r\n"), 0);
	send(this->_botFd, "QUIT\r\n", strlen("QUIT\r\n"), 0);
	close(_botFd);
}
