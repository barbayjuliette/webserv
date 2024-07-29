/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 18:47:56 by yliew             #+#    #+#             */
/*   Updated: 2024/07/26 15:32:23 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValidConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ValidConfig::ValidConfig()
{
	this->_port = 8080;
	this->_host = "localhost";
	this->_body_max_length = 5000;
	this->_autoindex = false;
	this->_address_info = NULL;
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
		this->_port = other._port;
		this->_body_max_length = other._body_max_length;
		this->_autoindex = other._autoindex;
		this->_address_info = other._address_info;
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

ValidConfig::~ValidConfig()
{
	if (this->_address_info)
		freeaddrinfo(this->_address_info);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* Function for validating all directives in the map
- Set root first if specified, to ensure that index files are rooted to the correct directory path */
void	ValidConfig::validateKeys(void)
{
	if (this->_directives.find("root") != this->_directives.end())
		setRoot(this->_directives["root"]);

	if (this->_directives.find("listen") != this->_directives.end())
		setListenPort(this->_directives["listen"]);

	for (t_strmap::iterator it = this->_directives.begin(); it != this->_directives.end(); it++)
	{
		if (TRACE)
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

	this->_port = strToInt(tokens[0]);

	if (this->_port < 0 || this->_port > 65353)
		throw InvalidConfigError("Listening port must be a number from 0 to 65353");
}

void	ValidConfig::setBodyMaxLength(const t_strvec& tokens)
{
	if (tokens.size() != 1)
		throw InvalidConfigError(PARAM_COUNT_ERR);

	this->_body_max_length = strToSizet(tokens[0]);
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

	if (tokens[0] == "localhost")
		this->_host = "127.0.0.1";
	else
		this->_host = tokens[0];

	setAddressInfo(this->_host, intToStr(this->_port));
}

void	ValidConfig::setAddressInfo(std::string& host, std::string port)
{
	struct addrinfo hints;
	struct addrinfo	*result;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int	res = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
	if (res != 0)
		throw InvalidConfigError(gai_strerror(res));

	this->_address_info = result;
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

std::string	ValidConfig::intToStr(const int nb)
{
	std::stringstream	stream;
	stream << nb;

	return (stream.str());
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
	return (this->_port);
}

int	ValidConfig::getBodyMaxLength(void)
{
	return (this->_body_max_length);
}

bool	ValidConfig::getAutoindex(void)
{
	return (this->_autoindex);
}

struct addrinfo	*ValidConfig::getAddressInfo(void)
{
	return (this->_address_info);
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

