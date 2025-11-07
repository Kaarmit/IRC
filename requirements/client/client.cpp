
#include "client.hpp"

client::client(void)
{
}

client::client(int clientFd): _fd(clientFd) {
	this->_registered = false;
	this->_toRemove = false;
	this->_fullMessage = "";
	this->_msg.clearMessage();
	this->_serverName = "";
	this->_host = "";
	this->_nick = "";
	this->_user = "";
	this->_real = "";
	this->_pass = "";
	this->_outbuf = "";
	this->_bytesSent = 0;
	this->_channels.clear();
	this->_connexionTime = time(NULL); // a revoir
	return;
}

client::client(const client& copy)
{
	(void)copy;
}

client& 					client::operator=(const client& rhs)
{
	(void)rhs;
	return (*this);
}

client::~client(void)
{
}

int							client::getFd(void) const
{
	return (this->_fd);
}

message&					client::getMessage(void)
{
	return (this->_msg);
}

message						client::getMessage(void) const
{
	return (this->_msg);
}

std::string					client::getFullMessage(void) const
{
	return (this->_fullMessage);
}

std::string&				client::getFullMessage(void)
{
	return (this->_fullMessage);
}

bool						client::getRegistered(void) const {
	return (this->_registered);
}

void						client::setRegistered(bool status)
{
	this->_registered = status;
}

std::string					client::getServerName(void) const
{
	return (this->_serverName);
}

void						client::setServerName(std::string name)
{
	this->_serverName = name;
}

std::string					client::getHost(void) const
{
	return (this->_host);
}

void						client::setHost(std::string name) {
	this->_host = name;
}

std::string					client::getNick(void) const
{
	return (this->_nick);
}

void						client::setNick(std::string name)
{
	this->_nick = name;
}

std::string					client::getUser(void) const
{
	return (this->_user);
}

void						client::setUser(std::string name) {
	this->_user = name;
}

std::string					client::getReal(void) const
{
	return (this->_real);
}

void						client::setReal(std::string name) {
	this->_real = name;
}

std::string					client::getPass(void) const
{
	return this->_pass;
}

void						client::setPass(std::string name)
{
	this->_pass = name;
}

std::list<std::string>		client::getChannelList(void) const
{
	return (this->_channels);
}

std::list<std::string>&		client::getChannelList(void)
{
	return (this->_channels);
}

void						client::enqueueLine(const std::string ircLine)
{
	_outbuf += ircLine;
}

bool						client::hasPending() const
{
	return _bytesSent < _outbuf.size();
}

void						client::clearIfFlushed()
{
	if (_bytesSent >= _outbuf.size()) {
		_outbuf.clear();
		_bytesSent = 0;
	}
}

const std::string& client::getOutbuf() const
{
	return _outbuf;
}

size_t client::getBytesSent() const
{
	return _bytesSent;
}

std::string& client::getOutbuf()
{
	return _outbuf;
}

size_t& client::getBytesSent() {
	return _bytesSent;
}

void client::setBytesSent(size_t value)
{
	_bytesSent = value;
}

bool client::getToRemove()
{
	return _toRemove;
}
void	client::setToRemove(bool value)
{
	this->_toRemove = value;
}

double	client::getTime() const
{
	return this->_connexionTime;
}

bool						client::operator==(const client& rhs) const
{
	if (this == &rhs || this->_fd == rhs.getFd())
		return (true);
	return (false);
}

bool						client::operator!=(const client& rhs) const
{
	return (!(*this == rhs));
}
