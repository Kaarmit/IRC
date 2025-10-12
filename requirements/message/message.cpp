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
		// this->... = rhs.get..();
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
	for (std::vector<std::string>::iterator it = this->_params.begin(); it != this->_params.end(); it++) 
		(*it).clear();
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