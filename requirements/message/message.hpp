
#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <iostream>
#include <vector>

class message {

	private:

		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_params;

	
	public:
		
		message(void);
		message(message const & copy);
		message& operator=(message const & rhs);
		~message();



};

#endif