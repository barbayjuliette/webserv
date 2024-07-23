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

void Request::printError(std::string error_msg)
{
	std::cout << RED << error_msg << RESET << std::endl;
}

void Request::parseBody()
{
	if (!_req_complete)
		return ;
	size_t headerEnd = _raw.find("\r\n\r\n");
	if (headerEnd != std::string::npos && headerEnd + 4 != _raw.size())
		this->_body = _raw.substr(headerEnd + 4);
	else if (_method == "POST")
	{
		_error = INVALID;
		if (VERBOSE)
			printError("invalid POST as body is missing");
	}
}

// Testing function
void Request::printHeaders(const std::map<std::string, std::string>& headers)
{
	std::cout << std::endl << std::endl << 
		"method: " << _method << std::endl << 
		"path:   " << _path << std::endl <<
		"http:   " << _http_version << std::endl <<
		"port:   " << _port << std::endl <<
		GREEN << "END OF VARS" << RESET << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << "[" << it->first << "] = " << it->second << std::endl;
    }
    std::cout << GREEN << "END OF HEADERS" << RESET << std::endl;
    std::cout << GREEN << "START OF BODY" << _body << RESET << std::endl;
    std::cout << GREEN << "END OF BODY" << RESET << std::endl;
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

	            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

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
    const std::string validMethods = ",PUT,PATCH,CONNECT,OPTIONS,TRACE,";
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
	return 0;
}

size_t Request::convert_sizet(std::string str)
{
	std::stringstream ss(str);
	size_t num;
	ss >> num;

	if (ss.fail())
	{
		_error = INVALID;
	}
	return num;
}

bool Request::is_header_complete()
{
	// Check if header is complete
	size_t headerEnd = _raw.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
	{
		_header_length = headerEnd + 4;
		// Set curr_length
		_curr_length = _curr_bytes_read - _header_length;
		// Check if body max length exceeded
			// Arbitrary body max length -> to be set by config file
		if (_curr_length > _body_max_length)
		{
			_error = INVALID;
			return true;
		}
		return true;
	}
	return false;
}

void Request::initRequest()
{
	// Check if header is complete
	if (!is_header_complete())
		return ;
	// Check if encoding chunked
	if (this->_headers.find("transfer-encoding") != this->_headers.end())
	{
		if (_headers["transfer-encoding"] == "chunked")
			_is_chunked = true;
	}
	// Check if curr read >= content length
	if (this->_headers.find("content-length") != this->_headers.end())
	{
		if (_is_chunked == true)
			_error = INVALID;
		else
		{
			_content_length = convert_sizet(_headers["content-length"]);
			// If no error, check if content length met
			if (_error != NO_ERR)
			{
				if (_curr_length - _header_length <= _content_length)
					_req_complete = true;
				else if (_curr_length - _header_length > _content_length) // if body length longer than content length, ret invalid
					_error = INVALID;
			}
		}
	}
	if ((_is_chunked == false && _content_length == -1) || _error == INVALID)
		_req_complete = true;
}

void	Request::copy_partial_buffer(int bytes_read, char *buffer)
{
	std::string new_raw;
	ft_strcpy(buffer, bytes_read, _curr_bytes_read);
	new_raw = _raw + buffer;
	_raw = new_raw;
	_curr_bytes_read += bytes_read; // update curr bytes read to new buf length
}

/* Function to handle incomplete header
	start copying to end of buf of previous read */
void	Request::handle_incomplete_header(int bytes_read, char *buffer)
{
	this->copy_partial_buffer(bytes_read, buffer);

	// Check if header is complete
	if (!is_header_complete())
		return ;
	else
	{
		this->initRequest();
		this->parseRequest();
		this->parsePort();
		this->parseHeader();
		this->parseBody();
	}
}

int 	Request::ft_strcmp(char *buf, char *buf2, size_t num)
{
	while (int i = 0; i < num; i++)
	{
		if (buf[i] != buf2[i])
			return buf[i] - buf2[i];
	}
	return buf[i] - buf2[i];
}

void	Request::handle_chunk(int bytes_read, char *buffer)
{
	if (_is_chunked == true)
		copy_partial_buffer(bytes_read, buffer);
	// Last chunk received
	if (bytes_read == 5 && !ft_strcmp(buffer, "0\r\n\r\n", 5))
	{
		_req_complete = true;
		// parse body
	}
}

void Request::ft_strcpy(char *full_request, int bytes_read, size_t start_index)
{
	for (int i = 0; i < bytes_read; i++)
	{
		_buf[i + start_index] = full_request[i];
	}
}

void Request::print_variables() const 
{
  std::cout << "Request Variables:\n";
    std::cout << "Raw: " << _raw << "\n";
    // std::cout << "Buffer: ";  // Print first 100 characters of buffer for practicality
    // for (int i = 0; i < 100 && _buf[i] != '\0'; ++i) {
    //     std::cout << _buf[i];
    // }
    // std::cout << "\n";
    std::cout << "Header Length: " << _header_length << "\n";
    std::cout << "Request Complete: " << (_req_complete ? "true" : "false") << "\n";
    std::cout << "Body Max Length: " << _body_max_length << "\n";
    std::cout << "Content Length: " << _content_length << "\n";
    std::cout << "Is Chunked: " << (_is_chunked ? "true" : "false") << "\n";
    std::cout << "Method: " << _method << "\n";
    std::cout << "Path: " << _path << "\n";
    std::cout << "HTTP Version: " << _http_version << "\n";
    std::cout << "Port: " << _port << "\n";
    std::cout << "Current Length: " << _curr_length << "\n";
    std::cout << "Headers:\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
    std::cout << "Body: " << _body << "\n";
    std::cout << "Error: " << (_error == NO_ERR ? "NO_ERR" : "ERR") << "\n";
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request() {}

Request::Request(char *full_request, int bytes_read) : 
	_raw(full_request),
	_header_length(-1),
	_req_complete(false),
	_body_max_length(10000),
	_content_length(-1),
	_is_chunked(false),
	_curr_bytes_read(bytes_read),
	_error(NO_ERR)
{
	if (this->_raw.length() == 0)
		_error = INVALID;
	_buf = new char[1000 + _body_max_length];
	std::memset(_buf, 0, 1000 + _body_max_length);
	// copies buffer to request's buf
	ft_strcpy(full_request, bytes_read, 0);
	this->initRequest();
	if (_header_length != -1)
	{
		this->parseRequest();
		this->parsePort();
		this->parseHeader();
		this->parseBody();
	}
	if (VERBOSE)
	{
		printHeaders(_headers);
		print_variables();
	}
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

Request::~Request() 
{
	delete _buf;
}

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

ssize_t		Request::getHeaderLength() const
{
	return (this->_header_length);
}

bool	Request::getReqComplete() const
{
	return (this->_req_complete);
}

/* ************************************************************************** */
