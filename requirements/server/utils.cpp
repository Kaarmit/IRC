/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 13:29:55 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/26 15:57:18 by aarmitan         ###   ########.fr       */
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