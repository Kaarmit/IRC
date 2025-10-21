
#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <iostream>
#include <vector>

class message {

private:

	std::string							_prefix;
	std::string							_command;
	std::vector<std::string>			_params;

public:

	message();
	message(message const & copy);
	message& operator=(message const & rhs);
	~message();

	std::string						getPrefix(void) const;
	std::string						getCommand(void) const;
	std::string						toIrcLine(void) const;
	std::vector<std::string>		getParams(void) const;
	std::vector<std::string>&		getParams(void);
	void							setParams(std::string input);
	void							setPrefix(std::string input);
	void							setCommand(std::string input);
	void							clearMessage(void);

};

#endif
