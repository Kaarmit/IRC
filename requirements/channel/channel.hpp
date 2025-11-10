#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <sstream>
#include <list>
#include <algorithm>

#include "../client/client.hpp"

class channel
{
	private:
		std::string				_name;
		std::list<client*>		_clientsList;
		std::list<client*>		_invitedList;
		std::list<client*>		_operatorsList;

		std::string				_topic;
		std::string				_topicWho;
		std::string				_key;//mdp du chan

		int						_limit;//limit de user du chan

		bool					_inviteOnly;
		bool					_topicStatus;
		bool					_protectedByKey;
		bool					_hasLimit;

		time_t					_topicTimeStamp;
		channel(void);
		channel(const channel& copy);
		channel&	operator=(const channel& rhs);

		public:

		channel(std::string name, client* creator);
		~channel(void);

		std::string				getChannelName(void) const;
		void					setChannelName(std::string newName);

		std::list<client*>&		getClientList(void);
		const std::list<client*>&		getClientList(void) const;

		std::list<client*>&		getInvitedList(void);
		const std::list<client*>&		getInvitedList(void) const;

		std::list<client*>&		getOpList(void);
		const std::list<client*>&		getOpList(void) const;

		bool					isRestrictedTopic(void) const;
		void					setRestrictedTopic(bool newStatus);

		std::string				getTopic(void) const;
		void					setTopic(std::string newTopic);

		std::string				getTopicAuthor(void) const;
		void					setTopicAuthor(std::string newAuthor);

		time_t					getTopicTimestamp(void) const;
		std::string				getTopicTimestampStr(void) const;
		void					setTopicTimestamp(void);

		bool					isLimited() const;
		int						getLimit(void) const;
		void					setLimit(int newLimit);

		bool					isInviteOnly(void) const;
		void					setInviteOnly(bool newStatus);
		void					addToInviteList(client*);

		bool					isProtected() const;
		std::string				getKey(void) const;
		void					setKey(std::string newKey);


		bool					operator==(const std::string& rhs) const;
		bool					operator!=(const channel& rhs) const;

		bool					isMember(client* cli) const;
		bool					isOperator(client* cli) const;
		void					setOperator(client* cli);
		void					removeOperator(client* cli);

		void 					remove(client *c);
    	// Vrai si plus aucun membre
    	bool 					empty() const;
    	// Nom public du salon (ex: "#42")
    	const std::string& 		name() const;

		std::string				getMode() const;

		//peut faire des fonctions void addMember(client* cli) et void removeMember(client* cli) ici

		int	getNumberOfCli() const;

};

#endif
