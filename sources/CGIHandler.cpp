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

CGIHandler::CGIHandler() : {}

CGIHandler::CGIHandler(const Request& request, Response& response, std::string cgi_ext) :
_request(request),
_response(response),
_cgi_ext(cgi_ext),
_error(0)
{
	_location = response.getLocation();
	create_response_pipes();
	if (_response.getCGIStatus() == CGI_POST_WRITE)
		create_request_pipes();
}

CGIHandler::CGIHandler(CGIHandler const & src) :
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

void	CGIHandler::create_response_pipes(void)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "Read pipe(): " << strerror(errno) << std::endl;
		exit(1);
	}

	std::vector<int>::iterator it = _response_pipes.begin();
	while (it != _response_pipes.end())
	{
		close(*it);
		_response_pipes.erase(it);
		it = _response_pipes.begin();
	}
	_response_pipes.push_back(pipefd[0]);
	_response_pipes.push_back(pipefd[1]);
}

void	CGIHandler::create_request_pipes(void)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "Write pipe(): " << strerror(errno) << std::endl;
		exit(1);
	}

	std::vector<int>::iterator it = _request_pipes.begin();
	while (it != _request_pipes.end())
	{
		close(*it);
		_request_pipes.erase(it);
		it = _request_pipes.begin();
	}
	_request_pipes.push_back(pipefd[0]);
	_request_pipes.push_back(pipefd[1]);
}

/* 3. if event & EPOLLOUT:
    1. if cgi status is CGI_GET: fork, send cgi response to pipe via `execute_cgi` 
        1. PARENT: close response_pipe[1] → wait
        2. CHILD: close response_pipe[0] → dup2 response_pipe[1] to stdout → close response_pipe[1] → execve
    2. if cgi status is CGI_POST_WRITE: fork, send request to cgi
        1. PARENT:
            1. close request_pipe[0] → write to request_pipe[1] → close request_pipe[1] → wait
            2. after child exits, set cgi status to CGI_POST_READ
        2. CHILD:
            1. close request_pipe[1] → dup2 request_pipe[0] to stdin → close request_pipe[0]
            2. close response_pipe[0] → dup2 response_pipe[1] to stdout → close response_pipe[1]
            3. execve
4. if event & EPOLLIN:
    1. if cgi status is CGI_GET: read from response_pipe[0]
    2. if cgi status is CGI_POST_READ: read from response_pipe[0]
5. close pipe fds and remove them from the Cluster map / Webserver vectors */

// void	CGIHandler::read_cgi(const Request &request, Response *response, int cgi_status)
// {
// 	std::string req_path = request.getPath();
// 	std::string ext = req_path.substr(req_path.rfind('.'), std::string::npos);

// 	this->_cgi_exec = location->getCGIExec(ext);
// 	setFullPath(get_cgi_location(location->getPrefix(), request.getPath()));

// 	if (access(getFullPath().c_str(), F_OK) != 0)
// 	{
// 		setError(404);
// 		return ;
// 	}

// 	int	pid = fork();
// 	if (pid == -1)
// 	{
// 		std::cerr << "Error fork(): " << strerror(errno) << std::endl;
// 		setError(500);
// 		return ;
// 	}
// 	if (pid == 0)
// 		execute_cgi(pipe_fd, request);
// 	else
// 		process_result_cgi(pid, pipe_fd);
// }

// /* CHILD: Reads form data from pipe, then sends result from cgi via pipe.
// PARENT: 
// */
// void	CGIHandler::execute_cgi(std::vector<int>& pipefd, Request const & request)
// {
// 	close(pipe_fd[0]);

// 	dup2(pipe_fd[1], STDOUT_FILENO); // Write result of script to pipe
// 	std::string	path = getFullPath();

// 	char* const argv[] = 
// 	{
// 		const_cast<char*>(this->_cgi_exec.c_str()),
// 		const_cast<char*>(path.c_str()),
// 		NULL
// 	};

// 	std::string	content_length = "CONTENT_LENGTH=" + intToString(request.getBody().size());
// 	std::string	request_method = "REQUEST_METHOD=GET";
// 	std::string	content_type = "CONTENT_TYPE=" + request.getHeaders()["content-type"];

// 	std::string	gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
// 	std::string	path_info = "PATH_INFO=" + getFullPath();
// 	std::string	script_name = "SCRIPT_NAME=" + getFullPath();
// 	std::string	server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
// 	std::string	server_software = "SERVER_SOFTWARE=42webserv";
// 	// std::string	query_string = "QUERY_STRING";
// 	// std::string	remote_addr = "REMOTE_ADDR";
// 	// std::string	remote_host = "REMOTE_HOST";
// 	// std::string	remote_ident = "REMOTE_IDENT";
// 	// std::string	remote_user = "REMOTE_USER";
// 	// std::string	server_name = "SERVER_NAME";
// 	// std::string	server_port = "SERVER_PORT";

// 	const char* env[] = {
// 		content_length.c_str(),
// 		request_method.c_str(),
// 		content_type.c_str(),
// 		NULL
// 	};
// 	close(pipe_fd[1]);
// 	execve(this->_cgi_exec.c_str(), argv, const_cast<char* const*>(env));
// 	std::cerr << "Error execve: " << strerror(errno) << std::endl;
// 	setError(500);
// }

// void	Response::process_cgi_response(CGIHandler* cgi)
// {
// 	if (cgi->getError() != 0)
// 		set_error(cgi->getError());
// 	else
// 	{
// 		_body = cgi->getHtml();
// 		_headers["Content-Type"] = cgi->getContentType();
// 		set_success();
// 	}
// 	_headers["Content-Length"] = intToString(this->_body.size());
// }

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

std::vector<int>	CGIHandler::get_response_pipes(void)
{
	return (_response_pipes);
}

std::vector<int>	CGIHandler::get_request_pipes(void)
{
	return (_request_pipes);
}
