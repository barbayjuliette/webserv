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
	std::map<std::string, LocationConfig*>::iterator	it;
	for (it = _locations.begin(); it != _locations.end(); it++)
	{
		if (it->second)
			delete it->second;
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

/* Function for validating all directives in the map
- Set root first if specified, to ensure that index files are rooted to the correct directory path */
void	ServerConfig::validateKeys(void)
{
	if (_directives.find("root") != _directives.end())
		setRoot(_directives["root"]);

	if (_directives.find("listen") != _directives.end())
		setListenPort(_directives["listen"]);

	for (t_strmap::iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		if (TRACE)
			std::cout << "current key: " << it->first << '\n';

		t_dirmap::iterator found = this->_validKeys.find(it->first);
		if (found == this->_validKeys.end())
			throw InvalidConfigError("Unsupported server directive => " + it->first);
		t_directive	handlerFunction = found->second;
		(this->*handlerFunction)(it->second);
	}

	if (_locations.empty())
		_locations["/"] = new LocationConfig(this);
}

void	ServerConfig::setLocation(const std::string& path, LocationConfig* location)
{
	_locations[path] = location;
}

LocationConfig*	ServerConfig::matchLocation(const std::string& path)
{
	// std::cout << "inside matchLocation: path: " << path << '\n';
	if (_locations.find(path) != _locations.end())
	{
		return (_locations[path]);
	}
	else
	{
		//path matching logic
	}
	return (_locations.begin()->second);
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

std::map<std::string, LocationConfig*>	ServerConfig::getLocations(void)
{
	return (_locations);
}

LocationConfig*	ServerConfig::getLocation(const std::string& path)
{
	if (_locations.find(path) == _locations.end())
		return (NULL);
	return (_locations[path]);
}
