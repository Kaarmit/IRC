#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <list>
#include <time.h>
#include "../message/message.hpp"

class client
{
	private :

		int 						_fd;

		bool 						_registered;
		bool						_toRemove;

		time_t						_connexionTime;

		std::string					_fullMessage;
		std::string					_serverName;
		std::string					_host;
		std::string					_nick;
		std::string   				_user;
		std::string   				_real;
		std::string					_pass;
		std::string					_outbuf;

		message						_msg;

		size_t						_bytesSent;

		std::list<std::string> 		_channels;

		client(void);
		client(const client& copy);
		client&	operator=(const client& rhs);

	public :

		client(int clientFd);
		~client(void);

		int							getFd(void) const;

		bool						getRegistered(void) const;
		void						setRegistered(bool status);

		message						getMessage(void) const;
		message&					getMessage(void);

		std::string					getFullMessage(void) const;
		std::string&				getFullMessage(void);

		std::string					getServerName(void) const;
		void						setServerName(std::string name);

		std::string					getHost(void) const;
		void						setHost(std::string name);

		std::string					getNick(void) const;
		void						setNick(std::string name);

		std::string					getUser(void) const;
		void						setUser(std::string name);

		std::string					getReal(void) const;
		void						setReal(std::string name);

		std::string					getPass(void) const;
		void						setPass(std::string name);

		std::list<std::string>		getChannelList(void) const;
		std::list<std::string>&		getChannelList(void);

		std::string& 				getOutbuf();
		const std::string& 			getOutbuf() const;

    	size_t& 					getBytesSent();
    	size_t 						getBytesSent() const;
		void 						setBytesSent(size_t value);
		
		bool						getToRemove();
		void						setToRemove(bool value);
		
		double						getTime() const;

		void						enqueueLine(const std::string ircLine);
		bool 						hasPending() const;
    	void 						clearIfFlushed();

		bool						operator==(const client& rhs) const;
		bool						operator!=(const client& rhs) const;
};

#endif
