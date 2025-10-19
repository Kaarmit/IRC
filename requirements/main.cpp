#include "server/server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Program must have: <port> <password>" << std::endl;
		return -1;
	}

	server	serveur(argv[1], argv[2]);

	/*-------TEST-------------*/
	std::cout << serveur.getPassWord() << std::endl;
	std::cout << serveur.getServerFd() << std::endl;
	std::cout << serveur.getPort() << std::endl;
	/*-----------------------*/
	return 0;
}
