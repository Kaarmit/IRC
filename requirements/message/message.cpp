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
	// std::cout << "Message destructor called" << std::endl;

}