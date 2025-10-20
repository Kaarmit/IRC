
#include "server.hpp"

volatile sig_atomic_t		stopSignal;

/*---------CONSTRUCTOR/DESTRUCTOR-----------*/
server::server(void)
{
}

server::server(char* port, char* pwd): _port(port) {
	std::stringstream ssMdp(pwd);
	ssMdp >> this->_passWord;

	std::cout << "Initialisation du Serveur IRC" << std::endl;
	this->initCmdServer();
	this->initServSocket(this->_port);
	stopSignal = 0;
	initStopSignal();
	std::cout << "Server pret." << std::endl;
	this->run();
}

server::server(const server& copy)
{
	*this = copy;
}

server&			server::operator=(const server& rhs)
{
	if (this != &rhs)
	{
		this->_fds = rhs._fds;
		this->_clients = rhs._clients;
		this->_channels = rhs._channels;
		this->_cmdList = rhs._cmdList;
		this->_passWord = rhs._passWord;
		this->_serverFd = rhs._serverFd;
		this->_port = rhs._port;
	}
	return (*this);
}

server::~server()
{
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

std::list<client>				server::getClients() const 
{
	return this->_clients;
}

std::list<client>&				server::getClients() 
{
	return this->_clients;
}

std::map<std::string, channel>	server::getChannels() const 
{
	return this->_channels;
}

std::map<std::string, channel>&	server::getChannels() 
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

/*-------------------------*/


/*--------------------CMD DU SERVER----------------*/

// user/nick/jsp deja pris ?
bool	server::isTaken(message& msg)
{
	if (msg.getParams()[0] == "USER")
	{
		for (std::list<client>::iterator i = _clients.begin(); i != _clients.end(); ++i)
		{
			if (msg.getParams()[0] == i->getUser())
				return true;
		}
	}
	else if (msg.getParams()[0] == "NICK")
	{
		for (std::list<client>::iterator i = _clients.begin(); i != _clients.end(); ++i)
		{
			if (msg.getParams()[0] == i->getNick())
				return true;
		}
	}
	return false;
}

// FONCTION POUR ACTIVER POLLOUT
void	server::polloutActivate(client* cli)
{
	for (size_t i = 0; i < this->_fds.size(); ++i){
		if (this->_fds[i].fd == cli->getFd()) {
            this->_fds[i].events |= POLLOUT;
            break;
		}
	}
	return;
}

//PASS
bool	server::handlePass(client* cli, message& msg)
{
	//verif si cli deja enregistrer
	if (cli->getRegistered())
	{
		std::string	error = ":server 462 " + cli->getNick() + " :You may not register\r\n";
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}

	//verif si NICK ou USER a ete envoye avant pass
	if (!cli->getNick().empty() || !cli->getUser().empty())
	{
		std::string	error = ":server 462 * :PASS must be sent before NICK/USER \r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}
	//verif si on a tout les params dans PASS
	if(msg.getParams().empty())
	{
		std::string	error = ":server 461 * :Not enough parameters\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}
	//verif du mdp
	if (msg.getParams()[0] != this->getPassWord())
	{
		std::string	error = ":server 464 * :Password incorrect\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}
	//si tout opk, on set le pass a true ou return true
	cli->setPass(msg.getParams()[0]);
	return true;
}


//NICK
bool	server::handleNick(client* cli, message& msg)
{
	if (msg.getParams().empty())
	{
		std::string	error = ":server 431 * :No nickname given\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}

	if(isTaken(msg))
	{
		std::string error = ":server 432 * :Nickname is already in use\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false; 
	}
	
	std::string parsedNick = msg.getParams()[0];
	for (size_t i = 0; i < parsedNick.length(); i++)
	{
		if ((parsedNick[0] <= 'a' || parsedNick[0] >= 'z') && 
				(parsedNick[0] <= 'A' || parsedNick[0] >= 'Z'))
			{
			std::string error = ":server 433 * :Erronoeous nickname\r\n";
			polloutActivate(cli);
			send(cli->getFd(), error.c_str(), error.length(), 0);
			return false; 
			}
		if ((((parsedNick[i] <= 'a' || parsedNick[i] >= 'z') && 
				(parsedNick[i] <= 'A' || parsedNick[i] >= 'Z')) &&
					(parsedNick[i] < '0' || parsedNick[i] > '9')) &&
						parsedNick[i] != '[' && parsedNick[i] != ']' &&
							parsedNick[i] != '\'' && parsedNick[i] != '`' &&
								parsedNick[i] != '_' && parsedNick[i] != '^' &&
									parsedNick[i] != '{'  && parsedNick[i] != '}' &&
										parsedNick[i] != '|')
			{
			std::string error = ":server 433 * :Erronoeous nickname\r\n";
			polloutActivate(cli);
			send(cli->getFd(), error.c_str(), error.length(), 0);
			return false; 
			}
		
	}
	

	cli->setNick(msg.getParams()[0]);
	std::string out = "Your NICK has been saved";
	polloutActivate(cli);
	send(cli->getFd(), out.c_str(), out.length(), 0);
	return true;
}

//USER
bool	server::handleUser(client* cli, message& msg)
{
}
//JOIN
bool	server::handleJoin(client* cli, message& msg)
{
	//check if registered
	if (!cli->getRegistered())
	{
		std::string	error = "\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}	
	std::vector<std::string> params = msg.getParams();
	//check if enough parameters are provided
	if (params.empty())
	{
		std::string	error = "\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}
	if (params.size() > 2)
	{
		std::string	error = "\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}
	// check if "join 0" = leave all channels;
	if (params[0] == "0" && params.size() == 1) {
		//remove client from all channels' client lists
		for (std::map<std::string, channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it) 
		{
			it->second.getClientList().remove(*cli);
			it->second.getOpList().remove(*cli);
		}
		//clear client chan list
		cli->getChannelList().clear();
		//server send msg to client saying he left them all
		//...
		return (true);
	}
	//avec une commande /join #chan1,#chan2 key1,key2 
	//subdiviser params en deux vect chanName, pwd
	std::vector<std::string> chanGiven;
	chanGiven.clear();
	std::vector<std::string> keysGiven;
	keysGiven.clear();
	std::stringstream ss(params[0]);
	std::string item;
	while (std::getline(ss, item, ','))
		chanGiven.push_back(item);
	if (params.size() == 2)
	{
		std::string item2;
		std::stringstream ss2(params[1]);
		while (std::getline(ss2, item2, ','))
			keysGiven.push_back(item2);
	}
	if (params.size() > chanGiven.size() + keysGiven.size())
	{
		std::string	error = "\r\n";
		polloutActivate(cli);
		send(cli->getFd(), error.c_str(), error.length(), 0);
		return false;
	}
	for (size_t i = 0; i < chanGiven.size(); ++i)
	{
		// check if channel name starts with # or & 
		if (chanGiven.empty()) // le channel doit commencer par # & ou +
		{
			std::string	error = "\r\n";
			polloutActivate(cli);
			send(cli->getFd(), error.c_str(), error.length(), 0);
			return false;
		}
		//search for channel name 
		std::map<std::string, channel>::iterator itChan = this->_channels.find(chanGiven[i]);
		if (itChan == this->_channels.end())
		{
			//create the channel if itChan doesn't exist
			//add the client to the channel's client list (channel constructor)
			channel newChannel(chanGiven[i], *cli);
			//add new channel to the server's channel map
			this->_channels[chanGiven[i]] = newChannel;
			//add the channel to the client's channel list
			cli->getChannelList().push_back(chanGiven[i]);
		}
		else
		{
			//check if client is invited
			if (itChan->second.isInviteOnly())
			{
				std::list<client> invitedList = itChan->second.getInvitedList();
				if (std::find(invitedList.begin(), invitedList.end(), *cli) == invitedList.end())
				{
					//not invited can't enter
					std::string	error = "\r\n";
					polloutActivate(cli);
					send(cli->getFd(), error.c_str(), error.length(), 0);
					return false;
				}
			}
			//check if theres a channel key/password
			if (!itChan->second.getKey().empty())
			{
				if (keysGiven.empty() || itChan->second.getKey().compare(keysGiven[i]) != 0)
				{
					std::string	error = "\r\n";
					polloutActivate(cli);
					send(cli->getFd(), error.c_str(), error.length(), 0);	
					return false;
				}
			}
			//check if theres a limit
			if (itChan->second.getLimit() != -1)
			{
				if (itChan->second.getLimit() <= itChan->second.getClientList().size())
				{
					std::string	error = "\r\n";
					polloutActivate(cli);
					send(cli->getFd(), error.c_str(), error.length(), 0);
					return false;
				}
			}
			// if client got invited, remove him from the invited list
			if (!itChan->second.getInvitedList().empty())
				itChan->second.getInvitedList().remove(*cli);
			//add the channel to the client's channel list
			cli->getChannelList().push_back(params[0]);
			//add the client to the channel's client list
			itChan->second.getClientList().push_back(*cli);
		}		
		//send the appropriate message to the client 
		//...
		//send the appropriate message to the channel members
		//...
	}
	return (true);	
}
//PART
bool	server::handlePart(client* cli, message& msg)
{
}
//PRIVMSG
bool	server::handlePrivmsg(client* cli, message& msg)
{
}
//KICK
bool	server::handleKick(client* cli, message& msg)
{
}
//INVITE
bool	server::handleInvite(client* cli, message& msg)
{
}
//TOPIC
bool	server::handleTopic(client* cli, message& msg)
{
}
//MODE
bool	server::handleMode(client* cli, message& msg)
{
}
//PING (optionnel)
bool	server::handlePing(client* cli, message& msg)
{
}
//QUIT
bool	server::handleQuit(client* cli, message& msg)
{
}
//WHO(optionnel)
bool	server::handleWho(client* cli, message& msg)
{
}

void	server::initCmdServer()
{
	this->_cmdList["NICK"] = &server::handleNick;
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
	if (std::count(name.begin(), name.end(), '-') != 1 || std::count(name.begin(), name.end(), '.') != 1)
		return (false);
	return (true);
}

bool	checkIdentity(client* c, std::string prefix) {
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
	std::vector<std::string> prms = c->getMessage().getParams();
	if (prms.size() > 15)
		return (false);
	return (true);
}

bool	parsing(client* c, std::string rawMsg) {
	std::istringstream		ss(rawMsg);
	std::string				prfx;
	std::string				cmd;
	std::string				prm;

	c->getMessage().clearMessage();
	size_t len = rawMsg.size();
	if (len > 512 || (rawMsg.find("\r\n") != (len-2)))
		return (false);
	if (rawMsg[0] == ':') {
		ss >> prfx;
		c->getMessage().setPrefix(prfx);
		if (!checkIdentity(c, prfx))
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
		c->getMessage().setParams(prm);
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

void		initStopSignal(void) {
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signalHandling;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
}

/*---------------------------------------*/

/*----------HELPER FUNCTIONS-------------*/

std::list<client>::iterator 		findClientByFd(std::list<client>& clients, int fd) 
{
    for (std::list<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getFd() == fd) 
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
        int fdReady = poll(&pollingRequestServ, this->_fds.size(), 100);
        if (fdReady == -1 && errno != EINTR)
		{
			std::cerr << "Erreur poll():" << strerror(errno) << std::endl;
            // close all fds
			closeAllFds(this->_fds);
			return;
        }
        if (fdReady <= 0)
			continue;
		for (std::vector<pollfd>::iterator itPollFd = this->_fds.begin(); itPollFd != this->_fds.end(); ++itPollFd)
		{
			std::list<client>::iterator itClient = findClientByFd(this->_clients, itPollFd->fd); // trouve le client a partir du fd dans la liste des clients sinon retourne .end()
			if (itPollFd->revents & (POLLERR | POLLHUP | POLLNVAL))
    		{
				if (itPollFd == this->_fds.begin()) // erreur sur le socket serv
				{
					std::cerr << "Erreur sur le socket serveur, arret du serveur." << std::endl;
					//close tous les fds
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
                    char buffer[513];
                    int ret;
                    do
					{
                        ret = recv(itPollFd->fd, buffer, sizeof(buffer) - 1, 0);
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
                            buffer[ret] = '\0';
                            //si le parsing est bon
                            std::string fragment(buffer);
                            if (!fragment.find("\r\n"))
                            	itClient->getFullMessage().append(" " + fragment);
                            else
							{
                            	if (parsing(&(*itClient), itClient->getFullMessage()))
								{
									//on recup le message parser
									message	msg = itClient->getMessage();
									std::string	cmdName = msg.getCommand();

									//verif d'auth
									bool	needsAuth = true;
									if (cmdName == "PASS" || cmdName == "USER" || cmdName == "NICK" || cmdName == "QUIT")
										needsAuth = false;
									if (needsAuth && !itClient->getRegistered())
									{
										std::string	error = ":server 451 * :Vous n'etes pas enregistre\r\n";
										send(itClient->getFd(), error.c_str(), error.length(), 0);
										msg.clearMessage();
										break;
									}

									//verif de la cmd et exec
									if (this->_cmdList.find(cmdName) != this->_cmdList.end())
									{
										bool	res;
										res = (this->*_cmdList[cmdName])(&(*itClient), msg);
										if (cmdName == "PASS" && !res)//a voir pour les autres cmds aussi
										toRemove.push_back(itPollFd->fd);
									}
									else
									{
										std::string	error = ":server 421 " + itClient->getNick() + " " + cmdName + " :Commande inconnue\r\n";
										send(itClient->getFd(), error.c_str(), error.length(), 0);
									}
									itClient->getFullMessage().clear();
									msg.clearMessage();
								}
                            }
                            break;
                    }
                }
            }
			// POLLOUT & send
			if (itClient != this->_clients.end() && itPollFd->revents & POLLOUT) 
			{
				while (itClient->hasPending())
				{
					const char* base = itClient->getOutbuf().data() + itClient->getBytesSent();
					size_t left = itClient->getOutbuf().size() - itClient->getBytesSent();

					size_t n = send(itPollFd->fd, base, left, 0);
					if (n > 0)
					{
						itClient->setBytesSent(itClient->getBytesSent() + n);
						itClient->clearIfFlushed();
						if (!(itClient->hasPending())) 
							itPollFd->events &= ~POLLOUT;
					}
					else 
					{
						if (n < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
							toRemove.push_back(itPollFd->fd); // mieux gerer cette erreur
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
                this->_fds.push_back(pollingRequestClient);
                client newClient(clientFd);
                this->_clients.push_back(newClient);
            }
            toAdd.clear();
        }
        if (!toRemove.empty()) {
            for (std::list<int>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it) // liste de fd a supprimer
			{
                int fd = *it;
                close(fd);
                // supprimer le pollfd correspondant
				for (std::vector<struct pollfd>::iterator pfIt = this->_fds.begin(); pfIt != this->_fds.end(); ++pfIt) {
					if (pfIt->fd == fd) {
						this->_fds.erase(pfIt);
						break;
					}
				}
                // supprimer le client correspondant
				for (std::list<client>::iterator cIt = this->_clients.begin(); cIt != this->_clients.end(); ++cIt) {
					if (cIt->getFd() == fd) {
						this->_clients.erase(cIt);
						break;
					}
				}
            }
            toRemove.clear();
        }
		// checker si channels vides:
		// 0. iterer sur map serv et check second.size() == 0
		// 1. recup la liste de client du channel 
		// 2. pr chaque client de la liste: client->getChannelList().remove(second.getChannelName())
		// 3. enlever de channel map serv
    }
}

/*---------------------------------------*/
