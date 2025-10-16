
#include "client.hpp"

client::client()
{
}

client::client(int clientFd): _fd(clientFd) {
	this->_registered = false;
	this->_connexionTime = time(NULL);
}

client::~client()
{
}

int				client::getFd(void) const {
	return (this->_fd);
}

message&			client::getMessage(void) {
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

void						client::enqueueLine(const std::string& ircLine) {
	_outbuf += ircLine;
}

bool						client::hasPending() const {
	return _bytesSent < _outbuf.size();
}

void						client::clearIfFlushed() {
	if (_bytesSent >= _outbuf.size()) {
		_outbuf.clear();
		_bytesSent = 0;
	}
}

const std::string& client::getOutbuf() const {
	return _outbuf;
}

size_t client::getBytesSent() const {
	return _bytesSent;
}

std::string& client::getOutbuf() {
	return _outbuf;
}

size_t& client::getBytesSent() {
	return _bytesSent;
}

void client::setBytesSent(size_t value) {
	_bytesSent = value;
}

double	client::getTime() const
{
	return this->_connexionTime;
}
