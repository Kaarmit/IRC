/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarmitan <aarmitan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:05:18 by aarmitan          #+#    #+#             */
/*   Updated: 2025/10/07 14:10:47 by aarmitan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>

class Client
{
  private :
    
    int _fd;
    std::string _nick;
    std::string _username;
    std::string _realname;
    std::string _buffer;
    std::vector<std::string> _channels;
    bool _registered;
    
  
  public :
  
  
};

#endif