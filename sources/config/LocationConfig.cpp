/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 17:42:24 by yliew             #+#    #+#             */
/*   Updated: 2024/08/12 14:37:38 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

LocationConfig::LocationConfig() : ValidConfig() {}

LocationConfig::LocationConfig(ServerConfig* server) :
	ValidConfig(), _server(server), _match_exact(false), _case_sensitive(true)
{
	initValidKeys();
	this->_prefix = "/";
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

	if (_directives.find("cgi_ext") != _directives.end() && _directives.find("cgi_exec") != _directives.end())
		parseCGIPath(_directives["cgi_ext"], _directives["cgi_exec"]);

	for (t_strmap::iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		if (it->first == "cgi_ext" || it->first == "cgi_exec" || it->first == "root")
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
		_cgi_exec[exts[i]] = paths[i];
	}
	// std::cout << "printing cgi map:\n";
	// for (std::map<std::string, std::string>::iterator it = _cgi_exec.begin(); it != _cgi_exec.end(); it++)
	// {
	// 	std::cout << it->first << ": " << it->second << '\n';
	// }
}

/* Syntax: location [modifier] [URI prefix] (+ inline open brace '{' if applicable) */
void	LocationConfig::parsePrefix(t_strvec& tokens)
{
	t_strvec::iterator	last = tokens.end();
	last--;

	if (*last == "{")
		tokens.erase(last);
	if (tokens.size() <= 1 || tokens.size() > 3)
		throw InvalidConfigError("Invalid number of parameters for location block");

	t_strvec::iterator	current = tokens.begin();
	current++;
	if (parsePrefixModifier(*current) == -1)
	{
		if (tokens.size() >= 3)
			throw InvalidConfigError("Invalid parameters for location block");
	}
	else
		current++;

	this->_prefix = *current;
}

int	LocationConfig::checkPrefixModifier(std::string& token)
{
	std::string	arr[3] = {"=", "~", "~*"};

	for (int i = 0; i < 3; i++)
	{
		if (token == arr[i])
			return (i);
	}
	return (-1);
}

int	LocationConfig::parsePrefixModifier(std::string& token)
{
	int modifier = checkPrefixModifier(token);

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

std::string	toLower(std::string str)
{
	std::string	res;

	for (size_t i = 0; i < str.size(); i++)
	{
		res += std::tolower(str[i]);
	}
	return (res);
}

/* If location is CGI block: instead of prefix, check if extension is a valid CGI extensions */
bool	LocationConfig::compareExtension(const std::string& str)
{
	std::string	req_path(str);
	size_t	i = req_path.rfind('.');
	if (i == std::string::npos)
		return (false);

	std::string	req_ext = req_path.substr(i, std::string::npos);

	if (getCGIExec(req_ext).size() > 0)
	{
		std::cout << RED << req_ext << " is a CGI extension; returning CGI block\n" << RESET;
		return (true);
	}
	return (false);
}

/* Compare location with request URI
- If location directive is case-insensitive: convert paths to lowercase */
size_t	LocationConfig::comparePath(const std::string& str)
{
	size_t	count = 0;
	size_t	i = 1;
	std::string	prefix(this->_prefix);
	std::string	req_path(str);

	if (!_case_sensitive)
	{
		std::transform(req_path.begin(), req_path.end(), req_path.begin(), ::tolower);
		std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
	}

	if (TRACE)
		std::cout << "req_path: " << RED << req_path << RESET << " vs location: " << CYAN << prefix << RESET << " => ";

	while (i < prefix.size() && i < req_path.size())
	{
		if (prefix[i] == '/' && req_path[i] == '/')
			count++;
		if (prefix[i] != req_path[i])
			break ;
		i++;
	}
	if ((long)(count + 1) < std::count(prefix.begin(), prefix.end(), '/'))
		count = 0;
	if (TRACE)
		std::cout << count << '\n';
	return (count);
}

/*
** ---------------------------------- PRINT -----------------------------------
*/

void	LocationConfig::printConfig(void)
{
	Print::printLine("CASE SENSITIVE: ", _case_sensitive ? "on" : "off");
	Print::printLine("ROOT: ", _root);
	Print::printLine("INDEX: ", _index);
	Print::printLine("AUTOINDEX: ", _autoindex ? "on" : "off");
	if (_redirect.size() > 0)
		Print::printLine("REDIRECT: ", _redirect);
	Print::printVector("ALLOWED METHODS: ", _allowed_methods);
	Print::printMap("CGI PATH: ", _cgi_exec);
	Print::printMap("ERROR PAGES: ", _error_page);
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

std::string	LocationConfig::getPrefix(void) const
{
	return (this->_prefix);
}

std::string	LocationConfig::getCGIExec(std::string ext) const
{
	if (this->_cgi_exec.find(ext) == this->_cgi_exec.end())
		return ("");
	return (this->_cgi_exec.at(ext));
}

bool	LocationConfig::isCGILocation(void) const
{
	return (!this->_cgi_exec.empty());
}

bool	LocationConfig::getMatchExact(void) const
{
	return (this->_match_exact);
}

bool LocationConfig::getCaseSensitive(void) const
{
	return (this->_case_sensitive);
}
