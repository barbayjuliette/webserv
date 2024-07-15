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

#include "ServerConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerConfig::ServerConfig() : ValidConfig()
{
	initValidKeys();
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

void	ServerConfig::initValidKeys(void)
{
	this->_validKeys["listen"] = &ServerConfig::setListenPort;
	this->_validKeys["client_max_body_size"] = &ServerConfig::setClientMaxBodySize;
	this->_validKeys["autoindex"] = &ServerConfig::setAutoindex;
	this->_validKeys["error_page"] = &ServerConfig::setErrorPages;
}

void	ServerConfig::setLocation(LocationConfig* location)
{
	this->_locations.push_back(location);
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

std::vector<LocationConfig*>	ServerConfig::getLocations(void)
{
	return (this->_locations);
}
