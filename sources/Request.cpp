/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:33 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/16 16:35:49 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
** ------------------------------- MEMBER FUNCTIONS ---------------------------
*/

void Request::parseBody()
{
	size_t headerEnd = _raw.find("\r\n\r\n");
	if (headerEnd != std::string::npos && headerEnd + 4 != _raw.size())
		this->_body = _raw.substr(headerEnd + 4);
	else if (_method == "POST")
			_error = INVALID;
}

// Testing function
void Request::printHeaders(const std::map<std::string, std::string>& headers)
{
	std::cout << std::endl << std::endl << 
		_method + " " + _path + " " + _http_version + " " << _port << " end of vars" << 
		std::endl << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << "[" << it->first << "] = " << it->second << std::endl;
    }
    std::cout << GREEN << "end of headers" << RESET << std::endl << std::endl;
}

std::string Request::extractHeader()
{
    // Find the "Connection:" line in the raw request
    size_t connectionStart = _raw.find("Connection: ");
    if (connectionStart == std::string::npos)
        return "";
    // Find the end of the header
    size_t headersEnd = _raw.find("\r\n\r\n", connectionStart);
    if (headersEnd == std::string::npos)
        return "";
    // Extract from "Connection:" line to the end of headers (not including the body)
    std::string result = _raw.substr(connectionStart, headersEnd - connectionStart);
    return result;
}

void Request::parseHeader()
{
	std::string header = extractHeader();
	if (header != "")
	{
		std::istringstream stream(header);
		std::string line;

		while(std::getline(stream, line))
		{
			// Remove trailing carriage return char
			if (!line.empty() && line[line.size() - 1] == '\r')
	            line.erase(line.size() - 1);
	        // Find colon
	        size_t colon = line.find(":");
	        if (colon != std::string::npos)
	        {
	        	std::string name = line.substr(0, colon);
	        	std::string value = line.substr(colon + 2);

	        	// Trim leading/trailing whitespaces
		       	name.erase(name.find_last_not_of(" \n\r\t") + 1);
	            value.erase(0, value.find_first_not_of(" \n\r\t"));

	            this->_headers[name] = value;
	        }
		}
	}
}

/* Function to get second line with Host: 
convert port number to int and store */
void Request::parsePort()
{
	size_t hostStart = _raw.find("Host: ");
	if (hostStart != std::string::npos)
	{
		hostStart += 6;
		size_t hostEnd = _raw.find("\r\n", hostStart);
		if (hostEnd != std::string::npos)
		{
			std::string host = _raw.substr(hostStart, hostEnd - hostStart);
			size_t colon = host.find(":");
			if (colon != std::string::npos)
				this->_port = std::atoi(host.substr(colon + 1).c_str());
			else
				_error = INVALID;
		}
		else
			_error = INVALID;
	}
	else
		_error = INVALID;
}

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
	this->_http_version = "HTTP/1.1";
	// std::cout << this->_full_request << std::endl;
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
	this->parsePort();
	this->parseHeader();
	this->parseBody();
	// printHeaders(_headers);
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

std::string		Request::getRaw() const
{
	return (this->_raw);
}

std::string		Request::getPath() const
{
	return (this->_path);
}

std::string		Request::getHttpVersion() const
{
	return (this->_http_version);
}

std::string		Request::getMethod() const
{
	return (this->_method);
}

std::map<std::string, std::string>		Request::getHeaders() const
{
	return (this->_headers);
}

std::string		Request::getBody() const
{
	return (this->_body);
}


/* ************************************************************************** */