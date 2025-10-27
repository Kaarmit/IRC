/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 13:29:55 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/27 15:18:22 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

bool isSpecial(char c)
{
    // RFC: specials = []\`_^{}|
    return c=='[' || c==']' || c=='\\' || c=='`' || c=='_' || c=='^' || c=='{' || c=='}' || c=='|';
}
bool isNickFirst(char c)
{
    return isalpha(c) || isSpecial(c);
}
bool isNickRest(char c)
{
    return isalnum(c) || isSpecial(c) || c=='-';
}

bool isValidNick(const std::string& s)
{
    if (s.empty())
		return false;
    if (!isNickFirst(s[0]))
		return false;
    for (size_t i = 1; i < s.size(); ++i)
	{
        if (!isNickRest(s[i]))
			return false;
	}
    return true;
}

bool isSpecialUser(char c)
{
	return c == '-' || c == '_' || c == '.';
}

bool isValidUsername(const std::string& s)
{
	if (s.empty())
	{
		// std::cout << "LOG: s empty()" << std::endl;
		return false;
	}
	if (s.size() > 25)//ici c'est 10 mais j'ai change sinon Daryl Avril ne passe pas
	{
		// std::cout << "LOG: s > 10" << std::endl;
		return false;
	}
	for (size_t i = 0; i < s.size(); i++)
	{
		if(!isSpecialUser(s[i]) && !isalnum(s[i])) //creer isspecialUser() juste au dessus
		{
			// std::cout << "LOG: s dans for()" << std::endl;
			return false;
		}
	}
	return true;

}

bool channelHasFd(const channel* ch, int fd)
{
    std::list<client*> lst = ch->getClientList();
    for (std::list<client*>::const_iterator it = lst.begin(); it != lst.end(); ++it)
	{
        if ((*it)->getFd() == fd)
			return true;
    }
    return false;
}

// Partagent-ils au moins un channel ? (version map<string, channel>)
bool sharesAChannelByFd(const client* a, const client* b,
                               const std::list<channel*>& channels)
{
    if (!a || !b) return false;
    const int fda = a->getFd();
    const int fdb = b->getFd();
    if (fda == fdb) return true; // même client, cas trivial

    for (std::list<channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (channelHasFd((*it), fda) && channelHasFd((*it), fdb))
            return true;
    }
    return false;
}

// Construit un prefix utilisateur : nick!user@host
std::string userPrefix(const client* c)
{
	std::string prefix;
	if (!(c->getNick().empty()))
		prefix.append(":" + c->getNick());
	if (!(c->getUser().empty()))
		prefix.append("!" + c->getUser());
	if (!(c->getHost().empty()))
		prefix.append("@" + c->getHost());
    return (prefix);
}

client* server::getClientByF(int fd)
{
	for (std::list<client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getFd() == fd)
			return (*it);
	}
	return NULL;
}

client* server::getClientByNick(const std::string& nick)
{
	for (std::list<client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNick() == nick)
			return (*it);
	}
	return NULL;
}

channel* server::getChannelByName(const std::string& name)
{
	for (std::list<channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if ((*it)->getChannelName() == name)
			return (*it);
	}
	return NULL;
}
/*-------------------------*/

 /*-------------------UTILS------------------------*/

bool server::isNickTaken(const std::string& nick) {
    for (std::list<client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        if ((*it)->getNick() == nick) return true;
    return false;
}


// Broadcast NICK (à soi + tous les clients partageant un canal)
void server::broadcastNickChange(client* cli, const std::string& oldNick, const std::string& newNick)
{

	//Daryl: juste pour faire un test de make sorry
	(void)oldNick;

    // Prépare la ligne NICK
    const std::string line = userPrefix(cli) + " NICK :" + newNick + "\r\n";

    // À soi
    cli->enqueueLine(line);
    polloutActivate(cli);

    // À tous les autres clients qui partagent au moins un channel
    for (std::list<client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
        if ((*it)->getFd() == cli->getFd())
            continue;
        if (sharesAChannelByFd(cli, (*it), _channels))
		{
            (*it)->enqueueLine(line);
            polloutActivate((*it));
        }
    }
}

void server::broadcastJoin(client* cli, channel& chan)
{
	std::list<client*> chanCL = chan.getClientList();
	std::string line;
    for (std::list<client*>::iterator itChan = chanCL.begin(); itChan != chanCL.end(); ++itChan)
	{
		polloutActivate(*itChan);
		line = userPrefix(cli) + " JOIN :" + chan.getChannelName() + "\r\n";
		(*itChan)->enqueueLine(line);
        if ((*itChan)->getFd() == cli->getFd())
		{
			if (!chan.getTopic().empty())
			{
				line = this->_serverName + " 332 " + cli->getNick() + " " + chan.getChannelName() + " :" + chan.getTopic() + "\r\n";
				cli->enqueueLine(line);
				line = this->_serverName + " 333 " + cli->getNick() + " " + chan.getChannelName() + " " + chan.getTopicAuthor() + chan.getTopicTimestampStr() + "\r\n"; //convertir de long/time_t a string
				cli->enqueueLine(line);
			}
			for (std::list<client*>::iterator itPrint = chanCL.begin(); itPrint != chanCL.end(); ++itPrint)
			{
				line = this->_serverName + " 353 " + cli->getNick() + " = " + chan.getChannelName() + ":";
				std::string present = (*itPrint)->getNick();
				if (std::find(chan.getOpList().begin(), chan.getOpList().end(), *itPrint) != chan.getOpList().end())
					present.insert(present.begin(), '@');
				line.append(" " + present);
			}
			cli->enqueueLine(line);
			line = this->_serverName + " 366 " + cli->getNick() + " " + chan.getChannelName() + " :End of /NAMES list.\r\n";
			cli->enqueueLine(line);
		}
    }
}

bool	server::isChannel(std::string str) const
{
	return str[0] == '#' || str[0] == '!' || str[0] == '+' || str[0] == '&';
}

// Envoi 001 à l’enregistrement
void server::sendWelcomeIfRegistrationComplete(client* cli)
{
    if (!cli->getRegistered() && !cli->getNick().empty() && !cli->getUser().empty())
    {
        cli->setRegistered(true);

        // Préfixe utilisateur pour le texte (nick!user@host)
        std::string prefix = userPrefix(cli);

        // RPL_WELCOME (001)
        std::string line = ":" + _serverName + " 001 " + cli->getNick()
                         + " :Welcome to the Internet Relay Network !!! " + prefix + "\r\n";

        cli->enqueueLine(line);
        polloutActivate(cli);
    }
}

bool	server::basicChecks(client* cli, message& msg)
{
	const std::string target = cli->getNick().empty() ? "*" : cli->getNick();
	
	if (cli->getRegistered())
	{
		std::string line = ":" + _serverName + " 462 " + target + " :You may not reregister\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans user() a la verif 1" << std::endl;
        return false;
	}
    if (msg.getParams().empty())
    {
        std::string line = ":" + _serverName + " 431 " + target + " :No nickname given\r\n";
        cli->enqueueLine(line);
        polloutActivate(cli);
		// std::cout << "LOG: return false dans nick() a la verif 1" << std::endl;
        return false;
    }
	return true;
}

void server::broadcastToChannel(channel* ch, const std::string& line)
{
    // Diffuse à tous les membres du salon (détectés via channelHasFd)
    for (std::list<client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        client* c = *it;
        if (!c) continue;
        if (channelHasFd(ch, c->getFd()))
        {
            c->enqueueLine(line);
            polloutActivate(c);
        }
    }
}

// Retire le client du salon (de toutes les "rôles"/listes), via channel::remove(fd)
void server::removeClientFromChannel(channel* ch, client* cli)
{
    if (!ch || !cli) return;
    ch->remove(cli);
}

bool server::channelEmpty(channel* ch) const
{
    if (!ch) return true;
    return ch->empty();
}

// Supprime le salon de _channels et libère la mémoire
void server::deleteChannel(channel* ch)
{
    if (!ch) return;
    const std::string& name = ch->name();

    for (std::list<channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (*it == ch || (*it && (*it)->name() == name))
        {
            _channels.erase(it);
            delete ch;
            return;
        }
    }
}

// FONCTION POUR ACTIVER POLLOUT
void	server::polloutActivate(client* cli)
{
	for (size_t i = 0; i < this->_fds.size(); ++i){
		if (this->_fds[i].fd == cli->getFd()) {
            this->_fds[i].events |= POLLOUT;
            break;
		}
	}
	return;
}

