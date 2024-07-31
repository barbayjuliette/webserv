/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/31 18:02:47 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

CGIHandler::CGIHandler(Request const & request)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
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
		execute_cgi(request.getPath(), pipe_fd);
	else
		process_result_cgi(pid, pipe_fd);
}

void	CGIHandler::process_result_cgi(int pid, int pipe_fd[])
{
		close(pipe_fd[1]);
        int status;
        waitpid(pid, &status, 0);

		// std::cout << "In Parent process\n";

		char buffer[4096];
		ssize_t bytesRead;

		while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) 
		{
			buffer[bytesRead] = '\0';
			_result += buffer;
		}
		close(pipe_fd[0]);
		std::cout << RED << "Result CGI: " << _result << RESET << std::endl;
		setContentType();
		setHtml();
}

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


void	CGIHandler::execute_cgi(std::string path, int pipe_fd[])
{
		std::cout << "In Child process\n";
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		path = "/home/jbarbay/webserv" + path;
		// std::cout << "Path: " << path << std::endl;

		char* const argv[] = 
		{
			const_cast<char*>("/usr/bin/python3"),
			const_cast<char*>(path.c_str()),
			NULL
		};

		const char* env[] = {
			"red=on",
			"blue=off",
			NULL
		};
		execve("/usr/bin/python3", argv, const_cast<char* const*>(env));
		std::cout << "Execve failed\n";
}

void	CGIHandler::handle_cgi(std::string path)
{
	(void)path;
}

// -------------------- ACCESSORS --------------------

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

void		CGIHandler::setFirstLine()
{
	std::size_t		pos = _result.find("\r\n\r\n", 0);

	if (pos == std::string::npos)
	{
		// TO DO Error
		std::cout << "No CGI first line found\n";
		_first_line = "";
		return ;
	}
	_first_line = _result.substr(0, pos);
	// std::cout << "First line: " << _first_line << std::endl;
}

void	CGIHandler::setContentType()
{
	setFirstLine();

	std::string	low = _first_line;
	for (size_t i = 0; i < _first_line.size() ; i++)
	{
		low[i] = (char)tolower(_first_line[i]);
	}

	std::size_t		pos = low.find("content-type:", 0);
	if (pos == std::string::npos)
	{
		// TO DO Error 
		// this->_content_type = "text/plain";
		std::cout << "No Content-type found in CGI\n";
		return ;
	}

	this->_content_type = _first_line.substr(pos + 13, low.find("/n", pos + 13) - 1);
	std::cout << "Content_type: " << _content_type << std::endl;
}

void	CGIHandler::setHtml()
{
	std::string		delim = "\r\n\r\n";
	std::size_t		pos = _result.find(delim, 0);

	_html = _result.substr(pos + delim.size() + 1, _result.size());
	std::cout << "HTML: " << _html << std::endl;
}
