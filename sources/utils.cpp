/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 22:19:19 by agan              #+#    #+#             */
/*   Updated: 2024/07/08 22:19:22 by agan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::string ipToString(uint32_t ip)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    
    std::ostringstream ss;
    ss << static_cast<int>(bytes[0]) << "."
       << static_cast<int>(bytes[1]) << "."
       << static_cast<int>(bytes[2]) << "."
       << static_cast<int>(bytes[3]);
    
    return ss.str();
}
