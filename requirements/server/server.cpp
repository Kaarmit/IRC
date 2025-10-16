
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
void	server::handleNick(client* cli, message& msg)
{
	cli->setNick(msg.getParams()[0]);
	message out;
	out.setCommand("001");
	out.setParams(cli->getNick());
	out.setParams("Welcome to IRC " + cli->getNick());
	std::string line = out.toIrcLine();
	
	cli->enqueueLine(line);
	
	for (size_t i = 0; i < this->_fds.size(); ++i){
		if (this->_fds[i].fd == cli->getFd()) {
            this->_fds[i].events |= POLLOUT;
            break;
		}
	}
}

//USER
void	server::handleUser(client* cli, message& msg)
{
}
//JOIN
void	server::handleJoin(client* cli, message& msg)
{
}
//PART
void	server::handlePart(client* cli, message& msg)
{
}
//PRIVMSG
void	server::handlePrivmsg(client* cli, message& msg)
{
}
//KICK
void	server::handleKick(client* cli, message& msg)
{
}
//INVITE
void	server::handleInvite(client* cli, message& msg)
{
}
//TOPIC
void	server::handleTopic(client* cli, message& msg)
{
}
//MODE
void	server::handleMode(client* cli, message& msg)
{
}
//PING (optionnel)
void	server::handlePing(client* cli, message& msg)
{
}
//QUIT
void	server::handleQuit(client* cli, message& msg)
{
}
//WHO(optionnel)
void	server::handleWho(client* cli, message& msg)
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
	if (std::count(name.begin(), name.end(), "-") != 1 || std::count(name.begin(), name.end(), ".") != 1)
		return (false);
	return (true);
}

void	setIdentity(client* c) {
	std::string	prfx = c->getMessage().getPrefix();
	if (!prfx.empty()) {
		std::size_t userIndex = prfx.find_first_of('!');
		std::size_t hostIndex = prfx.find_first_of('@');

		if (userIndex == prfx.npos && hostIndex == prfx.npos) { // :server.name or :nick
			if (checkServName(prfx))
				c->setServerName(prfx);
			else
				c->setNick(prfx);
		}
		else if (userIndex != prfx.npos) { // :nick!user or :nick!user@host
			c->setNick(prfx.substr(0, userIndex));
				if (hostIndex == prfx.npos)
					c->setUser(prfx.substr(userIndex + 1));
				else {
					c->setUser(prfx.substr(userIndex + 1, hostIndex));
					c->setHost(prfx.substr(hostIndex + 1));
				}
		}
		else { // (hostIndex != prfx.npos) // :nick@host
			c->setNick(prfx.substr(0, hostIndex));
			c->setHost(prfx.substr(hostIndex + 1));
		}
	}
	return ;
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
			if (!checkIdentity(c, prfx))
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

void server::run() {
	
	signal(SIGPIPE, SIG_IGN);
    struct pollfd pollingRequestServ;
    pollingRequestServ.fd = this->_serverFd;
    pollingRequestServ.events = POLLIN;
    this->_fds.push_back(pollingRequestServ);

    std::vector<int> toRemove; // pour supprimer les clients
    std::vector<int> toAdd;    // pour ajouter les clients

    while (true)
	{
        int fdReady = poll(&this->_fds[0], this->_fds.size(), 100);
        if (fdReady == -1 && errno != EINTR)
		{
            fprintf(stderr, "Erreur de poll(): %s\n", strerror(errno));
            // close all fds ?
            // free qqc?
            break; // exit() au lieu de break; ?
        }
        if (fdReady <= 0)
			continue;
        for (int i = 0; i < this->_fds.size(); i++)
		{
            if (this->_fds[i].revents && POLLIN)
			{
                if (i == 0) // signal sur socket serv = nouvelle(s) connexion(s) client
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
							fprintf(stderr, "No more clients wants to connect: %s\n", strerror(errno)); //other err
							continue;
						}
						int flag = fcntl(clientFd, F_GETFL, 0);
                		if (flag < 0)
						{
                			fprintf(stderr, "Erreur de fcntl() getfl: %s\n", strerror(errno));
                			close(clientFd);
                			continue;
                		}
                		if (fcntl(clientFd, F_SETFL, flag | O_NONBLOCK) < 0)
						{
                			fprintf(stderr, "Erreur de fcntl() setfl: %s\n", strerror(errno));
                			close(clientFd);
                			continue;
						}
                        toAdd.push_back(clientFd);
                        std::cout << "Nouvelle connexion ! fd=" << clientFd << std::endl;
                    }
                }
                else
				{ // signal sur un socket client
                    std::cout << "Message du client fd=" << this->_fds[i].fd << std::endl;
                    char buffer[513];
                    int ret;
                    do
					{
                        ret = recv(this->_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    } while (ret == -1 && errno == EINTR);
                    switch (ret)
					{
                        case (0): // déconnexion client
                            std::cout << "Client déconnecté fd=" << this->_fds[i].fd << std::endl;
                            toRemove.push_back(i);
                            break;
                        case (-1): // erreur recv
                            std::cerr << "Erreur recv() : " << strerror(errno) << std::endl;
                            toRemove.push_back(i); // t sur ? -> oui je crois, c'est le i du fd qui ici i != 0 jamais donc sur que c'est pas server on on supp le bon cli
                            break;
                        default: // message
                            buffer[ret] = '\0';
                            if (parsing(&this->_clients[i - 1], buffer))
							{
								//on recup le message parser
								message	msg = this->_clients[i - 1].getMessage();
								std::string	cmdName = msg.getCommand();
								//on verif si la cmd existe
								if (this->_cmdList.find(cmdName) != this->_cmdList.end())
								{
									//on execute
									(this->*_cmdList[cmdName])(&this->_clients[i - 1], msg);
								}
								else
								{
									std::cerr << "Commande inconnue: " << cmdName << std::endl;
									//envoie de l'erreur au client aussi
								}
								msg.clearMessage();
								}
							break;
                    }
                }
            }
        }
		// POLLOUT & send
		for (int i = 0; i < this->_fds.size(); i++)
		{
            if (!(this->_fds[i].revents & POLLOUT))
				continue;
			client& c = this->_clients[i - 1];
			while (c.hasPending()){
				const char* base = c.getOutbuf().data() + c.getBytesSent();
				size_t left = c.getOutbuf().size() - c.getBytesSent();
				
				size_t n = send(this->_fds[i].fd, base, left, 0);
				if (n > 0){
					c.setBytesSent(c.getBytesSent() + n);
					c.clearIfFlushed();
					if (!(c.hasPending())) {
						this->_fds[i].events &= ~POLLOUT;
					}
				}
				else {
					if (n < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
						toRemove.push_back(i); // mieux gerer cette erreur
					}
				}
				break;
			}
		}
        // Ajouter les nouveaux clients dans _fds et _clients
        if (!toAdd.empty())
		{
            for (std::vector<int>::iterator it = toAdd.begin(); it != toAdd.end(); ++it)
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
        // Supprimer les clients
        if (!toRemove.empty()) {
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
/*---------------------------------------*/


// void server::run() {
//     struct pollfd pollingRequestServ;
//     pollingRequestServ.fd = this->_serverFd;
//     pollingRequestServ.events = POLLIN;
//     this->_fds.push_back(pollingRequestServ);

//     std::vector<int> toRemove;
//     std::vector<int> toAdd;

//     for (;;) {
//         int fdReady = poll(&this->_fds[0], this->_fds.size(), 100);
//         if (fdReady == -1 && errno != EINTR) {
//             fprintf(stderr, "poll(): %s\n", strerror(errno));
//             break;
//         }
//         if (fdReady <= 0) continue;

//         for (size_t i = 0; i < this->_fds.size(); i++) {
//             short rev = this->_fds[i].revents;
//             if (!rev) continue;

//             // erreurs/fin: marque pour suppression
//             if (rev & (POLLHUP | POLLERR | POLLNVAL)) {
//                 if (i != 0) toRemove.push_back((int)i);
//                 continue;
//             }
//             if (!(rev & POLLIN)) continue;

//             if (i == 0) {
//                 // Accepter en rafale avec accept() + fcntl()
//                 for (;;) {
//                     struct sockaddr_storage clientAddr;
//                     socklen_t addrLen = sizeof clientAddr;
//                     int clientFd;
//                     do {
//                         clientFd = accept(this->_serverFd, (struct sockaddr*)&clientAddr, &addrLen);
//                     } while (clientFd < 0 && errno == EINTR);

//                     if (clientFd < 0) {
//                         if (errno == EAGAIN || errno == EWOULDBLOCK) break; // backlog vidé
//                         // erreur transitoire/conn abort: log et tente suivant
//                         fprintf(stderr, "accept(): %s\n", strerror(errno));
//                         continue;
//                     }

//                     // Rendre non bloquant et CLOEXEC immédiatement
//                     int fl = fcntl(clientFd, F_GETFL, 0);
//                     if (fl == -1 || fcntl(clientFd, F_SETFL, fl | O_NONBLOCK) == -1) {
//                         fprintf(stderr, "fcntl(set O_NONBLOCK): %s\n", strerror(errno));
//                         close(clientFd);
//                         continue;
//                     }
//                     int fdfl = fcntl(clientFd, F_GETFD, 0);
//                     if (fdfl == -1 || fcntl(clientFd, F_SETFD, fdfl | FD_CLOEXEC) == -1) {
//                         fprintf(stderr, "fcntl(set FD_CLOEXEC): %s\n", strerror(errno));
//                         close(clientFd);
//                         continue;
//                     }

//                     toAdd.push_back(clientFd);
//                     std::cout << "Nouvelle connexion ! fd=" << clientFd << std::endl;
//                 }
//             } else {
//                 // Lecture non bloquante: drainer jusqu’à EAGAIN/EWOULDBLOCK
//                 int cfd = this->_fds[i].fd;
//                 for (;;) {
//                     char buffer[513];
//                     ssize_t ret;
//                     do {
//                         ret = recv(cfd, buffer, sizeof(buffer) - 1, 0);
//                     } while (ret == -1 && errno == EINTR);

//                     if (ret > 0) {
//                         buffer[ret] = '\0';
//                         if (parsing(&this->_clients[i - 1], buffer))
//                             execute((int)i - 1);
//                         continue; // tenter de vider le socket
//                     }
//                     if (ret == 0) { // fermeture pair
//                         std::cout << "Client déconnecté fd=" << cfd << std::endl;
//                         toRemove.push_back((int)i);
//                         break;
//                     }
//                     if (errno == EAGAIN || errno == EWOULDBLOCK) break; // rien d’autre
//                     std::cerr << "recv(): " << strerror(errno) << std::endl;
//                     toRemove.push_back((int)i);
//                     break;
//                 }
//             }
//         }

//         // Ajouter clients: plus de fcntl ici (déjà fait après accept)
//         if (!toAdd.empty()) {
//             for (std::vector<int>::reverse_iterator it = toAdd.rbegin(); it != toAdd.rend(); ++it) {
//                 int clientFd = *it;
//                 struct pollfd p; p.fd = clientFd; p.events = POLLIN;
//                 this->_fds.push_back(p);
//                 client newClient(clientFd);
//                 this->_clients.push_back(newClient);
//             }
//             toAdd.clear();
//         }

//         // Supprimer clients en ordre décroissant
//         if (!toRemove.empty()) {
//             for (std::vector<int>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
//                 int idx = *it;
//                 close(this->_fds[idx].fd);
//                 this->_fds.erase(this->_fds.begin() + idx);
//                 this->_clients.erase(this->_clients.begin() + (idx - 1));
//             }
//             toRemove.clear();
//         }
//     }
// }

/*---------------------------------------*/
