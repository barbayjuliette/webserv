/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:33 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 16:03:48 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
** ------------------------------- MEMBER FUNCTIONS ---------------------------
*/

// int Request::parseHeader()
// {

// }

void Request::checkMethod()
{
    const std::string validMethods = "PUT,PATCH,CONNECT,OPTIONS,TRACE";
    std::string searchMethod = "," + _method + ",";

	if (this->_method == "GET" || this->_method == "POST" 
	 	|| this->_method == "DELETE")
	 	return ;
	else if (validMethods.find(searchMethod) != std::string::npos)
		this->_error = NOT_SUPPORTED;
	else
		this->_error = INVALID;
}

void Request::checkPath()
{
	if (_path == "/")
		_path = "/index.html";
	_path = "./wwwroot" + _path;
}

int Request::parseRequest()
{
	// Find first space for method -> Unsupport method 501
	size_t methodEnd = _raw.find(' ');
	// No method found
	if (methodEnd == std::string::npos)
	{
		this->_error = INVALID;
		return -1;
	}
	this->_method = _raw.substr(0, methodEnd);
	// Check if valid method
	checkMethod();

	// Find second space for path
	size_t pathEnd = _raw.find(' ', methodEnd + 1);
	if (pathEnd == std::string::npos)
	{
		this->_error = INVALID;
		return -1;
	}
	this->_path = _raw.substr(methodEnd + 1, pathEnd - (methodEnd + 1));
	// Check if path is root
	checkPath();

	// Find newline for http version
	size_t versionEnd = _raw.find("\r\n", pathEnd + 1);
	if (versionEnd == std::string::npos)
	{
		this->_error = INVALID;
		return -1;
	}
	this->_http_version = _raw.substr(pathEnd + 1, versionEnd - (pathEnd + 1));
	// std::cout << std::endl << std::endl << 
	// 	_method + " " + _path + " " + _http_version + " " << "end of vars" << 
	// 	std::endl << std::endl; 
	return 0;
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request() {}

Request::Request(std::string full_request) : _raw(full_request), _error(NO_ERR)
{
	if (this->_raw.length() == 0)
		 std::cerr << "Error: empty request" << std::endl;
	this->parseRequest();
}

Request::Request( const Request & src ):
	_raw(src._raw),
	_method(src._method),
	_path(src._path),
	_http_version(src._http_version),
	_headers(src._headers),
	_body(src._body)
{}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Request &				Request::operator=( Request const & rhs )
{
	if (this != &rhs)
	{
		this->_raw = rhs._raw;
		this->_path = rhs._path;
		this->_http_version = rhs._http_version;
		this->_method = rhs._method;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string		Request::getRaw()
{
	return (this->_raw);
}

std::string		Request::getPath()
{
	return (this->_path);
}

std::string		Request::getHttpVersion()
{
	return (this->_http_version);
}

std::string		Request::getMethod()
{
	return (this->_method);
}

std::map<std::string, std::string>		Request::getHeaders()
{
	return (this->_headers);
}

std::string		Request::getBody()
{
	return (this->_body);
}


/* ************************************************************************** */