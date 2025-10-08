#include "server.hpp"

/*---------CONSTRUCTOR/DESTRUCTOR-----------*/
server::server(std::string port, std::string mdp) : _passWord(mdp)
{
	std::stringstream oss;
	oss << port;
	oss >> this->_port;

	std::cout << "Initialisation du Serveur IRC" << std::endl;
	this->initSocket(this->_port);
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

/*----------CREATION DU SOCKET-------------*/

int	createSocket(int port)
{
	//on creer toutes les variables dont o aura besoin
	struct addrinfo	hints, *res = NULL, *p = NULL;
	int	status, sockfd = -1, reuse = 1, v6only_off = 0;

	//on init la structure d'addresse
	//en initialisant a 0 avec memset
	//on donne la famille des deux type d'ip(4 et 6)
	//on donne le type de socket qu'on veut, ici prefere TCP
	//et on donne un flag qui dit que les addresses de socket seront bonnes pour bind des socket qui accept des connexions
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//donne une liste chainee d'addr qui respect les options ci-dessus dans res
	status = getaddrinfo(NULL, "6667", &hints, &res);
	if (status != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}

	//ici on va regarder dans la liste chainee quelle addresse on peu utiliser
	for (p = res; p != NULL; p= p->ai_next)
	{
		//creer un socket avec : domain(famille d'addresses) = ipv4, type(de communication) = tcp, protocol = default
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd < 0)
			continue;

		//on configure des options sur le socket sockfd
		//le niveau de l'option est SOL_SOCKET, niveau general du socket, TCP/IP ici
		//l'option qu'on veut activer est SO_REUSEADDR pour  qu'on puisse utiliser la meme addr/port apres une deco rapide
		//la valeur de l'option sera 1
		//et sa taille en octet
		//tout ca pour les addresse ipv6
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		{
			close(sockfd);
			sockfd = -1;
			continue;
		}
		//autorise le socket ipv6 a accepter aussi les connexions ipv4
		//permet d'avoir un seul socket pour deux protocoles
		if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only_off, sizeof(v6only_off)) < 0)
		{
			if (errno != ENOPROTOOPT)
			{
				close(sockfd);
				sockfd = -1;
				continue;
			}
		}
		//on bind le socket a l'addresse qu'on a trouve et on sort de la boucle(“assigning a name to a socket”)
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
		//si le bind a echoue alors on continue
		close(sockfd);
		sockfd = -1;
	}

	//on free le res et derniere verif
	freeaddrinfo(res);
	if (sockfd < 0)
	{
		perror("sockfd");
		exit(1);
	}

	//on met le socket en mode ecoute
	//creer une liste d'attente pour les connexions avec le max de personne dans la queue
	if (listen(sockfd, SOMAXCONN) < 0)
	{
		perror("listen");
		close(sockfd);
		exit(1);
	}

	//on rend le sockfd non bloquant
	int flag = fcntl(sockfd, F_GETFL, 0);
	if (flag < 0 || fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0)
	{
		perror("fcntl");
		close(sockfd);
		exit(1);
	}

	std::cout << "Serveur en ecoute sur le port " << port << std::endl;
	return sockfd;
}

void	server::initSocket(int port)
{
	this->_serverFd = createSocket(port);
}

/*---------------------------------------*/


/*----------PROGRAMME-------------*/

void	server::run()
{
	struct pollfd	serverPoll;
	serverPoll.fd = this->_serverFd;
	serverPoll.events = POLLIN;
	this->_fds.push_back(serverPoll);

	while (true)
	{
		int fd_ready = poll(&this->_fds[0], this->_fds.size(), -1);
		if (fd_ready < 0)
		{
			if (errno == EINTR)
			{
				continue; //et gestion du ctrl+C
			}
			perror("poll");
			break;
		}

		if (this->_fds[0].revents != 0 & POLLIN)
		{
			struct sockaddr_storage	clientAddr;
			socklen_t	addrLen = sizeof(clientAddr);
			int clientFd = accept(this->_serverFd, (struct sockaddr*)&clientAddr, &addrLen);
			if (clientFd < 0)
			{
				perror("accept");
				continue;
			}

			struct pollfd	serverPoll;
			serverPoll.fd = clientFd;
			serverPoll.events = POLLIN;
			this->_fds.push_back(serverPoll);

			//pour ajouter le client au serveur, surement verifier son nick tout ca tout ca
			this->createClient(clientFd);

			std::cout << "Nouvelle connexion ! fd=" << clientFd << std::endl;
		}

		for (int i = 1; i < this->_fds.size(); i++)
		{
			if (this->_fds[i].revents != 0 & POLLIN)
			{
				std::cout << "Message du client fd=" << this->_fds[i].fd << std::endl;
				void*	buffer = this->_clients[i].getBuffer();
				while (read(this->_fds[i].fd, buffer, 512) > 0)
				{
					//envyer le buffer au fonction de parsing
				}
			}
		}
	}

}

/*---------------------------------------*/
