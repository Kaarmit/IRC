/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistierl <aistierl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:47:06 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/20 13:53:12 by aistierl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>

#include "../client/client.hpp"

class channel
{
	private:
		std::string				_name;
		std::list<client>		_clientsList;
		bool					_topicStatus;
		std::string				_topic;
		int						_limit;
		bool					_inviteOnly;
		std::list<client>		_invitedList;
		std::string				_key;
		std::list<client>		_operatorsList;
		channel(void);

	public:

		channel(std::string name, client creator);
		channel(channel const & copy);
		channel& 		operator=(channel const & rhs);
		~channel(void);

		std::string				getChannelName(void) const;
		void					setChannelName(std::string newName);
		std::list<client>&		getClientList(void);
		std::list<client> 		getClientList(void) const;
		bool					isRestrictedTopic(void) const;
		void					setRestrictedTopic(bool newStatus);
		std::string				getTopic(void) const;
		void					setTopic(std::string newTopic);
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

		bool						operator==(channel & const rhs);
		bool						operator!=(channel & const rhs);					

};


#endif
