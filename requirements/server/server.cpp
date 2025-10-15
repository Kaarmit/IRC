
#include "server.hpp"

/*---------CONSTRUCTOR/DESTRUCTOR-----------*/
server::server(char* port, char* pwd): _port(port) {
	std::stringstream ssMdp(pwd);
	ssMdp >> this->_passWord;

	std::cout << "Initialisation du Serveur IRC" << std::endl;
	this->initCmdServer();
	this->initServSocket(this->_port);
	std::cout << "Server pret." << std::endl;
	this->run();
}

server::~server()
{
	std::cout << "Arret du server IRC, bye bye." << std::endl;
}
/*-------------------------*/


/*---------GETTER-----------*/

std::string	server::getPassWord() const
{
	return this->_passWord;
}

int	server::getFd() const
{
	return this->_serverFd;
}

char*	server::getPort() const
{
	return this->_port;
}
/*-------------------------*/


/*--------------------CMD DU SERVER----------------*/

//PASS

//NICK
void	server::handleNick(client* cli)
{
	cli->setNick(cli->getMessage()->getParams()[0]);
}

//USER

//JOIN

//PART

//PRIVMSG

//KICK

//INVITE

//TOPIC

//MODE

//PING (optionnel)

//QUIT

//WHO(optionnel)

void	server::initCmdServer()
{
	this->_cmdList["NICK"] = &server::handleNick;
}

void	server::execute(int idx)
{
	std::string cmd = this->_clients[idx].getMessage()->getCommand();
	this->_cmdList[cmd](this->_clients[idx]);
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
		exit(1);
	}

	//creer un socket avec : domain(famille d'addresses) = ipv6, type(de communication) = tcp, protocol = default
	serverFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (serverFd < 0) {
		fprintf(stderr, "Erreur de socket(): %s\n", strerror(errno));
		freeaddrinfo(res);
		exit(1);
	}
	//on rend le serverFd non bloquant
	int flag = fcntl(serverFd, F_GETFL, 0);
	if (flag < 0) {
		fprintf(stderr, "Erreur de fcntl() getfl: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		exit(1);
	}
	if (fcntl(serverFd, F_SETFL, flag | O_NONBLOCK) < 0) {
		fprintf(stderr, "Erreur de fcntl() setfl: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		exit(1);
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
		exit(1);
	}
	//autorise le socket ipv6 a accepter aussi les connexions ipv4
	//permet d'avoir un seul socket pour deux protocoles
	if (setsockopt(serverFd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only_off, sizeof(v6only_off)) < 0) {
		fprintf(stderr, "Erreur de setsockopt() IPv6_only off: %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		exit(1);
	}
	//on bind le socket a l'addresse qu'on a trouve (“assigning a name to a socket”)
	if (bind(serverFd, res->ai_addr, res->ai_addrlen) < 0) {
		fprintf(stderr, "Erreur de bind(): %s\n", strerror(errno));
		freeaddrinfo(res);
		close(serverFd);
		exit(1);
	}
	//on free le res et derniere verif
	freeaddrinfo(res);
	if (serverFd < 0) {
		fprintf(stderr, "Erreur: socket corrompu");
		close(serverFd);
		exit(1);
	}
	//on met le socket en mode ecoute
	//creer une liste d'attente pour les connexions avec le max de personne dans la queue
	if (listen(serverFd, SOMAXCONN) < 0)
	{
		fprintf(stderr, "Erreur de listen(): %s\n", strerror(errno));
		close(serverFd);
		exit(1);
	}

	std::cout << "Serveur en ecoute sur le port " << port << std::endl;
	return serverFd;
}

void	server::initServSocket(char* port)
{
	this->_serverFd = createServSocket(port);
}

/*---------------------------------------*/

/*----------PARSING-------------*/

// DNS hostname rules: a-z A-Z 0-9 "-" (not consecutive, dont start/end with it) "." (not consecutive) no other special characters
bool	checkServName(std::string name) {
	
	for (std::string::iterator it = name.begin(); it != name.end(); it++) {
		if (!std::isdigit(*it) && !std::isupper(*it) && !std::islower(*it) && *it != '.' && *it != '-')
			return (false);
	}
	if (std::count(name.begin(), name.end(), "-") != 1 || std::count(name.begin(), name.end(), ".") != 1)
		return (false);
	return (true);
}

void	setIdentity(client* c) {
	std::string	prfx = c->getMessage().getPrefix();
	if (prfx.empty())
		return ;
	std::size_t userIndex = prfx.find_first_of('!');
	std::size_t hostIndex = prfx.find_first_of('@');

	if (userIndex == prfx.npos && hostIndex == prfx.npos) {
		if (checkServName(prfx)) 
			c->setServerName(prfx);
		else
			c->setNick(prfx);
		return ; // :server.name or :nick
	}
	if (userIndex != prfx.npos) {
		c->setNick(prfx.substr(0, userIndex));
		if (hostIndex == prfx.npos)
			c->setUser(prfx.substr(userIndex + 1));	
		else {
			c->setUser(prfx.substr(userIndex + 1, hostIndex));
			c->setHost(prfx.substr(hostIndex + 1));
		}
		return ; // :nick!user or :nick!user@host
	}
	if (hostIndex != prfx.npos) {
		c->setNick(prfx.substr(0, hostIndex));
		c->setHost(prfx.substr(hostIndex + 1));
		return ; // :nick@host
	}
}

bool	checkIdentity(client* c) {
//compare servName if any
//compare nick if any
//compare user if any
//compare host if any
}

bool	onlyDigits(std::string s) {
	for (std::string::iterator it = s.begin(); it != s.end(); it++) {
		if (!std::isdigit(*it))
			return (false);
	}
	return (true);
}

bool	onlyUpperCase(std::string s) {
	for (std::string::iterator it = s.begin(); it != s.end(); it++) {
		if (!std::isupper(*it))
			return (false);
	}
	return (true);
}

bool	checkCommand(client* c) {
	std::string cmd = c->getMessage().getCommand();
	if ((onlyDigits(cmd) && cmd.size() != 3) || !onlyUpperCase(cmd))
		return (false);
	return (true);
}

bool	checkParams(client* c) {
	std::vector<std::string> prms = c->getMessage().getParams();
	if (prms.size() > 15)
		return (false);
	return (true);
}

bool	parsing(client* c, char* rawMsg) {
	std::istringstream		ss(rawMsg);
	std::string	rawMsgStr = ss.str();
	std::string					prfx;
	std::string					cmd;
	std::string					prm;

	c->getMessage().clearMessage();
	size_t len = rawMsgStr.size();
	if (rawMsgStr.find_last_of('\r') != (len-2) && rawMsgStr.find_last_of('\n') != (len-1))
		return (false);
	if (rawMsgStr.front() == ':') {
		ss >> prfx;
		c->getMessage().setPrefix(prfx);
		if (!c->getRegistered()) {
			setIdentity(c);
			if (!c->getNick().empty() && !c->getUser().empty())
				c->setRegistered(true);
		}
		else {
			if (!checkIdentity(c))
				return (false);
		}
	}
	ss >> cmd;
	c->getMessage().setCommand(cmd);
	while(ss >> prm) {
		c->getMessage().setParams(prm);
		prm.clear();
	}
	if (!checkCommand(c) || !checkParams(c))
		return (false);
	return (true);
}

/*---------------------------------------*/

/*----------PROGRAMME-------------*/

void	server::run() {
	struct pollfd	pollingRequestServ;
	pollingRequestServ.fd = this->_serverFd;
	pollingRequestServ.events = POLLIN;
	this->_fds.push_back(pollingRequestServ);

	std::vector<int>	toRemove;//pour supp les clients

	while (true) {
		int fdReady = poll(&this->_fds[0], this->_fds.size(), 100);
		if (fdReady == -1 && errno != EINTR) {
			fprintf(stderr, "Erreur de poll(): %s\n", strerror(errno));
			// close all fds ?
			// free qqc?
			break; // exit() au lieu de break; ?
		}
		if (fdReady > 0) {
			for (int i = 0; i < this->_fds.size(); i++) {
				if (this->_fds[i].revents & POLLIN) {
					if (i == 0) { // signal sur socket serv = nouvelle co client
						struct sockaddr_storage	clientAddr;
						socklen_t	addrLen = sizeof(clientAddr);
						int clientFd;
						do {
							clientFd = accept(this->_serverFd, (struct sockaddr*)&clientAddr, &addrLen);
						} while (clientFd < 0 && errno == EINTR);
						if (clientFd < 0) {
							fprintf(stderr, "Erreur de accept(): %s\n", strerror(errno));
						}
						else {
							int flag = fcntl(clientFd, F_GETFL, 0);
							if (flag < 0) {
								fprintf(stderr, "Erreur de fcntl() getfl: %s\n", strerror(errno));
								close(clientFd);
								continue;
							}
							if (fcntl(clientFd, F_SETFL, flag | O_NONBLOCK) < 0) {
								fprintf(stderr, "Erreur de fcntl() setfl: %s\n", strerror(errno));
								close(clientFd);
								continue;
							}

							struct pollfd	pollingRequestClient;
							pollingRequestClient.fd = clientFd;
							pollingRequestClient.events = POLLIN;
							this->_fds.push_back(pollingRequestClient);

							client	newClient(clientFd);
							this->_clients.push_back(newClient);
							 // tout rajouter a la fin ?						

							std::cout << "Nouvelle connexion ! fd=" << clientFd << std::endl;
						}
					}
					else { // signal sur un socket client
						std::cout << "Message du client fd=" << this->_fds[i].fd << std::endl;
						char buffer[513];
						int ret;
						do {
							ret = recv(this->_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
						} while (ret == -1 && errno == EINTR);
						switch (ret) {
							case (0): // deconnexion client
								std::cout << "Client deconnecte fd=" << this->_fds[i].fd << std::endl;
								toRemove.push_back(i);
								break;
							case (-1)://erreur
								std::cerr << "Erreur recv() : " << strerror(errno) << std::endl;
								toRemove.push_back(i);
								break;
							default://message
								buffer[ret] = '\0';
								if (parsing(&this->_clients[i - 1], buffer)) {
										execute(i - 1);//faire cmme claude un peu

								}
								break;
						}
					}
				}
			}
			//on remove les clients ici en mettant a jour les vector _fds et _clients
			if (!toRemove.empty())
			{
				for (std::vector<int>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it)
				{
					int idx = *it;
					close(this->_fds[idx].fd);
					this->_fds.erase(this->_fds.begin() + idx);
					this->_clients.erase(this->_clients.begin() + (idx - 1));
				}
				toRemove.clear();
			}
		}
	}
}
/*---------------------------------------*/
