/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:33 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 14:30:41 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request()
{

}

Request::Request(std::string full_request) : _full_request(full_request)
{
	std::stringstream			stream(_full_request);
	std::string					content;
	std::vector<std::string>	request_vector;

	while (stream >> content)
	{
		request_vector.push_back(content);
	}
	this->_type = request_vector[0];
	// Check if valid : GET POST DELETE
	this->_path = request_vector[1];
	if (_path == "/")
		_path = "/index.html";
	_path = "./wwwroot" + _path;
}

Request::Request( const Request & src ):
_full_request(src._full_request),
_path(src._path),
_http_version(src._http_version),
_type(src._type),
_headers(src._headers),
_body(src._body)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Request &				Request::operator=( Request const & rhs )
{
	if (this != &rhs)
	{
		this->_full_request = rhs._full_request;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string		Request::getFullRequest()
{
	return (this->_full_request);
}

std::string		Request::getPath()
{
	return (this->_path);
}

std::string		Request::getHttpVersion()
{
	return (this->_http_version);
}

std::string		Request::getType()
{
	return (this->_type);
}

std::string		Request::getHeaders()
{
	return (this->_headers);
}

std::string		Request::getBody()
{
	return (this->_body);
}



/* ************************************************************************** */