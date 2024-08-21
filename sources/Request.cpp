/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:33 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 13:42:31 by jbarbay          ###   ########.fr       */
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

std::string Request::extractHeader()
{
	std::string raw_str(_raw.begin(), _raw.end());
    // Find the end of the header
    size_t headersEnd = raw_str.find("\r\n\r\n");
    if (headersEnd == std::string::npos)
        return "";
    // Extract from start of _raw line to the end of headers (not including the body)
    std::string result = raw_str.substr(0, headersEnd);
    return result;
}

// If content type is multipart/form-data, parse boundary
void Request::parseContentType()
{
	if (_headers.find("content-type") != _headers.end() &&
    	_headers["content-type"].find("multipart/form-data") != std::string::npos)
    {
    	_content_type = _headers["content-type"].substr(0, _headers["content-type"].find(";"));
    	if (_headers["content-type"].find("boundary=") != std::string::npos)
    		_boundary = _headers["content-type"].substr(_headers["content-type"].find("boundary=") + 9);
    }
}

void Request::parseHeader(std::string header)
{
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
		parseContentType();
	}
}

/* Function to get second line with Host: 
convert port number to int and store */
void Request::parsePort(std::string header)
{
	size_t hostStart = header.find("Host: ");
	if (hostStart != std::string::npos)
	{
		std::cout << "\nHOST START: " << hostStart << '\n';
		hostStart += 6;
		size_t hostEnd = header.find("\r\n", hostStart);
		std::cout << "\nHOST END: " << hostEnd << '\n';
		if (hostEnd != std::string::npos)
		{
			std::string host = header.substr(hostStart, hostEnd - hostStart);
			std::cout << "\nHOST SUBSTR: " << host << '\n';
			size_t colon = host.find(":");
			if (colon != std::string::npos)
			{
				this->_host = host.substr(0, colon);
				if (this->_host == "localhost")
					this->_host = "127.0.0.1";
				this->_port = std::atoi(host.substr(colon + 1).c_str());
			}
			else
			{
				_error = INVALID_PORT;
				print_error("error: port is invalid");
			}
		}
		else
		{
			_error = NO_HOST;
			print_error("error: host is invalid");
		}
	}
	else
	{
		_error = NO_HOST;
		print_error("error: host is invalid");
	}
}

/* Static method to extract host/port before the Request instance is created
- Used in Cluster to determine which server to send the request to */
void	Request::parseHostPort(char *buffer, std::string& host, int& port)
{
	std::string	raw(buffer);
	size_t hostStart = raw.find("Host: ");

	if (hostStart != std::string::npos)
	{
		hostStart += 6;
		size_t hostEnd = raw.find("\r\n", hostStart);
		if (hostEnd != std::string::npos)
		{
			std::string str = raw.substr(hostStart, hostEnd - hostStart);
			size_t colon = str.find(":");
			if (colon != std::string::npos)
			{
				host = str.substr(0, colon);
				if (host == "localhost")
					host = "127.0.0.1";
				port = std::atoi(str.substr(colon + 1).c_str());
			}
			else
				throw std::runtime_error("Could not parse request host:port");
		}
		else
			throw std::runtime_error("Could not parse request host:port");
	}
	else
		throw std::runtime_error("Could not parse request host:port");
}

void Request::checkMethod()
{
    const std::string validMethods = ",HEAD,PUT,PATCH,CONNECT,OPTIONS,TRACE,";
    std::string searchMethod = "," + _method + ",";

	if (this->_method == "GET" || this->_method == "POST" 
	 	|| this->_method == "DELETE")
	 	return ;
	else if (validMethods.find(searchMethod) != std::string::npos)
	{
		_req_complete = true;
		this->_error = NOT_SUPPORTED;
		print_error("error: method not supported by webserv");
	}
	else
	{
		_req_complete = true;
		this->_error = INVALID_METHOD;
		print_error("error: method is invalid");
	}
}

void Request::checkPath()
{
	this->_http_version = "HTTP/1.1";
}

int Request::parseRequest(std::string header)
{
	// Find first space for method -> Unsupport method 501
	size_t methodEnd = header.find(' ');
	// No method found
	if (methodEnd == std::string::npos)
	{
		this->_error = INVALID;
		print_error("error: method not found in request");
		return -1;
	}
	this->_method = header.substr(0, methodEnd);
	// Check if valid method
	checkMethod();

	// Find second space for path
	size_t pathEnd = header.find(' ', methodEnd + 1);
	if (pathEnd == std::string::npos)
	{
		this->_error = INVALID;
		print_error("error: invalid path in request");
		return -1;
	}
	this->_path = header.substr(methodEnd + 1, pathEnd - (methodEnd + 1));
	// Check if path is root
	checkPath();

	// Find newline for http version
	size_t versionEnd = header.find("\r\n", pathEnd + 1);
	if (versionEnd == std::string::npos)
	{
		this->_error = INVALID;
		print_error("error: invalid http version in request");
		return -1;
	}
	this->_http_version = header.substr(pathEnd + 1, versionEnd - (pathEnd + 1));
	return 0;
}

size_t Request::convert_sizet(std::string str)
{
	std::stringstream ss(str);
	size_t num;
	ss >> num;

	if (ss.fail())
	{
		_error = INVALID_SIZE;
		print_error("error: invalid length");
	}
	return num;
}

bool Request::is_header_complete()
{
	std::string raw_str(_raw.begin(), _raw.end());
	// Check if header is complete
	size_t headerEnd = raw_str.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
	{
		_header_length = headerEnd + 4;
		return true;
	}
	return false;
}

bool 	Request::findSequence(const std::vector <unsigned char> &vec, \
	const std::vector<unsigned char>& seq) 
{
	if (seq.size() > vec.size())
		return false;
	for (std::vector<unsigned char>::const_iterator it = vec.begin(); \
		it <= vec.end() - seq.size(); it++)
	{
		if (std::equal(seq.begin(), seq.end(), it))
			return true;
	}
	return false;
}

void	Request::boundary_found()
{
	if (_content_type != "multipart/form-data")
		return ;

	std::vector<unsigned char> boundary_vec;
	// Get "--" + boundary + "--"
    boundary_vec.push_back('-');
    boundary_vec.push_back('-');
    boundary_vec.insert(boundary_vec.end(), _boundary.begin(), _boundary.end());
    boundary_vec.push_back('-');
    boundary_vec.push_back('-');

	if (findSequence(_body, boundary_vec))
    {
    	if (VERBOSE)
    		std::cout << GREEN "set req complete 5" << RESET << std::endl;
    	if (!_encoding_chunked)
    		_req_complete = true;
    }
}

void	Request::appendChunkedBody(char *buffer, int bytes_read)
{
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
            std::cout << GREEN << "set req complete 4" << RESET << std::endl;
            _req_complete = true;
            return ;
        }

        // Move the pointer past the chunk size and \r\n
        i += (chunk_size_end - chunk_size_start) + 2;

        // Ensure there are enough bytes in the buffer for the chunk data and \r\n
        if (i + chunk_size + 2 > bytes_read) 
            break;

        // Append the chunk data to the body
        _body.insert(_body.end(), buffer + i, buffer + i + chunk_size);

        // Move the pointer past the chunk data and \r\n
        i += chunk_size + 2;
    }
}

bool	Request::handle_chunk(char *buffer, int bytes_read)
{
    if (!_is_chunked)
    {
        return false;
    }
 	// Parse the new chunked data
    _raw.insert(_raw.end(), buffer, buffer + bytes_read);
    if (_content_type == "multipart/form-data" && !_encoding_chunked) 
    {
        _body.insert(_body.end(), buffer, buffer + bytes_read);
        if (VERBOSE)
    	{
    		std::cout << RED << "inserting body" <<RESET << std::endl;
    		print_vector(_body);
    	}
    	boundary_found();
    }
    else
    {
    	appendChunkedBody(buffer, bytes_read);
	}
	return _req_complete;
}

void Request::initRequest()
{
	// Check if header is complete
	if (!is_header_complete())
		return ;
	if (VERBOSE)
		std::cout << "initing request" << std::endl;
	std::string header = extractHeader();
	this->parseRequest(header);
	this->parsePort(header);
	this->parseHeader(header);
	// Check if encoding chunked
	if (this->_headers.find("transfer-encoding") != this->_headers.end())
	{
		if (_headers["transfer-encoding"] == "chunked")
		{
			_is_chunked = true;
			_encoding_chunked = true;
		}
	}
	// If content length key found
	if (this->_headers.find("content-length") != this->_headers.end())
	{
		// If chunked -> invalid
		if (_is_chunked == true)
		{
			_error = CHUNK_AND_LENGTH;
			print_error("error: transfer-encoding is chunked and content-length provided");
		}
		else
		{
			_content_length = convert_sizet(_headers["content-length"]);
			// If no error, check if content length met
			if (_error == NO_ERR)
			{
				if (_content_type != "multipart/form-data")
				{
					if (VERBOSE)
						std::cout << GREEN "set req complete 1" << RESET << std::endl;
					_req_complete = true;
				}
			}
		}
	}
    if (_content_type == "multipart/form-data")
    {
        _is_chunked = true;
    }
	// If not chunked and no content length -> req complete
	if (_is_chunked == false && _content_length == -1)
	{
		if (VERBOSE)
			std::cout << GREEN "set req complete 3" << RESET << std::endl;
		_req_complete = true;
	}
}

void Request::getInitialChunk(std::vector<unsigned char>::iterator body_start)
{
	// skip past first \r\n and append
	while (body_start != _raw.end())
	{
		std::vector<unsigned char> chunk_size_delimiter;
        chunk_size_delimiter.push_back('\r');
        chunk_size_delimiter.push_back('\n');

		// get chunk size
		std::vector<unsigned char>::iterator chunk_size_it = \
			std::search(
				body_start, 
				_raw.end(), 
				chunk_size_delimiter.begin(), 
				chunk_size_delimiter.end()
			);

	    if (chunk_size_it == _raw.end())
	    	break;

	    // convert chunk size from hex to decimal
	    std::string chunk_size_str(body_start, chunk_size_it);
	    std::istringstream iss(chunk_size_str);
	    std::size_t chunk_size;
	    iss >> std::hex >> chunk_size;

	    if (chunk_size == 0)
	    {
	    	_req_complete = true;
	    	break;
	    }

	    // check if there is valid data in raw to assign to body
	    body_start = chunk_size_it + 2;
		if (body_start + chunk_size > _raw.end())
			break;

		// assign data from raw to body
		_body.insert(_body.end(), body_start, body_start + chunk_size);
		body_start += chunk_size + 2;
	}
}

void Request::initBody()
{
	// If chunked -> copy body to _body -> continue appending from subsequent chunked reqs
	// If not chunked -> copy body and parse only if req is complete
	if ((this->_header_length != -1 && _is_chunked) || (!_is_chunked && _req_complete))
	{
		std::vector<unsigned char> delimiter;
        delimiter.push_back('\r');
        delimiter.push_back('\n');
        delimiter.push_back('\r');
        delimiter.push_back('\n');

		std::vector<unsigned char>::iterator body_start = \
			std::search(
				_raw.begin(), 
				_raw.end(), 
				delimiter.begin(), 
				delimiter.end()
			);
		// Find start of body for chunked
		if (body_start != _raw.end())
		{
			body_start += 4;
			if (_is_chunked)
			{
				if (_encoding_chunked == true)
					getInitialChunk(body_start);
				else
					_body.assign(body_start, _raw.end());
				boundary_found();
				if (VERBOSE) 
				{
					std::cout << RED << "Current body is size (" << _body.size() << ") " << RESET << std::endl;
					print_vector(_body);
				}
			}
			else
			{
	            if (body_start == _raw.end() && _method == "POST")
	            {
	            	_error = POST_MISSING_BODY;
	            	print_error("error: body missing in POST method");
	            }
                _body.assign(body_start, _raw.end());
			}
		}
	}
}

void	Request::printMap(std::map<std::string, std::string> map)
{
	std::map<std::string, std::string>::iterator it;

	for (it = map.begin(); it != map.end(); it++)
    {
        std::cout << "[" << it->first << "] = " << it->second << std::endl;
    }		
}

/* Function to handle incomplete header
	start copying to end of buf of previous read */
void	Request::handle_incomplete_header(int bytes_read, char *buffer)
{
	_raw.insert(_raw.end(), buffer, buffer + bytes_read);
	// Check if header is complete
	if (!is_header_complete())
		return ;
	else
	{
		this->initRequest();
		this->initBody();
	}
}

void 	Request::checkBodyLength()
{
	if (_body.size() > static_cast<unsigned long>(_content_length) || \
		_body.size() > static_cast<unsigned long>(_body_max_length))
	{
		_error = BODY_TOO_LONG;
		print_error("error: body exceeds content_length or body_max_length");
	}
}

void Request::print_variables() const 
{
  std::cout << "Request Variables:\n";
    std::cout << "Raw: ";
    for (std::vector<unsigned char>::const_iterator it = _raw.begin(); it != _raw.end(); it++)
    	std::cout << *it;
    std::cout << "\n\n";
    std::cout << GREEN << "start printing variables after raw" << RESET << std::endl;
    std::cout << "Header Length: " << _header_length << "\n";
    std::cout << "Request Complete: " << (_req_complete ? "true" : "false") << "\n";
    std::cout << "Body Max Length: " << _body_max_length << "\n";
    std::cout << "Content Length: " << _content_length << "\n";
    std::cout << "Is Chunked: " << (_is_chunked ? "true" : "false") << "\n";
    std::cout << "Method: " << _method << "\n";
    std::cout << "Path: " << _path << "\n";
    std::cout << "HTTP Version: " << _http_version << "\n";
    std::cout << "Port: " << _port << "\n";
    std::cout << "Host: " << _host << "\n";
    std::cout << "Boundary: " << _boundary << "\n";
    std::cout << "Headers:\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
    std::cout << "Request complete: " << (_req_complete ? "true" : "false") << "\n";
    std::cout << "Error: " << (_error) << "\n";
    std::cout << "Body: ";
    for (std::vector<unsigned char>::const_iterator it = _body.begin(); it != _body.end(); it++)
    	std::cout << *it;
    std::cout << std::endl;
}

void Request::print_vector(std::vector<unsigned char> vec) 
{
    for (std::vector<unsigned char>::const_iterator it = vec.begin(); it != vec.end(); it++)
    	std::cout << *it;
    std::cout << std::endl;
}

void Request::copyRawRequest(char *buf, int bytes_read)
{
	size_t curr_size = _raw.size();
	_raw.reserve(curr_size + bytes_read);
	_raw.insert(_raw.end(), buf, buf + bytes_read);
}

void Request::print_error(std::string msg)
{
	std::cerr << msg << std::endl;
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request() {}

Request::Request(char *full_request, int bytes_read) : 
	_header_length(-1),
	_req_complete(false),
	_body_max_length(DEFAULT_BODY_MAX),
	_content_length(-1),
	_is_chunked(false),
	_encoding_chunked(false),
	_error(NO_ERR)
{
	_timeout = time(NULL);
	copyRawRequest(full_request, bytes_read);
	if (this->_raw.size() == 0)
	{
		_error = INVALID_EMPTY_REQ;
		print_error("error: request is empty");
	}
	this->initRequest();
	if (_header_length != -1)
	{
		this->initBody();
	}
	if (VERBOSE)
	{
		// std::cout << _raw << std::endl << RED << "end of req" << RESET << std::endl;
		print_variables();
	}
}

Request::Request( const Request & src ):
	_server(src._server),
	_raw(src._raw),
	_header_length(src._header_length),
	_req_complete(src._req_complete),
	_body_max_length(src._body_max_length),
	_content_length(src._content_length),
	_is_chunked(src._is_chunked),
	_method(src._method),
	_path(src._path),
	_http_version(src._http_version),
	_content_type(src._content_type),
	_boundary(src._boundary),
	_host(src._host),
	_port(src._port),
	_headers(src._headers),
	_body(src._body),
	_error(src._error),
	_config(src._config),
	_formData(src._formData),
	_bodyMap(src._bodyMap),
	_fileMap(src._fileMap)
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
		this->_server = rhs._server;
		this->_raw = rhs._raw;
		this->_header_length = rhs._header_length;
		this->_req_complete = rhs._req_complete;
		this->_body_max_length = rhs._body_max_length;
		this->_content_length = rhs._content_length;
		this->_is_chunked = rhs._is_chunked;
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_http_version = rhs._http_version;
		this->_content_type = rhs._content_type;
		this->_boundary = rhs._boundary;
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_error = rhs._error;
		this->_config = rhs._config;
		this->_formData = rhs._formData;
		this->_bodyMap = rhs._bodyMap;
		this->_fileMap = rhs._fileMap;
	}
	return (*this);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::vector<unsigned char>	Request::getRaw() const
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

std::vector<unsigned char>		Request::getBody() const
{
	return (this->_body);
}

std::string		Request::getHost() const
{
	return (this->_host);
}

int		Request::getPort() const
{
	return (this->_port);
}

ssize_t		Request::getHeaderLength() const
{
	return (this->_header_length);
}

bool	Request::getReqComplete() const
{
	return (this->_req_complete);
}

error_type	Request::getError() const
{
	return (this->_error);
}

Webserver *Request::getServer()
{
	return (this->_server);
}

time_t	Request::getTimeout()
{
	return (this->_timeout);
}

void Request::setBodyMaxLength(size_t len)
{
	this->_body_max_length = len;
}

void	Request::setServer(Webserver* server)
{
	_server = server;
}

/* ************************************************************************** */
