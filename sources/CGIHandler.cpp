/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/01 17:57:42 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

CGIHandler::CGIHandler( CGIHandler const & src )
{
	(void)src;
}

CGIHandler::~CGIHandler()
{

}

CGIHandler::CGIHandler()
{

}

CGIHandler &		CGIHandler::operator=( CGIHandler const & rhs )
{
	(void)rhs;
	return(*this);
}

CGIHandler::CGIHandler(Request const & request)
{
	int	pipe_fd[2];
	int	pipe_data[2];

	if (pipe(pipe_fd) == -1 || pipe(pipe_data) == -1)
	{
		std::cout << "Error: " << strerror(errno) << std::endl;
		return ;
	}

	int	pid = fork();
	if (pid == -1)
	{
		std::cout << "Error: " << strerror(errno) << std::endl;
		return ;
	}
	if (pid == 0)
		execute_cgi(request.getPath(), pipe_fd, pipe_data, request);
	else
		process_result_cgi(pid, pipe_fd, pipe_data, request);
}

// PARENT: Writes the form data to the pipe, then waits for the child to send the result from cgi.
void	CGIHandler::process_result_cgi(int pid, int pipe_fd[], int pipe_data[], Request const & request)
{
		close(pipe_fd[1]);
		close(pipe_data[0]);

		write(pipe_data[1], request.getBody().c_str(), request.getBody().size());
		close(pipe_data[1]);
        waitpid(pid, NULL, 0);

		char buffer[4096];
		ssize_t bytesRead;

		while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) 
		{
			buffer[bytesRead] = '\0';
			_result += buffer;
		}
		close(pipe_fd[0]);
		// std::cout << RED << "Result CGI: " << _result << RESET << std::endl;
		setContentType();
		setHtml();
}

std::string	CGIHandler::intToString(int num)
{
	std::stringstream	stream;
	stream << num;
	return (stream.str());
}

// CHILD: Read form data from pipe then send result from cgi via pipe.
void	CGIHandler::execute_cgi(std::string path, int pipe_fd[], int pipe_data[], Request const & request)
{
	close(pipe_fd[0]);
	close(pipe_data[1]);

	dup2(pipe_data[0], STDIN_FILENO); // Read form data from the pipe
	dup2(pipe_fd[1], STDOUT_FILENO); // Write result of script to pipe
	path = "." + path;

	char* const argv[] = 
	{
		const_cast<char*>("/usr/bin/python3"),
		const_cast<char*>(path.c_str()),
		NULL
	};

	std::string	content_length = "CONTENT_LENGTH=" + intToString(request.getBody().size());
	std::string	request_method = "REQUEST_METHOD=POST";
	std::string	content_type = "CONTENT_TYPE=" + request.getHeaders()["content-type"];
	// TO DO Choose which headers to put based on tester
	// std::string	gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
	// std::string	path_info = "PATH_INFO=" + path;
	// std::string	path_translated = "PATH_TRANSLATED";
	// std::string	query_string = "QUERY_STRING";
	// std::string	remote_addr = "REMOTE_ADDR";
	// std::string	remote_host = "REMOTE_HOST";
	// std::string	remote_ident = "REMOTE_IDENT";
	// std::string	remote_user = "REMOTE_USER";
	// std::string	auth_type = "AUTH_TYPE=null";
	// std::string	script_name = "SCRIPT_NAME";
	// std::string	server_name = "SERVER_NAME";
	// std::string	server_port = "SERVER_PORT";
	// std::string	server_protocol = "SERVER_PROTOCOL";
	// std::string	server_software = "SERVER_SOFTWARE";

	const char* env[] = {
		content_length.c_str(),
		request_method.c_str(),
		content_type.c_str(),
		NULL
	};
	close(pipe_fd[1]);
	close(pipe_data[0]);
	execve("/usr/bin/python3", argv, const_cast<char* const*>(env));
	std::cout << "Execve failed\n";
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
