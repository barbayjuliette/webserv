/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 17:42:24 by yliew             #+#    #+#             */
/*   Updated: 2024/07/13 17:42:26 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValidConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerConfig::ServerConfig() : ValidConfig()
{
	std::cout << "ServerConfig was created.\n";
}

// ServerConfig::ServerConfig(const ServerConfig& other) : ValidConfig(other)
// {
// }

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerConfig::~ServerConfig()
{
	for (std::vector<LocationConfig*>::iterator it = this->_locations.begin();
		it != this->_locations.end(); it++)
	{
		delete *it;
	}
	std::cout << "ServerConfig was destroyed.\n";
}

/*
** --------------------------------- METHODS ----------------------------------
*/

std::vector<LocationConfig*>	ServerConfig::getLocations(void)
{
	return (this->_locations);
}

void	ServerConfig::setLocation(LocationConfig* location)
{
	this->_locations.push_back(location);
}
