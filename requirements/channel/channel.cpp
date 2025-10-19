
#include "channel.hpp"

channel::channel(void) {
	this->_name = "";
	this->_clientsList.clear();
	this->_topicStatus = false;
	this->_topic = "";
	this->_limit = -1;
	this->_inviteOnly = false;
	this->_invitedList.clear();
	this->_key = "";
	this->_operatorsList.clear();
	return;
}

channel::channel(std::string name, client creator) {
	this->_name = name;
	this->_clientsList.push_back(creator);
	this->_topicStatus = false;
	this->_topic = "";
	this->_limit = -1;
	this->_inviteOnly = false;
	this->_invitedList.clear();
	this->_key = "";
	this->_operatorsList.push_back(creator);
	return;
}

channel::channel(channel const & copy) {
	*this = copy;
	return;
}

channel& 		channel::operator=(channel const & rhs) {
	if (this != &rhs) {
		this->_name = rhs.getChannelName();
		this->_clientsList = rhs.getClientList();
		this->_topicStatus = rhs.isRestrictedTopic();
		this->_topic = rhs.getTopic();
		this->_limit = rhs.getLimit();
		this->_inviteOnly = rhs.isInviteOnly();
		this->_invitedList = rhs.getInvitedList();
		this->_key = rhs.getKey();
		this->_operatorsList = rhs.getOpList();
	}
	return (*this);
}

channel::~channel(void) {
	return;
}

std::string				channel::getChannelName(void) const {
	return (this->_name);
}

void					channel::setChannelName(std::string newName) {
	this->_name = newName;
	return ;
}

std::list<client>		channel::getClientList(void) const {
	return (this->_clientsList);
}

bool					channel::isRestrictedTopic(void) const {
	return (this->_topicStatus);
}

void					channel::setRestrictedTopic(bool newStatus) {
	this->_topicStatus = newStatus;
}

std::string				channel::getTopic(void) const {
	return (this->_topic);
}

void					channel::setTopic(std::string newTopic) {
	this->_topic = newTopic;
}

int						channel::getLimit(void) const {
	return (this->_limit);
}
void					channel::setLimit(int newLimit) {
	this->_limit = newLimit;
}
bool					channel::isInviteOnly(void) const {
	return (this->_inviteOnly);

}
void					channel::setInviteOnly(bool newStatus) {
	this->_inviteOnly = newStatus;
}

std::list<client>		channel::getInvitedList(void) const {
	return (this->_invitedList);
}

std::string				channel::getKey(void) const {
	return (this->_key);
}

void					channel::setKey(std::string newKey) {
	this->_key = newKey;	
}

std::list<client>		channel::getOpList(void) const {
	return (this->_operatorsList);
}
