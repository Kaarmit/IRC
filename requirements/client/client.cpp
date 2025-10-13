
#include "client.hpp"

client::client()
{
}

client::client(int clientFd): _fd(clientFd) {
	this->_registered = false;
}

client::~client()
{
}

int				client::getFd(void) const {
	return (this->_fd);
}

message*		client::getMessage(void) const {
	return (this->_msg);
}

bool			client::getRegistered(void) const {
	return (this->_registered);
}

void			client::setRegistered(bool status) {
	this->_registered = status;
}