/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daavril <daavril@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:05:18 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/08 18:54:56 by daavril          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>

class client
{
  private :

    int _fd;
    std::string _nick;
    std::string _username;
    std::string _realname;
    void* _buffer;
    std::vector<std::string> _channels;
    bool _registered;


  public :
	client();
	~client();

	void*	getBuffer() const;

};

#endif
