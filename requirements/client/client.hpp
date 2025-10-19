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
		time_t						_connexionTime;
		std::string					_fullMessage;
		message						_msg;
		std::string					_serverName;
		std::string					_host;
		std::string					_nick;
		std::string   				_user;
		std::string   				_real;
		std::string					_pass;
		std::string					_outbuf;
		size_t						_bytesSent;
		std::list<std::string> 		_channels;

		client(void);

	public :

		client(int clientFd);
		client(const client& copy);
		client& 					operator=(const client& rhs);
		~client(void);

		int							getFd(void) const;
		bool						getRegistered(void) const;
		void						setRegistered(bool status);
		message						getMessage(void) const;
		message&					getMessage(void);//modif de Daryl: retourne un ref pour avoir levrai message et pas une cpy
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
		void						enqueueLine(const std::string& ircLine); // ajoute une ligne a la file d attente
		bool 						hasPending() const;                      // reste-t-il des octets à envoyer ?
    	void 						clearIfFlushed();                        // si tout envoyé -> vider && reset bytesSent
		const std::string& 			getOutbuf() const;
    	size_t 						getBytesSent() const;
		std::string& 				getOutbuf();
    	size_t& 					getBytesSent();
		void 						setBytesSent(size_t value);
		double						getTime() const;

		bool						operator==(client & const rhs);
		bool						operator!=(client & const rhs);
};

#endif
