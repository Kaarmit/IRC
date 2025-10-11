
#include "server.hpp"

/*---------CONSTRUCTOR/DESTRUCTOR-----------*/
server::server(std::string port, std::string mdp) : _passWord(mdp)
{
	std::stringstream oss;
	oss << port;
	oss >> this->_port;

	std::cout << "Initialisation du Serveur IRC" << std::endl;
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

int	server::getPort() const
{
	return this->_port;
}
/*-------------------------*/

/*----------CREATION DU SOCKET DECOUTE-------------*/

int	createServSocket(int port) {
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
	status = getaddrinfo(NULL, "6667", &hints, &res);
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

void	server::initServSocket(int port)
{
	this->_serverFd = createServSocket(port);
}

/*---------------------------------------*/

/*----------PROGRAMME-------------*/

bool	parsing(client& c, char* rawMsg) {
	std::stringstream		ss(rawMsg);



}

void	server::run() {
	struct pollfd	pollingRequestServ;
	pollingRequestServ.fd = this->_serverFd;
	pollingRequestServ.events = POLLIN;
	this->_fds.push_back(pollingRequestServ);

	while (true) {
		int fd_ready = poll(&this->_fds[0], this->_fds.size(), 0);
		if (fd_ready < 0) {
			if (errno == EINTR) // ?
				continue; //et gestion du ctrl+C
			fprintf(stderr, "Erreur de poll(): %s\n", strerror(errno));
			break;
		}
		if (fd_ready > 0) {
			for (int i = 0; i < this->_fds.size(); i++) {
				if (this->_fds[i].revents & POLLIN) {
					if (i == 0) {
						struct sockaddr_storage	clientAddr;
						socklen_t	addrLen = sizeof(clientAddr);
						int clientFd = accept(this->_serverFd, (struct sockaddr*)&clientAddr, &addrLen);
						if (clientFd < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
							fprintf(stderr, "Erreur de accept(): %s\n", strerror(errno));
							continue;
						}

						struct pollfd	pollingRequestClient;
						pollingRequestClient.fd = clientFd;
						pollingRequestClient.events = POLLIN;
						this->_fds.push_back(pollingRequestClient);
						std::cout << "Nouvelle connexion ! fd=" << clientFd << std::endl;

						client	newClient(clientFd);
						this->_clients.push_back(newClient);
					}
					else {
						std::cout << "Message du client fd=" << this->_fds[i].fd << std::endl;
						char*	buffer = NULL;
						if (read(this->_fds[i].fd, buffer, 512) > 0) {
							parsing(this->_clients[i - 1], buffer);//fonction pour parser le message
						}

					}	
				}	
			}
		}
	}
}

/*---------------------------------------*/

