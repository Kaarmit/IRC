
#include "channel.hpp"

channel::channel(void)
{
}

channel::channel(std::string name, client* creator)
{
	this->_name = name;
	if (creator)
	{
		this->_clientsList.push_back(creator);
		this->_operatorsList.push_back(creator);
	}
	this->_topicStatus = false;
	this->_topic.clear();
	this->_topicTimeStamp = 0;
	this->_limit = -1;
	this->_inviteOnly = false;
	this->_invitedList.clear();
	this->_key.clear();
	this->_topicWho.clear();
	return;
}

channel::channel(channel const & copy)
{
  (void)copy;
}

channel& channel::operator=(channel const & rhs)
{
   (void)rhs;
    return *this;
}

channel::~channel(void)
{
	return;
}

std::string				channel::getChannelName(void) const
{
	return (this->_name);
}

void					channel::setChannelName(std::string newName)
{
	this->_name = newName;
	return ;
}

std::list<client*>&		channel::getClientList(void)
{
	return this->_clientsList;
}

const std::list<client*>&		channel::getClientList(void) const
{
	return this->_clientsList;
}

bool					channel::isRestrictedTopic(void) const
{
	return (this->_topicStatus);
}

void					channel::setRestrictedTopic(bool newStatus)
{
	this->_topicStatus = newStatus;
}

std::string				channel::getTopic(void) const
{
	return (this->_topic);
}

void					channel::setTopic(std::string newTopic)
{
	this->_topic = newTopic;
}

std::string					channel::getTopicAuthor(void) const
{
	return (this->_topicWho);
}

void					channel::setTopicAuthor(std::string newAuthor)
{
	this->_topicWho = newAuthor;
}

time_t					channel::getTopicTimestamp(void) const
{
	return (this->_topicTimeStamp);
}

std::string				channel::getTopicTimestampStr(void) const
{
	std::ostringstream oss;
    oss << this->_topicTimeStamp;
    return (oss.str());
}

void					channel::setTopicTimestamp(void)
{
	this->_topicTimeStamp = time(NULL);
}

int						channel::getLimit(void) const
{
	return (this->_limit);
}
void					channel::setLimit(int newLimit)
{
	this->_limit = newLimit;
}

bool					channel::isInviteOnly(void) const
{
	return (this->_inviteOnly);

}

void					channel::setInviteOnly(bool newStatus)
{
	this->_inviteOnly = newStatus;
}

std::list<client*>&		channel::getInvitedList(void)
{
	return (this->_invitedList);
}

std::string				channel::getKey(void) const
{
	return (this->_key);
}

void					channel::setKey(std::string newKey)
{
	this->_key = newKey;
}

std::list<client*>&		channel::getOpList(void)
{
	return (this->_operatorsList);
}

bool					channel::operator==(const std::string& rhs) const
{
	return this->_name == rhs;
}

bool					channel::operator!=(const channel& rhs) const
{
	return (!(*this == rhs.getChannelName()));
}

bool	channel::isMember(client* cli) const
{
	return std::find(this->_clientsList.begin(), this->_clientsList.end(), cli) != this->_clientsList.end();
}

bool	channel::isOperator(client* cli) const
{
	return std::find(this->_operatorsList.begin(), this->_operatorsList.end(), cli) != this->_operatorsList.end();
}

const std::string& channel::name() const
{
	return _name;
}

static void removeFromList(std::list<client*>& lst, client* c)
{
    if (!c)
        return;
    for (std::list<client*>::iterator it = lst.begin(); it != lst.end(); ++it)
    {
        if (*it == c)
        {
            lst.erase(it);
            break;
        }
    }
}

void channel::remove(client* c)
{
	if (!c) return;
    removeFromList(_clientsList, c);
    removeFromList(_operatorsList, c);
    removeFromList(_invitedList, c);
}

bool channel::empty() const
{
    return _clientsList.empty();
}

void	channel::addToInviteList(client* cli)
{
	if (!cli)
		return;
	if (std::find(this->_invitedList.begin(), this->_invitedList.end(), cli) == this->_invitedList.end())
		this->_invitedList.push_back(cli);
}
