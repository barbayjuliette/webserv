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

ValidConfig::ValidConfig(const ValidConfig& other)
{
	*this = other;
}

ValidConfig&	ValidConfig::operator=(const ValidConfig& other)
{
	if (this != &other)
	{
		this->_directives = other._directives;
		this->_listen_port = other._listen_port;
		this->_client_max_body_size = other._client_max_body_size;
		this->_autoindex = other._autoindex;
		this->_host = other._host;
		this->_root = other._root;
		this->_redirect = other._redirect;
		this->_server_name = other._server_name;
		this->_index = other._index;
		this->_allow_methods = other._allow_methods;
	}
	return (*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ValidConfig::~ValidConfig() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* Function for validating all directives in the map
- Set root first if specified, to ensure that index files are rooted to the correct directory path */
void	ValidConfig::validateKeys(void)
{
	if (this->_directives.find("root") != this->_directives.end())
		setRoot(this->_directives["root"]);
	for (t_strmap::iterator it = this->_directives.begin(); it != this->_directives.end(); it++)
	{
		if (DEBUG)
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

	this->_listen_port = strToInt(tokens[0]);

	if (this->_listen_port < 0 || this->_listen_port > 65353)
		throw InvalidConfigError("Listening port must be a number from 0 to 65353");
}

void	ValidConfig::setClientMaxBodySize(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_client_max_body_size = strToSizet(tokens[0]);
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

void	ValidConfig::setHost(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	struct addrinfo hints;
	struct addrinfo	*res;
	std::string	host;
	if (tokens[0] == "localhost")
		host = "127.0.0.1";
	else
		host = tokens[0];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int	result = getaddrinfo(host.c_str(), NULL, &hints, &res);
	if (result != 0)
		throw InvalidConfigError(gai_strerror(result));
    freeaddrinfo(res);

	this->_host = host;
}

void	ValidConfig::setRoot(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	if (!isDirectory(tokens[0]))
		throw InvalidConfigError("Invalid root directory");
	this->_root = tokens[0];
}

void	ValidConfig::setRedirect(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	if (!isRegularFile(tokens[0]))
		throw InvalidConfigError("Invalid redirect");
	this->_redirect = tokens[0];
}

void	ValidConfig::setServerName(const t_strvec& tokens)
{
	this->_server_name = tokens;
}

void	ValidConfig::setIndex(const t_strvec& tokens)
{
	if (tokens.empty())
		return ;

	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::string	index_file = this->_root + tokens[i];
		if (!isRegularFile(index_file))
			throw InvalidConfigError("Invalid index file");
		if (access(index_file.c_str(), R_OK))
			throw InvalidConfigError("No permission to read index file");
	}
	this->_index = tokens;
}

void	ValidConfig::setAllowedMethods(const t_strvec& tokens)
{
	if (tokens.empty())
		return ;

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] != "GET" && tokens[i] != "POST" && tokens[i] != "DELETE")
			throw InvalidConfigError("Invalid method");
	}
	this->_allow_methods = tokens;
}

void	ValidConfig::setErrorPages(const t_strvec& tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		int	delimiter = tokens[i].find("=");
		std::string	status = tokens[i].substr(0, delimiter);
		int	status_code = strToInt(status);

		std::string	error_page = tokens[i].substr(delimiter + 1, std::string::npos);
		if (!isRegularFile(error_page))
			throw InvalidConfigError("Invalid error page");

		this->_error_page[status_code] = error_page;
	}
}

/*
** ---------------------------------- UTILS -----------------------------------
*/

int	ValidConfig::strToInt(const std::string& str)
{
	std::stringstream	stream(str);
	int	nb;
	stream >> nb;

	if (!stream.eof() || stream.fail())
		throw InvalidConfigError("Non-numeric parameter");
	return (nb);
}

int	ValidConfig::strToSizet(const std::string& str)
{
	std::stringstream	stream(str);
	size_t	nb;
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

int	ValidConfig::isDirectory(const std::string& str)
{
	struct stat	buffer;
	const char	*path = str.c_str();

	stat(path, &buffer);
	return (S_ISDIR(buffer.st_mode));
}

int	ValidConfig::isRegularFile(const std::string& str)
{
	struct stat	buffer;
	const char	*path = str.c_str();

	stat(path, &buffer);
	return (S_ISREG(buffer.st_mode));
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

t_strmap&	ValidConfig::getDirectives(void)
{
	return (this->_directives);
}

int	ValidConfig::getPort(void)
{
	return (this->_listen_port);
}

int	ValidConfig::getClientMaxBodySize(void)
{
	return (this->_client_max_body_size);
}

bool	ValidConfig::getAutoindex(void)
{
	return (this->_autoindex);
}

std::string	ValidConfig::getHost(void)
{
	return (this->_host);
}

std::string	ValidConfig::getRoot(void)
{
	return (this->_root);
}

std::string	ValidConfig::getRedirect(void)
{
	return (this->_redirect);
}

t_strvec	ValidConfig::getServerName(void)
{
	return (this->_server_name);
}

t_strvec	ValidConfig::getIndex(void)
{
	return (this->_index);
}

t_strvec	ValidConfig::getAllowedMethods(void)
{
	return (this->_allow_methods);
}

std::string	ValidConfig::getErrorPage(int status_code)
{
	if (this->_error_page.find(status_code) == this->_error_page.end())
		return ("");
	return (this->_error_page[status_code]);
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

