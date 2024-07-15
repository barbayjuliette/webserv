/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 18:47:56 by yliew             #+#    #+#             */
/*   Updated: 2024/07/12 18:48:54 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValidConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ValidConfig::ValidConfig()
{
	this->_listen_port = 0;
	this->_client_max_body_size = 5000;
	this->_autoindex = false;
}

// ValidConfig::ValidConfig(const ValidConfig& other)
// {
// 	*this = other;
// }

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ValidConfig::~ValidConfig() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

// void	ValidConfig::initValidKeys

// /* Function for validating all directives in the map */
void	ValidConfig::validateKeys(void)
{
	for (t_strmap::iterator it = this->_directives.begin(); it != this->_directives.end(); it++)
	{
		std::cout << "current key: " << it->first << '\n';

		t_dirmap::iterator found = this->_validKeys.find(it->first);
		if (found == this->_validKeys.end())
			throw InvalidConfigError("Invalid directive");
		t_directive	handlerFunction = found->second;
		(this->*handlerFunction)(it->second);
	}
}

/* Port number range: 0 to 65353 */
void	ValidConfig::setListenPort(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_listen_port = convertToInt(tokens[0]);

	if (this->_listen_port < 0 || this->_listen_port > 65353)
		throw InvalidConfigError("Listening port must be a number from 0 to 65353");
}

void	ValidConfig::setClientMaxBodySize(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_client_max_body_size = convertToInt(tokens[0]);
}

void	ValidConfig::setAutoindex(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);
	if (tokens[0] == "on")
		this->_autoindex = true;
	else if (tokens[0] == "off")
		this->_autoindex = false;
	else
		throw InvalidConfigError("Invalid param for autoindex");
}

/*
200 OK.	The request finished normally.
204 No response.	The request was understood and processed, but there is no new document to be loaded by the client.
302 Found.	The client should look for data at a new URL, given by a Location header.
304 Use local copy.	The client sent a request with an if-modified-since header, and the requested data hasn't been modified since the given date.
400 Bad request.	The request had illegal or unintelligible HTTP inside.
401 Unauthorized.	If access authorization is enabled, the request could not be fulfilled because the user did not provide the proper authorization to access the area. With current authorization schemes, a WWW-Authenticate header must be provided to give the client instructions on how to complete the request with the proper authorization.
403 Forbidden.	The client is not allowed to access what it requested.
404 Not found.	The client asked for something the server couldn't find.
500 Server error.	This is a catch-all error code that indicates something went wrong in the server or the CGI program, and the problem stopped the request from being completed.
501 Not implemented.	The client asked the server to perform an action that the server knows about, but can't do.*/
void	ValidConfig::setErrorPages(const t_strvec& tokens)
{
	std::string	error_page = tokens[tokens.size() - 1];

	for (size_t i = 0; i < tokens.size() - 1; i++)
	{
		int	status_code = convertToInt(tokens[i]);
		this->_error_page[status_code] = error_page;
		// std::cout << "status code: " << status_code << "; error page: " << error_page << '\n';
	}
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

t_strmap&	ValidConfig::getDirectives(void)
{
	return (this->_directives);
}

/*
** ---------------------------------- UTILS -----------------------------------
*/

int	ValidConfig::convertToInt(const std::string& str)
{
	std::stringstream	stream(str);
	int	nb;
	stream >> nb;

	if (!stream.eof() || stream.fail())
		throw InvalidConfigError("Non-numeric parameter");
	return (nb);
}

bool	ValidConfig::isStatusCode(const std::string& str)
{
	std::stringstream	stream(str);
	int	nb;
	stream >> nb;

	if (!stream.eof() || stream.fail())
		return (false);
	if (nb < 100 || nb > 599)
		return (false);
	return (true);
}

/*
** -------------------------------- EXCEPTIONS --------------------------------
*/

ValidConfig::InvalidConfigError::InvalidConfigError(const std::string& message) \
	: _message("Invalid config: " + message) {};

ValidConfig::InvalidConfigError::~InvalidConfigError() throw() {}

const char	*ValidConfig::InvalidConfigError::what() const throw()
{
	return (_message.c_str());
}

