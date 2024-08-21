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
	this->_validKeys["listen"] = &ServerConfig::parseListenPort;
	this->_validKeys["body_max_length"] = &ServerConfig::parseBodyMaxLength;
	this->_validKeys["autoindex"] = &ServerConfig::parseAutoindex;
	this->_validKeys["error_page"] = &ServerConfig::parseErrorPages;
	this->_validKeys["host"] = &ServerConfig::parseHost;
	this->_validKeys["root"] = &ServerConfig::parseRoot;
	this->_validKeys["server_name"] = &ServerConfig::parseServerName;
	this->_validKeys["index"] = &ServerConfig::parseIndex;
	this->_validKeys["redirect"] = &ServerConfig::parseRedirect;
	this->_validKeys["allowed_methods"] = &ServerConfig::parseAllowedMethods;
}

/* Function for validating all directives in the map
- Set root first if specified, to ensure that index files are rooted to the correct directory path */
void	ServerConfig::validateKeys(void)
{
	if (_directives.find("root") != _directives.end())
		parseRoot(_directives["root"]);

	if (_directives.find("listen") != _directives.end())
		parseListenPort(_directives["listen"]);

	for (t_strmap::iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		if (it->first == "root" || it->first == "listen")
			continue;

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

/* Location matching:
- If CGI block is defined: check request path for valid CGI extension
- Iterate over all location blocks, store all with matching prefix
- If there are multiple matches, return the location with the longest matching prefix */
LocationConfig*	ServerConfig::matchLocation(const std::string& path)
{
	if (TRACE)
		std::cout << "inside matchLocation: path: " << path << '\n';

	std::map<size_t, LocationConfig*>	match_result;

	std::map<std::string, LocationConfig*>::iterator	it;
	for (it = _locations.begin(); it != _locations.end(); it++)
	{
		if (it->second->isCGILocation() && it->second->compareExtension(path))
			return (it->second);

		size_t	cmp = it->second->comparePath(path);
		if (cmp > 0)
		{
			match_result[cmp] = it->second;
			if (TRACE)
				std::cout << "\t--> added to match_result\n\n";
		}
	}
	if (TRACE)
		std::cout << "no. of match_result: " << match_result.size() << '\n';

	if (match_result.size() == 1)
		return (match_result.begin()->second);

	if (match_result.size() > 0)
	{
		std::map<size_t, LocationConfig*>::iterator	match = match_result.end();
		match--;
		return (match->second);
	}
	return (_locations.begin()->second);
}

/*
** ---------------------------------- PRINT -----------------------------------
*/

void	ServerConfig::printConfig(void)
{
	std::cout << std::left;
	Print::printVector("SERVER NAMES: ", _server_name);
	Print::printLine("ROOT: ", _root);
	Print::printLine("INDEX: ", _index);
	Print::printLine("AUTOINDEX: ", _autoindex ? "on" : "off");
	Print::printLine("BODY MAX LENGTH: ", _body_max_length);
	Print::printVector("ALLOWED METHODS: ", _allowed_methods);
	if (_redirect.size() > 0)
		Print::printLine("REDIRECT: ", _redirect);
	Print::printMap("ERROR PAGES: ", _error_page);
	if (_cgi_exec.size() > 0)
		Print::printMap("CGI: ", _cgi_exec);

	/* Print config for all locations on this server */
	// std::cout << '\n' << GREEN << std::setw(5) << std::left << " ";
	// std::cout << std::setw(20) << "LOCATION";
	// std::cout << '\n' << RESET;

	// std::map<std::string, LocationConfig*>::iterator	it;
	// int	i = 0;
	// for (it = _locations.begin(); it != _locations.end(); it++)
	// {
	// 	std::stringstream	stream;
	// 	stream << "[" << i++ << "]";
	// 	std::cout << std::setw(5) << stream.str();
	// 	std::cout << std::setw(20) << it->first << '\n';
	// }
}

/*
** --------------------------------- SETTER -----------------------------------
*/

void	ServerConfig::setCGIPath(std::map<std::string, std::string> cgi_map)
{
	this->_cgi_exec = cgi_map;
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
