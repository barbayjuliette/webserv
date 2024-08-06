/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 17:42:24 by yliew             #+#    #+#             */
/*   Updated: 2024/07/13 17:42:26 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

LocationConfig::LocationConfig() : ValidConfig() {}

LocationConfig::LocationConfig(ServerConfig* server) :
	ValidConfig(), _server(server), _match_exact(false), _case_sensitive(false)
{
	initValidKeys();
	this->_path = "/";
}

LocationConfig::LocationConfig(const LocationConfig& other) : ValidConfig(other)
{
	initValidKeys();
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

LocationConfig::~LocationConfig() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	LocationConfig::initValidKeys(void)
{
	this->_validKeys["autoindex"] = &LocationConfig::parseAutoindex;
	this->_validKeys["error_page"] = &LocationConfig::parseErrorPages;
	this->_validKeys["root"] = &LocationConfig::parseRoot;
	this->_validKeys["index"] = &LocationConfig::parseIndex;
	this->_validKeys["redirect"] = &LocationConfig::parseRedirect;
	this->_validKeys["allowed_methods"] = &LocationConfig::parseAllowedMethods;
}

/* Function for validating all directives in the map
- Set root first if specified, to ensure that index files are rooted to the correct directory path */
void	LocationConfig::validateKeys(void)
{
	if (_directives.find("root") != _directives.end())
		parseRoot(_directives["root"]);
	else
		this->_root = this->_server->getRoot();

	if (_directives.find("cgi_ext") != _directives.end() && _directives.find("cgi_path") != _directives.end())
		parseCGIPath(_directives["cgi_ext"], _directives["cgi_path"]);

	for (t_strmap::iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		if (it->first == "cgi_ext" || it->first == "cgi_path" || it->first == "root")
			continue;

		t_dirmap::iterator found = this->_validKeys.find(it->first);
		if (found == this->_validKeys.end())
			throw InvalidConfigError("Unsupported location directive => " + it->first);
		t_directive	handlerFunction = found->second;
		(this->*handlerFunction)(it->second);
	}
}

void	LocationConfig::parseCGIPath(t_strvec& exts, t_strvec& paths)
{
	if (exts.size() != paths.size())
		throw InvalidConfigError("CGI extensions and paths do not match");

	for (size_t i = 0; i < exts.size(); i++)
	{
		_cgi_path[exts[i]] = paths[i];
	}
	// std::cout << "printing cgi map:\n";
	// for (std::map<std::string, std::string>::iterator it = _cgi_path.begin(); it != _cgi_path.end(); it++)
	// {
	// 	std::cout << it->first << ": " << it->second << '\n';
	// }
}

/* Syntax: location [modifier] [URI] (+ inline open brace '{' if applicable) */
void	LocationConfig::parsePath(t_strvec& tokens)
{
	t_strvec::iterator	last = tokens.end();
	last--;

	if (*last == "{")
		tokens.erase(last);
	if (tokens.size() <= 1 || tokens.size() > 3)
		throw InvalidConfigError("Invalid number of parameters for location block");

	t_strvec::iterator	current = tokens.begin();
	current++;
	if (parsePathModifier(*current) == -1)
	{
		if (tokens.size() >= 3)
			throw InvalidConfigError("Invalid parameters for location block");
	}
	else
		current++;

	this->_path = *current;
}

int	LocationConfig::checkPathModifier(std::string& token)
{
	std::string	arr[3] = {"=", "~", "~*"};

	for (int i = 0; i < 3; i++)
	{
		if (token == arr[i])
			return (i);
	}
	return (-1);
}

int	LocationConfig::parsePathModifier(std::string& token)
{
	int modifier = checkPathModifier(token);

	switch (modifier)
	{
		case MATCH_EXACT:
			this->_match_exact = true;
			break ;
		case CASE_SENSITIVE:
			this->_case_sensitive = true;
			break ;
		case CASE_INSENSITIVE:
			this->_case_sensitive = false;
			break ;
		default:
			break ;
	}
	return (modifier);
}

int	LocationConfig::comparePath(const std::string& str)
{
	size_t	count = 0;
	size_t	i = 1;

	while (i < _path.size() && i < str.size())
	{
		if (_path[i] == '/' && str[i] == '/')
			count++;
		if (_path[i] != str[i])
			break ;
		i++;
	}
	return (count);
	// size_t	start1 = _path.find('/', pos);
	// size_t	start2 = path.find('/', pos);
	// size_t	end1 = _path.find('/', pos + 1);
	// size_t	end2 = path.find('/', pos + 1);

	// for (size_t i = start1; i < end1 && i < end2; i++)
	// {
	// 	if (_path[i] != path[i])
	// 		return (count);
	// }
	// count++;
	// return (comparePath(path, end1, count));
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

std::string	LocationConfig::getPath(void)
{
	return (this->_path);
}

bool	LocationConfig::getMatchExact(void)
{
	return (this->_match_exact);
}

bool LocationConfig::getCaseSensitive(void)
{
	return (this->_case_sensitive);
}
