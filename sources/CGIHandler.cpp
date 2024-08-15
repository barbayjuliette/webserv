/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 14:31:57 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CGIHandler::CGIHandler(const Request& request, LocationConfig *location, std::string cgi_ext) :
_request(request),
_location(location),
_cgi_ext(cgi_ext),
_result(""),
_content_type(""),
_html(""),
_headers(""),
_error(0)
{
	_cgi_exec = _location->getCGIExec(_cgi_ext);
	_full_path = get_cgi_location(_location->getPrefix(), _request.getPath());

	if (access(_full_path.c_str(), F_OK) != 0)
		setError(404);
}

CGIHandler::CGIHandler(CGIHandler const & src) :
_request(src._request),
_location(src._location),
_request_pipe(src._request_pipe),
_response_pipe(src._response_pipe),
_cgi_ext(src._cgi_ext),
_cgi_exec(src._cgi_exec),
_result(src._result),
_content_type(src._content_type),
_html(src._html),
_headers(src._headers),
_full_path(src._full_path),
_error(src._error)
{}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CGIHandler &		CGIHandler::operator=( CGIHandler const & rhs )
{
	if (this != &rhs)
	{
		this->_location = rhs._location;
		this->_request_pipe = rhs._request_pipe;
		this->_response_pipe = rhs._response_pipe;
		this->_cgi_ext = rhs._cgi_ext;
		this->_cgi_exec = rhs._cgi_exec;
		this->_result = rhs._result;
		this->_content_type = rhs._content_type;
		this->_html = rhs._html;
		this->_headers = rhs._headers;
		this->_full_path = rhs._full_path;
		this->_error = rhs._error;
	}
	return(*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CGIHandler::~CGIHandler() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	CGIHandler::create_response_pipe(void)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "Read pipe(): " << strerror(errno) << std::endl;
		exit(1);
	}

	std::vector<int>::iterator it = _response_pipe.begin();
	while (it != _response_pipe.end())
	{
		close(*it);
		_response_pipe.erase(it);
		it = _response_pipe.begin();
	}
	_response_pipe.push_back(pipefd[0]);
	_response_pipe.push_back(pipefd[1]);
}

void	CGIHandler::create_request_pipe(void)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "Write pipe(): " << strerror(errno) << std::endl;
		exit(1);
	}

	std::vector<int>::iterator it = _request_pipe.begin();
	while (it != _request_pipe.end())
	{
		close(*it);
		_request_pipe.erase(it);
		it = _request_pipe.begin();
	}
	_request_pipe.push_back(pipefd[0]);
	_request_pipe.push_back(pipefd[1]);
}

/* Called by child process; closing of fds to be handled before calling this function */
void	CGIHandler::execute_cgi(int cgi_status)
{
	char* const argv[] = 
	{
		const_cast<char*>(_cgi_exec.c_str()),
		const_cast<char*>(_full_path.c_str()),
		NULL
	};

	std::string	content_length = "CONTENT_LENGTH=" + intToString(_request.getBody().size());

	std::string	request_method;
	if (cgi_status == CGI_GET)
		request_method = "REQUEST_METHOD=GET";
	else if (cgi_status == CGI_POST)
		request_method = "REQUEST_METHOD=POST";

	std::string	content_type = "CONTENT_TYPE=" + _request.getHeaders()["content-type"];
	std::string	gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
	std::string	path_info = "PATH_INFO=" + getFullPath();
	std::string	script_name = "SCRIPT_NAME=" + getFullPath();
	std::string	server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string	server_software = "SERVER_SOFTWARE=42webserv";
	// std::string	query_string = "QUERY_STRING";
	// std::string	remote_addr = "REMOTE_ADDR";
	// std::string	remote_host = "REMOTE_HOST";
	// std::string	remote_ident = "REMOTE_IDENT";
	// std::string	remote_user = "REMOTE_USER";
	// std::string	server_name = "SERVER_NAME";
	// std::string	server_port = "SERVER_PORT";

	const char* env[] =
	{
		content_length.c_str(),
		request_method.c_str(),
		content_type.c_str(),
		gateway_interface.c_str(),
		path_info.c_str(),
		script_name.c_str(),
		server_protocol.c_str(),
		server_software.c_str(),
		NULL
	};

	execve(_cgi_exec.c_str(), argv, const_cast<char* const*>(env));

	std::cerr << "Error execve: " << strerror(errno) << std::endl;
	setError(500);
}

/*
4. if event & EPOLLIN:
    1. if cgi status is CGI_GET: read from response_pipe[0]
    2. if cgi status is CGI_POST: read from response_pipe[0]
5. close pipe fds and remove them from the Cluster map / Webserver vectors */

/* Parent: reads result of the cgi script from the pipe */
void	CGIHandler::read_cgi_result(int cgi_status)
{
	if (cgi_status != CGI_GET && cgi_status != CGI_POST)
		return ;

	char buffer[500];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytesRead = read(_response_pipe[0], buffer, 500);
	
	if (bytesRead == 0)
	{
		std::cerr << "Error: No result CGI" << std::endl;
		setError(500);
		close(_response_pipe[0]);
		return ;
	}
	while (bytesRead > 0)
	{
		bytesRead = read(_response_pipe[0], buffer, 500);
		if (bytesRead < 0)
		{
			std::cerr << strerror(errno) << std::endl;
			setError(500);
			close(_response_pipe[0]);
			return ;
		}
		setResult(getResult() + buffer);
	}
	close(_response_pipe[0]);

	setContentType();
	setHtml();
}

void		CGIHandler::setHeaders()
{
	std::size_t		pos = _result.find("\r\n\r\n", 0);

	if (pos == std::string::npos)
		_headers = "";
	else
		_headers = _result.substr(0, pos);
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
		std::cerr << "No Content-type found in CGI\n";
		return ;
	}

	this->_content_type = _headers.substr(pos + 13, low.find("/n", pos + 13) - 1);
}

void	CGIHandler::setHtml()
{
	std::string		delim = "\r\n\r\n";
	std::size_t		pos = _result.find(delim, 0);

	if (pos == std::string::npos)
		_html = _result;
	else
		_html = _result.substr(pos + delim.size() + 1, _result.size());
}

/*
** ---------------------------------- UTILS -----------------------------------
*/

std::string	CGIHandler::intToString(int num)
{
	std::stringstream	stream;
	stream << num;
	return (stream.str());
}

std::string		CGIHandler::get_cgi_location(std::string prefix, std::string req_path)
{
	if (!req_path.compare(0, prefix.size(), prefix))
		return ("." + req_path);
	if (prefix[prefix.size() - 1] == '/')
		return ("." + prefix.substr(0, prefix.size() - 1) + req_path);
	return ("." + prefix + req_path);
}

void	CGIHandler::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}
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

std::vector<int>	CGIHandler::get_response_pipe(void)
{
	return (_response_pipe);
}

std::vector<int>	CGIHandler::get_request_pipe(void)
{
	return (_request_pipe);
}
