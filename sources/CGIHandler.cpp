/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/06 19:32:01 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

CGIHandler::CGIHandler(CGIHandler const & src) :
_result(src._result),
_content_type(src._content_type),
_html(src._html),
_headers(src._headers),
_full_path(src._full_path),
_error(src._error)
{

}

CGIHandler::~CGIHandler()
{

}

CGIHandler::CGIHandler() : _error(0)
{

}

CGIHandler &		CGIHandler::operator=( CGIHandler const & rhs )
{
	if (this != &rhs)
	{
		this->_result = rhs._result;
		this->_content_type = rhs._content_type;
		this->_html = rhs._html;
		this->_headers = rhs._headers;
		this->_full_path = rhs._full_path;
		this->_error = rhs._error;
	}
	return(*this);
}


std::string	CGIHandler::intToString(int num)
{
	std::stringstream	stream;
	stream << num;
	return (stream.str());
}

// ---------------------------------------- ACCESSORS ----------------------------------------

std::string	CGIHandler::getResult()
{
	return (this->_result);
}

std::string	CGIHandler::getHtml()
{
	return (this->_html);
}

std::string	CGIHandler::getContentType()
{
	return (this->_content_type);
}

void		CGIHandler::setHeaders()
{
	std::size_t		pos = _result.find("\r\n\r\n", 0);

	if (pos == std::string::npos)
		_headers = "";
	else
		_headers = _result.substr(0, pos);
	// std::cout << "Headers: " << _headers << std::endl;
}

void	CGIHandler::setContentType()
{
	setHeaders();

	std::string	low = _headers;
	for (size_t i = 0; i < _headers.size() ; i++)
	{
		low[i] = (char)tolower(_headers[i]);
	}

	std::size_t		pos = low.find("content-type:", 0);
	if (pos == std::string::npos)
	{
		// TO DO Error 
		this->_content_type = "text/plain";
		std::cout << "No Content-type found in CGI\n";
		return ;
	}

	this->_content_type = _headers.substr(pos + 13, low.find("/n", pos + 13) - 1);
	std::cout << "Content_type: " << _content_type << std::endl;
}

void	CGIHandler::setHtml()
{
	std::string		delim = "\r\n\r\n";
	std::size_t		pos = _result.find(delim, 0);

	if (pos == std::string::npos)
		_html = _result;
	else
		_html = _result.substr(pos + delim.size() + 1, _result.size());
	std::cout << "HTML: " << _html << std::endl;
}

void	CGIHandler::setResult(std::string result)
{
	this->_result = result;
}

std::string	CGIHandler::getFullPath()
{
	return (this->_full_path);
}

void	CGIHandler::setFullPath(std::string path)
{
	this->_full_path = path;
}

int			CGIHandler::getError()
{
	return (this->_error);
}

void		CGIHandler::setError(int error)
{
	this->_error = error;
}
