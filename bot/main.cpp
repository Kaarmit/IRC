#include "bot.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Program must have: <ip> <port>" << std::endl;
		return -1;
	}

	signal(SIGINT, handleSigint);
	bot	robot(argv[1], argv[2]);

	return 0;
}
