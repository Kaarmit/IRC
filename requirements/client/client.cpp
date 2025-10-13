
#include "client.hpp"

client::client()
{
}

client::client(int clientFd): _fd(clientFd) {
}

client::~client()
{
}

message*		client::getMessage(void) const {
	return (this->_msg);
}
