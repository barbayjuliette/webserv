/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/31 20:08:12 by jbarbay          ###   ########.fr       */
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
		execute_cgi(request.getPath(), pipe_fd, pipe_data);
	else
		process_result_cgi(pid, pipe_fd, pipe_data);
}

// PARENT
void	CGIHandler::process_result_cgi(int pid, int pipe_fd[], int pipe_data[])
{
		close(pipe_fd[1]);
		close(pipe_data[0]);

		write(pipe_data[1], "red=off&blue=on", 15);
		close(pipe_data[1]);
        waitpid(pid, NULL, 0);

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



// CHILD
void	CGIHandler::execute_cgi(std::string path, int pipe_fd[], int pipe_data[])
{
	std::cout << "In Child process\n";
	close(pipe_fd[0]);
	close(pipe_data[1]);

	dup2(pipe_fd[1], STDOUT_FILENO); // Write result of script to pipe
	dup2(pipe_data[0], STDIN_FILENO); // Read form data from the pipe
	path = "." + path;

	char* const argv[] = 
	{
		const_cast<char*>("/usr/bin/python3"),
		const_cast<char*>(path.c_str()),
		NULL
	};

	const char* env[] = {
		"CONTENT_LENGTH=15",
		"REQUEST_METHOD=POST",
		"CONTENT_TYPE=application/x-www-form-urlencoded",
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

	if (pos == std::string::npos)
	{
		// TO DO Error 
		// this->_content_type = "text/plain";
		std::cout << "No html found in CGI\n";
		return ;
	}
	_html = _result.substr(pos + delim.size() + 1, _result.size());
	std::cout << "HTML: " << _html << std::endl;
}
// const char*		CGIHandler::set_environment_cgi(std::string path)
// {
// 	std::string request_method = "POST";
//     std::string content_type = "application/x-www-form-urlencoded"; // or other MIME type
//     std::string content_length = "15";
//     std::string script_name = path;
//     std::string server_name = "localhost";
//     std::string server_port = "8080";
//     std::string server_protocol = "HTTP/1.1";
//     std::string remote_addr = "127.0.0.1";
//     std::string remote_port = "12345";

//     // Environment variables for the CGI script
//     const char* env[] = {
//         ("REQUEST_METHOD=" + request_method).c_str(),
//         ("CONTENT_TYPE=" + content_type).c_str(),
//         ("CONTENT_LENGTH=" + content_length).c_str(),
//         ("SCRIPT_NAME=" + script_name).c_str(),
//         ("SERVER_NAME=" + server_name).c_str(),
//         ("SERVER_PORT=" + server_port).c_str(),
//         ("SERVER_PROTOCOL=" + server_protocol).c_str(),
//         ("REMOTE_ADDR=" + remote_addr).c_str(),
//         ("REMOTE_PORT=" + remote_port).c_str(),
//         NULL
//     };
// 	return (env);
// }
