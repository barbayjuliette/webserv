/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:33 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/26 17:59:42 by jbarbay          ###   ########.fr       */
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
    const std::string validMethods = ",HEAD,PUT,PATCH,CONNECT,OPTIONS,TRACE,";
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
	_path = "./wwwroot" + _path;
	this->_http_version = "HTTP/1.1";
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
		_curr_length = _raw.length() - _header_length;
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

bool	Request::handle_chunk(char *buffer, int bytes_read)
{
    if (!_is_chunked)
    {
        return false;
    }
 	// Parse the new chunked data
 	_raw.append(buffer, bytes_read);
    int i = 0;
    while (i < bytes_read) 
    {
        // Find the chunk size in hexadecimal format
        char *chunk_size_start = buffer + i;
        char *chunk_size_end = std::strstr(chunk_size_start, "\r\n");
        if (!chunk_size_end)
            break;

        // Convert chunk size from hexadecimal to integer
        *chunk_size_end = '\0';
        int chunk_size = std::strtol(chunk_size_start, NULL, 16);
        if (chunk_size == 0) 
        {
            // Last chunk received
            _req_complete = true;
            return true;
        }

        // Move the pointer past the chunk size and \r\n
        i += (chunk_size_end - chunk_size_start) + 2;

        // Ensure there are enough bytes in the buffer for the chunk data and \r\n
        if (i + chunk_size + 2 > bytes_read) 
            break;

        // Append the chunk data to the body
        _body.append(buffer + i, chunk_size);

        // Move the pointer past the chunk data and \r\n
        i += chunk_size;
    }
    return _req_complete;
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
	// If content length key found
	if (this->_headers.find("content-length") != this->_headers.end())
	{
		// If chunked -> invalid
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
	// If not chunked and no content length -> req complete
	if ((_is_chunked == false && _content_length == -1) || _error == INVALID)
		_req_complete = true;
}

void Request::initBody()
{
	// If chunked -> copy body to _body -> continue appending from subsequent chunked reqs
	// If not chunked -> copy body and parse only if req is complete
	if ((this->_header_length != -1 && _is_chunked) || (!_is_chunked && _req_complete))
	{
		std::string::size_type body_start = _raw.find("\r\n\r\n");
		// Find start of body for chunked
		if (_is_chunked && body_start != std::string::npos)
        {
            body_start += 4;
            _body = _raw.substr(body_start);
       	}
       	else if (!_is_chunked && body_start != std::string::npos)
        {
            body_start += 4;
            // Post method missing body error
            if (body_start == _raw.size() && _method == "POST")
            	_error = INVALID;
            else if (body_start + _content_length <= _raw.size()) // Check if there is enough length in _raw
                _body = _raw.substr(body_start, _content_length);
            else
                _error = INVALID; // Error: insufficient length in _raw
        }
	}
}

/* Function to handle incomplete header
	start copying to end of buf of previous read */
void	Request::handle_incomplete_header(int bytes_read, char *buffer)
{
	_raw.append(buffer, bytes_read);
	// Check if header is complete
	if (!is_header_complete())
		return ;
	else
	{
		this->initRequest();
		this->parseRequest();
		this->parsePort();
		this->parseHeader();
		this->initBody();
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

Request::Request(char *full_request, ServerConfig *config) : 
	_raw(full_request),
	_header_length(-1),
	_req_complete(false),
	_body_max_length(10000),
	_content_length(-1),
	_is_chunked(false),
	_error(NO_ERR),
	_config(config)
{
	(void)_config;
	if (this->_raw.length() == 0)
		_error = INVALID;
	this->initRequest();
	if (_header_length != -1)
	{
		this->parseRequest();
		this->parsePort();
		this->parseHeader();
		this->initBody();
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
{}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Request &				Request::operator=( Request const & rhs )
{
	if (this != &rhs)
	{
		this->_raw = rhs._raw;
		this->_header_length = rhs._header_length;
		this->_req_complete = rhs._req_complete;
		this->_body_max_length = rhs._body_max_length;
		this->_content_length = rhs._content_length;
		this->_is_chunked = rhs._is_chunked;
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_http_version = rhs._http_version;
		this->_port = rhs._port;
		this->_curr_length = rhs._curr_length;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_error = rhs._error;
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
