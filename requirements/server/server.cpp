#include "server.hpp"

volatile sig_atomic_t		stopSignal;

/*---------CONSTRUCTOR/DESTRUCTOR-----------*/
server::server(void) : _serverName("myIRCserver")
{
}

server::server(char* port, char* pwd): _port(port), _serverName("serverIRC") {
	std::stringstream ssMdp(pwd);
	ssMdp >> this->_passWord;

	std::cout << "Initialisation du Serveur IRC" << std::endl;
	_serverTime = time(NULL);

	this->initCmdServer();
	if (!this->initServSocket(this->_port))
		return ;
	stopSignal = 0;
	initStopSignal();
	std::cout << "Server pret." << std::endl;
	this->run();
}

server::server(const server& copy)
{
	(void)copy;
	return;
}

server&			server::operator=(const server& rhs)
{
	(void)rhs;
	return (*this);
}

server::~server()
{
	for (std::list<client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete *it;
	_clients.clear();

	for (std::list<channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete *it;
	_channels.clear();

	for (size_t i = 0; i < _fds.size(); ++i)
		close(_fds[i].fd);
	_fds.clear();
	std::cout << "Arret du server IRC, bye bye." << std::endl;
}
/*-------------------------*/

/*---------GETTER-----------*/

std::vector<struct pollfd>		server::getFds() const
{
	return this->_fds;
}

std::vector<struct pollfd>&		server::getFds()
{
	return this->_fds;
}

std::list<client*>&				server::getClients()
{
	return this->_clients;
}

const std::list<client*>&				server::getClients() const
{
	return this->_clients;
}

std::list<channel*>&				server::getChannels()
{
	return this->_channels;
}

const std::list<channel*>&				server::getChannels() const
{
	return this->_channels;
}

std::string	server::getPassWord() const
{
	return this->_passWord;
}

int	server::getServerFd() const
{
	return this->_serverFd;
}

char*	server::getPort() const
{
	return this->_port;
}

std::string	server::getServerTime() const
{
	struct tm datetime = *localtime(&_serverTime);

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

/*-------------------------------------------------*/


void	server::initCmdServer()
{
	this->_cmdList["INVITE"] = &server::handleInvite;
	this->_cmdList["JOIN"] = &server::handleJoin;
	this->_cmdList["KICK"] = &server::handleKick;
	this->_cmdList["LOG"] = &server::handleLog;//test opur afficher des logs
	this->_cmdList["MODE"] = &server::handleMode;
	this->_cmdList["NICK"] = &server::handleNick;
	this->_cmdList["PART"] = &server::handlePart;
	this->_cmdList["PASS"] = &server::handlePass;
	this->_cmdList["PRIVMSG"] = &server::handlePrivmsg;
	this->_cmdList["QUIT"] = &server::handleQuit;
	this->_cmdList["TOPIC"] = &server::handleTopic;
	this->_cmdList["USER"] = &server::handleUser;
}

/*-------------------------------------------------*/

/*----------CREATION DU SOCKET DECOUTE-------------*/

int	createServSocket(char* port) {
	//on creer toutes les variables dont on aura besoin
	int	status;
	int serverFd = -1;
	int reuse = 1;
	int v6only_off = 0;
	struct addrinfo	hints, *res = NULL;

	//on init la structure d'addresse
	//en initialisant a 0 avec memset
	//on donne la famille des deux type d'ip(4 et 6)
	//on donne le type de socket qu'on veut, ici prefere TCP
	//et on donne un flag qui dit que les addresses de socket seront bonnes pour bind des socket qui accept des connexions
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; //uniquement pour le socket server

	// donne une liste chainee d'addr IP qui respecte les options ci-dessus
	// 1. node = addr IP literal ou nom dhote a resoudre; NULL pour serv
	// 2. port ou nom de service
	// 3. structure d'addrinfo avc toutes les options quon veut
	// 4. pointeur sur pointeur ou getaddrinfo() place la liste chaine 9chaque element a un sockaddr)
	status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0) {
		fprintf(stderr, "Erreur de getaddrinfo(): %s\n", gai_strerror(status));
		freeaddrinfo(res);
		return (-1);
	}

	//creer un socket avec : domain(famille d'addresses) = ipv6, type(de communication) = tcp, protocol = default
	serverFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (serverFd < 0) {
		fprintf(stderr, "Erreur de socket(): %s\n", strerror(errno));
		freeaddrinfo(res);
		return (-1);
	}
	//on rend le serverFd non bloquant
	int flag = fcntl(serverFd, F_GETFL, 0);
	if (flag < 0) {
		fprintf(stderr, "Erreur de fcntl() getfl: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		return (-1);
	}
	if (fcntl(serverFd, F_SETFL, flag | O_NONBLOCK) < 0) {
		fprintf(stderr, "Erreur de fcntl() setfl: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		return (-1);
	}
	//on configure des options sur le socket serverFd
	//le niveau de l'option est SOL_SOCKET, niveau general du socket, TCP/IP ici
	//l'option qu'on veut activer est SO_REUSEADDR pour  qu'on puisse utiliser la meme addr/port apres une deco rapide
	//la valeur de l'option sera 1
	//et sa taille en octet
	//tout ca pour les addresse ipv6
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		fprintf(stderr, "Erreur de setsockopt() reuse: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		return (-1);
	}
	//autorise le socket ipv6 a accepter aussi les connexions ipv4
	//permet d'avoir un seul socket pour deux protocoles
	if (setsockopt(serverFd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only_off, sizeof(v6only_off)) < 0) {
		fprintf(stderr, "Erreur de setsockopt() IPv6_only off: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		return (-1);
	}
	//on bind le socket a l'addresse qu'on a trouve (“assigning a name to a socket”)
	if (bind(serverFd, res->ai_addr, res->ai_addrlen) < 0) {
		fprintf(stderr, "Erreur de bind(): %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		return (-1);
	}
	//on free le res et derniere verif
	freeaddrinfo(res);
	if (serverFd < 0) {
		fprintf(stderr, "Erreur: socket corrompu");
		close(serverFd);
		return (-1);
	}
	//on met le socket en mode ecoute
	//creer une liste d'attente pour les connexions avec le max de personne dans la queue
	if (listen(serverFd, SOMAXCONN) < 0)
	{
		fprintf(stderr, "Erreur de listen(): %s\n", strerror(errno));
		close(serverFd);
		return (-1);
	}

	std::cout << "Serveur en ecoute sur le port " << port << std::endl;
	return serverFd;
}

bool	server::initServSocket(char* port)
{
	this->_serverFd = createServSocket(port);
	if (this->_serverFd == -1)
		return false;
	return true;
}

/*---------------------------------------*/

/*----------PARSING-------------*/

// DNS hostname rules: a-z A-Z 0-9 "-" (not consecutive, dont start/end with it) "." (not consecutive) no other special characters
bool	checkServName(std::string name) {

	for (std::string::iterator it = name.begin(); it != name.end(); it++) {
		if (!std::isdigit(*it) && !std::isupper(*it) && !std::islower(*it) && *it != '.' && *it != '-')
			return (false);
	}
	if (std::count(name.begin(), name.end(), '-') != 1 || std::count(name.begin(), name.end(), '.') != 1)
		return (false);
	return (true);
}

bool	checkIdentity(client* c) {
	std::string	prfx = c->getMessage().getPrefix();
	if (!prfx.empty()) {
		std::size_t userIndex = prfx.find_first_of('!');
		std::size_t hostIndex = prfx.find_first_of('@');
		if (userIndex == prfx.npos && hostIndex == prfx.npos) {
			if (prfx.compare(c->getServerName()) != 0 && prfx.compare(c->getNick()) != 0)
				return (false);
		}
		else if (userIndex != prfx.npos) {
			if (c->getNick().compare(prfx.substr(0, userIndex)) != 0)
				return (false);
			if (hostIndex == prfx.npos) {
				if (c->getUser().compare(prfx.substr(userIndex + 1)) != 0)
					return (false);
			}
			else {
				if (c->getUser().compare(prfx.substr(userIndex + 1, hostIndex)) != 0 || c->getHost().compare(prfx.substr(hostIndex + 1)))
					return (false);
			}
		}
		else { //if (hostIndex != prfx.npos)
			if (c->getNick().compare(prfx.substr(0, hostIndex)) != 0 || c->getHost().compare(prfx.substr(hostIndex + 1)) != 0)
				return (false);
		}
	}
	return (true);
}

bool	checkCommand(client* c) {
	std::string cmd = c->getMessage().getCommand();
	for (std::string::iterator it = cmd.begin(); it != cmd.end(); it++) {
		if (!std::isupper(*it))
			return (false);
	}
	return (true);
}

bool	checkParams(client* c) {
	if (c->getMessage().getParams().size() > 15)
		return (false);
	return (true);
}

bool	parsing(client* c, std::string rawMsg) {
	std::istringstream		ss(rawMsg);
	std::string				prfx;
	std::string				cmd;
	std::string				prm;

	// std::cout << "LOG: rawMsg dans parsing() avant de parser: " << rawMsg << std::endl;
	c->getMessage().clearMessage();
	size_t len = rawMsg.size();
	// if (len > 512 || (rawMsg.find("\r\n") != (len-2)))
	if (len > 512)
		return (false);
	if (rawMsg[0] == ':') {
		ss >> prfx;
		c->getMessage().setPrefix(prfx);
		if (!checkIdentity(c))
			return (false);
	}
	ss >> cmd;
	c->getMessage().setCommand(cmd);
	while (ss >> prm) {
		if (prm[0] == ':') {
			std::string	sentence = prm.substr(1);
			while (ss >> prm)
			sentence.append(" " + prm);
			c->getMessage().setParams(sentence);
			break ;
		}
		// std::cout << "LOG: prm avant de setParams(prm): " << prm << std::endl;
		c->getMessage().setParams(prm);
		// std::cout << "LOG: prm.back() du msg dans parsing: " << c->getMessage().getParams().back() << std::endl;
		prm.clear();
	}
	if (!checkCommand(c) || !checkParams(c))
		return (false);
	return (true);
}

/*---------------------------------------*/

/*----------SIGNAUX-------------*/

void		signalHandling(int signum) {
	(void)signum;
	stopSignal = 1;
	return;
}

void	server::initStopSignal(void) {
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signalHandling;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	signal(SIGPIPE, SIG_IGN); // a checker

}

/*---------------------------------------*/

/*----------HELPER FUNCTIONS-------------*/

std::list<client*>::iterator 		server::findClientByFd(std::list<client*>& clients, int fd)
{
    for (std::list<client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->getFd() == fd)
            return it;
    }
    return clients.end();
}

void 								closeAllFds(std::vector<struct pollfd>& fds)
{
	for (std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end(); ++it)
		close(it->fd);
	return ;
}

/*---------------------------------------*/

/*----------PROGRAMME-------------*/

void server::run()
{
    struct pollfd pollingRequestServ;
    pollingRequestServ.fd = this->_serverFd;
    pollingRequestServ.events = POLLIN;
	pollingRequestServ.revents = 0;
    this->_fds.push_back(pollingRequestServ);

    std::list<int> toRemove; // pour supprimer les clients
    std::list<int> toAdd;   // pour ajouter les clients

    while (true)
	{

		if (stopSignal)
		{
			//close tous les fds
			closeAllFds(this->_fds);
			return ;
		}
        int fdReady = poll(this->_fds.data(), this->_fds.size(), 100);
        if (fdReady == -1 && errno != EINTR)
		{
			std::cerr << "Erreur poll():" << strerror(errno) << std::endl;
            // close all fds
			closeAllFds(this->_fds);
			return;
        }
		// std::cout << "LOG: " << fdReady << std::endl;
        if (fdReady <= 0)
			continue;
		for (std::vector<pollfd>::iterator itPollFd = this->_fds.begin(); itPollFd != this->_fds.end(); ++itPollFd)
		{
			client* itClient = this->getClientByFd(itPollFd->fd);//remplacer par ca pour ne pas utiliser un iterator (?)
			if (!itClient && itPollFd != this->_fds.begin())
			{
   				 toRemove.push_back(itPollFd->fd);
   				 continue;
			}
			if (itPollFd->revents & (POLLERR | POLLHUP | POLLNVAL))
    		{
				if (itPollFd == this->_fds.begin()) // erreur sur le socket serv
				{
					std::cerr << "Erreur sur le socket serveur, arret du serveur." << std::endl;
					closeAllFds(this->_fds);
					return;
				}
				std::cerr << "Erreur sur le socket client fd=" << itPollFd->fd << ", deconnexion." << std::endl;
        		toRemove.push_back(itPollFd->fd);
        		continue ;
    		}
			if (itPollFd->revents & POLLIN)
			{
                if (itPollFd == this->_fds.begin()) // signal sur socket serv = nouvelle(s) connexion(s) client
				{
					while (true) // en 'rafale'
					{
                    	struct sockaddr_storage clientAddr;
                    	socklen_t addrLen = sizeof(clientAddr);
						int clientFd;
						do {
							clientFd = accept(this->_serverFd, (struct sockaddr*)&clientAddr, &addrLen);
						} while (clientFd < 0 && errno == EINTR);
						if (clientFd < 0)
						{
							if (errno == EAGAIN || errno == EWOULDBLOCK)
								break; //no more client try to connect
							std::cerr << "Erreur accept():" << strerror(errno) << std::endl;
							continue;
						}
						int flag = fcntl(clientFd, F_GETFL, 0);
                		if (flag < 0)
						{
							std::cerr << "Erreur de fcntl() getfl:" << strerror(errno) << std::endl;
                			close(clientFd);
                			continue;
                		}
                		if (fcntl(clientFd, F_SETFL, flag | O_NONBLOCK) < 0)
						{
							std::cerr << "Erreur de fcntl() setfl:" << strerror(errno) << std::endl;
                			close(clientFd);
                			continue;
						}
                        toAdd.push_back(clientFd);
                        std::cout << "Nouvelle connexion ! fd=" << clientFd << std::endl;
                    }
                }
                else
				{ // signal sur un socket client
                    std::cout << "Message du client fd=" << itPollFd->fd << std::endl;
                    char bufferRecv[513];
                    int ret;
                    do
					{
                        ret = recv(itPollFd->fd, bufferRecv, sizeof(bufferRecv) - 1, 0);
                    } while (ret == -1 && errno == EINTR);
                    switch (ret)
					{
                        case (0): // déconnexion client
                            std::cout << "Client déconnecté fd=" << itPollFd->fd << std::endl;
                            toRemove.push_back(itPollFd->fd);
                            break;
                        case (-1): // erreur recv
                            std::cerr << "Erreur recv() : " << strerror(errno) << std::endl;
                            toRemove.push_back(itPollFd->fd);
                            break;
                        default: // message
                            bufferRecv[ret] = '\0';
							std::string& buffer = (*itClient).getFullMessage();
							buffer += std::string(bufferRecv, ret);
							size_t pos;
							pos = buffer.find("\r\n");
							while ((pos = buffer.find("\r\n")) != std::string::npos)
							{
							    std::string line = buffer.substr(0, pos);
							    buffer.erase(0, pos + 2);
							    std::cout << "Full msg: " << line << std::endl;
							    if (parsing((itClient), line)) // only true if line has \r\n
							    {
									message&	msg = (*itClient).getMessage();
									std::string	cmdName = msg.getCommand();
									// std::cout << "LOG: cmd du msg dans RUN: " << cmdName << std::endl;
									// std::cout << "LOG: prm[0] du msg dans RUN: " << msg.getParams()[0] << std::endl;
									// std::cout << "LOG: prm.back() du msg dans RUN: " << msg.getParams().back() << ", avec size=" << msg.getParams().size()<< std::endl;

									//verif d'auth
									bool	needsAuth = true;
									if (cmdName == "PASS" || cmdName == "USER" || cmdName == "NICK" || cmdName == "QUIT")
										needsAuth = false;
									if (needsAuth && !(*itClient).getRegistered())
									{
										std::string	error = ":server 451 * :Vous n'etes pas enregistre !\r\n";
										send(itClient->getFd(), error.c_str(), error.length(), 0);
										msg.clearMessage();
										break;
									}

									//verif de la cmd et exec
									if (this->_cmdList.find(cmdName) != this->_cmdList.end())
									{
										bool	res;
										res = (this->*_cmdList[cmdName])((itClient), msg);
										// std::cout << "LOG: on passe dans la verif cmd avec res= " << res << std::endl;
										if (cmdName == "PASS" && !res)
										{
											// std::cout << "LOG: on passe dans la verif PASS et !res avec cmdName:" << cmdName << "et res(true == 1 si la cmd a ete exec)= " << res << std::endl;
											toRemove.push_back(itPollFd->fd);
										}
									}
									else
									{
										std::string	error = ":server 421 " + itClient->getNick() + " " + cmdName + " :Commande inconnue\r\n";
										send(itClient->getFd(), error.c_str(), error.length(), 0);
									}
									msg.clearMessage();
								}
                            }
							if (itClient->getToRemove() == true)
								toRemove.push_back(itPollFd->fd);
                            break;

                    }
                }
            }
			// POLLOUT & send
			if (itClient && (itPollFd->revents & POLLOUT))
			{
				while ((*itClient).hasPending())
				{
					const char* base = itClient->getOutbuf().data() + itClient->getBytesSent();
					size_t left = itClient->getOutbuf().size() - itClient->getBytesSent();
					std::cout << "LOG: Envoi réponse au fd=" << itPollFd->fd << std::endl;
					ssize_t n = send(itPollFd->fd, base, left, 0);
					if (n > 0)
					{
						itClient->setBytesSent((itClient)->getBytesSent() + n);
						itClient->clearIfFlushed();
						if (!(itClient->hasPending()))
							itPollFd->events &= ~POLLOUT;
					}
					else
					{
						if (n < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
							toRemove.push_back(itPollFd->fd);
						break;
					}
				}
			}
        }
        // Ajouter les nouveaux clients dans _fds et _clients
        if (!toAdd.empty())
		{
            for (std::list<int>::iterator it = toAdd.begin(); it != toAdd.end(); ++it)
			{
                int clientFd = *it;
                struct pollfd pollingRequestClient;
                pollingRequestClient.fd = clientFd;
                pollingRequestClient.events = POLLIN;
                pollingRequestClient.revents = 0;
                this->_fds.push_back(pollingRequestClient);
				client* newClient = new client(clientFd);
				this->_clients.push_back(newClient);
            }
            toAdd.clear();
        }
        if (!toRemove.empty()) {
            for (std::list<int>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it) // liste de fd a supprimer
			{
                int fd = *it;
				// supprimer le client correspondant de chaque chan auquel il appartient
				for (std::list<channel*>::iterator chIt = this->_channels.begin(); chIt != this->_channels.end(); ++chIt) {
					std::list<client*>::iterator cChIt = findClientByFd((*chIt)->getClientList(), fd);
					std::list<client*>::iterator cinvChIt = findClientByFd((*chIt)->getInvitedList(), fd);
					//broadcast quit intempestif
					if (cChIt != (*chIt)->getClientList().end())
					{
						for (std::list<client*>::iterator castIt = (*chIt)->getClientList().begin(); castIt != (*chIt)->getClientList().end(); ++castIt) 
						{
							if (cChIt != castIt)
							{
								std::string	quitMsg = userPrefix(*cChIt) + " QUIT\r\n";
								send((*castIt)->getFd(), quitMsg.c_str(), quitMsg.size(), 0);
							}
						}
					}
					if (cChIt != (*chIt)->getClientList().end() || cinvChIt != (*chIt)->getInvitedList().end()) 
						(*chIt)->remove(*cChIt);
				}
				// verif si chan vide et suppr le chan
				for (std::list<channel*>::reverse_iterator chIt2 = this->_channels.rbegin(); chIt2 != this->_channels.rend(); ++chIt2) {
					if ((*chIt2)->empty()) {
						delete (*chIt2);
						this->_channels.remove(*chIt2);
					}
				}
                // supprimer le client correspondant du server
				for (std::list<client*>::iterator cServIt = this->_clients.begin(); cServIt != this->_clients.end(); ++cServIt) {
					if ((*cServIt)->getFd() == fd) {
						client* toDel = *cServIt;
						this->_clients.erase(cServIt);
						delete toDel;
						break;
					}
				}
				// supprimer le pollfd correspondant
				for (std::vector<struct pollfd>::iterator pfIt = this->_fds.begin(); pfIt != this->_fds.end(); ++pfIt) {
					if (pfIt->fd == fd) {
						this->_fds.erase(pfIt);
						break;
					}
				}
				close(fd);
            }
            toRemove.clear();
        }
    }
}

/*---------------------------------------*/
