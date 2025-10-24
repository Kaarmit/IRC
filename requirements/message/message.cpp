#include "message.hpp"

message::message() {
	// std::cout << "message default constructor called" << std::endl;

}

message::message(message const & copy) {
	// std::cout << "message copy constructor called" << std::endl;
	*this = copy;
	return ;
}

message& message::operator=(message const & rhs) {
	// std::cout << "message copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
		this->_command = rhs.getCommand();
		this->_prefix = rhs.getPrefix();
		this->_params = rhs.getParams();
	}
	return (*this);
}

message::~message() {
	// std::cout << "message destructor called" << std::endl;
}

void		message::setPrefix(std::string input) {
	this->_prefix = input;
	return ;
}

void		message::setCommand(std::string input) {
	this->_command = input;
	return ;
}

void		message::setParams(std::string input) {
	this->_params.push_back(input);
	return ;
}

void		message::clearMessage(void) {
	this->_prefix.clear();
	this->_command.clear();
	this->_params.clear();
	// for (std::vector<std::string>::iterator it = this->_params.begin(); it != this->_params.end(); it++)
	// 	(*it).clear();
	return ;
}

std::string						message::getPrefix(void) const{
	return (this->_prefix);
}

std::string						message::getCommand(void) const {
	return (this->_command);
}

std::vector<std::string>		message::getParams(void) const {
	return (this->_params);
}

std::vector<std::string>&		message::getParams(void) {
	return (this->_params);
}

std::string						message::toIrcLine() const {
	std::string line;

	if (!_prefix.empty())
		line += ":" + _prefix + " ";
	line += _command;

	for (size_t i = 0; i < _params.size(); ++i) {
		if (i + 1 == _params.size())
			line += " :" +_params[i];
		else
			line += " " + _params[i];
	}
	line += "\r\n";

	if (line.size() > 512) {
		line.resize(510);
		line += "\r\n";
	}
	return line;
}
