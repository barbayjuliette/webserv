/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 17:42:24 by yliew             #+#    #+#             */
/*   Updated: 2024/07/25 16:42:53 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerConfig::ServerConfig() : ValidConfig()
{
	initValidKeys();
}

ServerConfig::ServerConfig(const ServerConfig& other) : ValidConfig(other)
{
	initValidKeys();
}

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
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ServerConfig::initValidKeys(void)
{
	this->_validKeys["listen"] = &ServerConfig::setListenPort;
	this->_validKeys["body_max_length"] = &ServerConfig::setBodyMaxLength;
	this->_validKeys["autoindex"] = &ServerConfig::setAutoindex;
	this->_validKeys["error_page"] = &ServerConfig::setErrorPages;
	this->_validKeys["host"] = &ServerConfig::setHost;
	this->_validKeys["root"] = &ServerConfig::setRoot;
	this->_validKeys["server_name"] = &ServerConfig::setServerName;
	this->_validKeys["index"] = &ServerConfig::setIndex;
	this->_validKeys["redirect"] = &ServerConfig::setRedirect;
	this->_validKeys["allowed_methods"] = &ServerConfig::setAllowedMethods;
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
