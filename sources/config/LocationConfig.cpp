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
	this->_validKeys["autoindex"] = &LocationConfig::setAutoindex;
	this->_validKeys["error_page"] = &LocationConfig::setErrorPages;
	this->_validKeys["root"] = &LocationConfig::setRoot;
	this->_validKeys["index"] = &LocationConfig::setIndex;
	this->_validKeys["redirect"] = &LocationConfig::setRedirect;
	this->_validKeys["allowed_methods"] = &LocationConfig::setAllowedMethods;
}

/* Function for validating all directives in the map
- Set root first if specified, to ensure that index files are rooted to the correct directory path */
void	LocationConfig::validateKeys(void)
{
	if (_directives.find("root") != _directives.end())
		setRoot(_directives["root"]);
	else
		this->_root = this->_server->getRoot();

	for (t_strmap::iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		if (TRACE)
			std::cout << "current key: " << it->first << '\n';

		t_dirmap::iterator found = this->_validKeys.find(it->first);
		if (found == this->_validKeys.end())
			throw InvalidConfigError("Unsupported location directive => " + it->first);
		t_directive	handlerFunction = found->second;
		(this->*handlerFunction)(it->second);
	}
}

/* Syntax: location [modifier] [URI] (+ inline open brace '{' if applicable) */
void	LocationConfig::setPath(t_strvec& tokens)
{
	t_strvec::iterator	last = tokens.end();
	last--;

	if (*last == "{")
		tokens.erase(last);
	if (tokens.size() <= 1 || tokens.size() > 3)
		throw InvalidConfigError("Invalid number of parameters for location block");

	t_strvec::iterator	current = tokens.begin();
	current++;
	if (setPathModifier(*current) == -1)
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

int	LocationConfig::setPathModifier(std::string& token)
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
