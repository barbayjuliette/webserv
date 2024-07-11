/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:27 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 14:54:24 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response()
{

}

Response::Response(Request *request)
{
	std::string					str;
	char						c;
	std::ifstream	page(request->getPath().c_str());
	std::stringstream			stream;

	if (page.good())
	{
		while (page.get(c))
			str += c;
		this->_status_code = 200;
		this->_status_text = "OK";
		page.close();
	}
	else
	{
		std::cerr << strerror(errno) << std::endl;
		this->_status_code = 404;
		this->_status_text = "Not found";
	}
	// std::cout << "Status: " << status_code << std::endl;
	// stream.clear();
	stream << "HTTP/1.1 " << this->_status_code << this->_status_text << "\r\n";
	stream << "Cache-Control: no-cache, private\r\n";
	stream << "Content-Type: text/html\r\n";
	stream << "Content-Length: " << str.size() << "\r\n";
	stream << "\r\n";
	stream << str;

	// std::string	message = stream.str();
	this->_full_response = stream.str();

}

Response::Response( const Response & src ) :
_status_code(src._status_code),
_status_text(src._status_text),
_http_version(src._http_version),
_headers(src._headers),
_body(src._body)

{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Response &				Response::operator=( Response const & rhs )
{
	if (this != &rhs)
	{
		this->_status_code = rhs._status_code;
	}
	return (*this);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Response::getStatusCode()
{
	return (this->_status_code);
}

std::string	Response::getStatusText()
{
	return (this->_status_text);
}

std::string	Response::getHttpVersion()
{
	return (this->_http_version);
}

std::string	Response::getHeaders()
{
	return (this->_headers);
}

std::string	Response::getBody()
{
	return (this->_body);
}

std::string	Response::getFullResponse()
{
	return (this->_full_response);
}


/* ************************************************************************** */