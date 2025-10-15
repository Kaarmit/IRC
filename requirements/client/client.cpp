
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

message			client::getMessage(void) const {
	return (this->_msg);
}

bool			client::getRegistered(void) const {
	return (this->_registered);
}

void			client::setRegistered(bool status) {
	this->_registered = status;
}

std::string		client::getServerName(void) const {
	return (this->_serverName);
}

void			client::setServerName(std::string name) {
	this->_serverName = name;
}

std::string					client::getHost(void) const { 
	return (this->_host);
}

void						client::setHost(std::string name) {
	this->_host = name;
}

std::string					client::getNick(void) const {
	return (this->_nick);
}

void						client::setNick(std::string name) {
	this->_nick = name;
}

std::string					client::getUser(void) const {
	return (this->_user);
}

void						client::setUser(std::string name) {
	this->_user = name;
}

std::string					client::getReal(void) const {
	return (this->_real);
}

void						client::setReal(std::string name) {
	this->_real = name;
}

std::vector<std::string>	client::getChannel(void) const {
	return (this->_channels);
}