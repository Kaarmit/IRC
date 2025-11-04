#include "server/server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Program must have: <port> <password>" << std::endl;
		return -1;
	}

	server	serveur(argv[1], argv[2]);

	return 0;
}
