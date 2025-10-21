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
		std::list<client>		_clientsList;
		bool					_topicStatus;
		std::string				_topic;
		client*					_topicWho;
		time_t					_topicTimeStamp;
		int						_limit;
		bool					_inviteOnly;
		std::list<client>		_invitedList;
		std::string				_key;
		std::list<client>		_operatorsList;
		channel(void);

	public:

		channel(std::string name, client& creator);
		channel(channel const & copy);
		channel& 				operator=(channel const & rhs);
		~channel(void);

		std::string				getChannelName(void) const;
		void					setChannelName(std::string newName);

		std::list<client>&		getClientList(void);
		std::list<client>		getClientList(void) const;

		bool					isRestrictedTopic(void) const;
		void					setRestrictedTopic(bool newStatus);

		std::string				getTopic(void) const;
		void					setTopic(std::string newTopic);
		client*					getTopicAuthor(void) const;
		void					setTopicAuthor(client* newAuthor);
		time_t					getTopicTimestamp(void) const;
		std::string				getTopicTimestampStr(void) const;
		void					setTopicTimestamp(void);
		int						getLimit(void) const;
		void					setLimit(int newLimit);

		bool					isInviteOnly(void) const;
		void					setInviteOnly(bool newStatus);

		std::list<client>&		getInvitedList(void);
		std::list<client>		getInvitedList(void) const;

		std::string				getKey(void) const;
		void					setKey(std::string newKey);

		std::list<client>&		getOpList(void);
		std::list<client>		getOpList(void) const;

		bool						operator==(const std::string& rhs) const;
		bool						operator!=(const channel& rhs) const;

		bool					isMember(client* cli) const;

};


#endif
