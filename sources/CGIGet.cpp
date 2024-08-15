/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIGet.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/02 20:26:17 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIGet.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CGIGet::CGIGet(const Request& request, LocationConfig *location, std::string cgi_ext) :
CGIHandler(request, location, cgi_ext)
{
	create_response_pipe();
}

CGIGet::CGIGet(CGIGet const & src) : CGIHandler(src)
{
	(void)src;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CGIGet &	CGIGet::operator=( CGIGet const & rhs )
{
	CGIHandler::operator=(rhs);
	return(*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CGIGet::~CGIGet() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* if event & EPOLLOUT:
if cgi status is CGI_GET: fork, send cgi response to pipe via `execute_cgi` 
1. PARENT: close response_pipe[1] → wait
2. CHILD: close response_pipe[0] → dup2 response_pipe[1] to stdout → close response_pipe[1] → execve */
void	CGIGet::write_cgi(int cgi_status)
{
	if (cgi_status != CGI_GET)
		return ;

	std::cout << "INSIDE CGIGET WRITE_CGI: " << cgi_status << '\n';
	int	pid = fork();
	if (pid == -1)
	{
		std::cerr << "Error fork(): " << strerror(errno) << std::endl;
		setError(500);
		return ;
	}
	if (pid == 0)
	{
		/* CHILD: write result to response_pipe */
		close(_response_pipe[0]);
		dup2(_response_pipe[1], STDOUT_FILENO);
		close(_response_pipe[1]);

		execute_cgi(cgi_status);
	}
	else
	{
		/* PARENT: close response_pipe[1] → wait */
		close(_response_pipe[1]);
		waitpid(pid, NULL, 0);
	}
}






// CGIGet::CGIGet(Request const & request, LocationConfig* location) : CGIHandler()
// {
// 	// int	pipe_fd[2];
// 	std::string req_path = request.getPath();
// 	std::string ext = req_path.substr(req_path.rfind('.'), std::string::npos);

// 	this->_cgi_exec = location->getCGIExec(ext);
// 	setFullPath(get_cgi_location(location->getPrefix(), request.getPath()));

// 	if (access(getFullPath().c_str(), F_OK) != 0)
// 	{
// 		setError(404);
// 		return ;
// 	}

// 	if (pipe(pipe_fd) == -1)
// 	{
// 		std::cerr << "Error pipe(): " << strerror(errno) << std::endl;
// 		setError(500);
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

// // PARENT: Waits for the child to send the result from cgi, then reads.
// void	CGIGet::process_result_cgi(int pid, int pipe_fd[])
// {
// 		close(pipe_fd[1]);
//         waitpid(pid, NULL, 0);

// 		char buffer[500];
// 		memset(buffer, 0, sizeof(buffer));
// 		ssize_t bytesRead = read(pipe_fd[0], buffer, 500);
		
// 		if (bytesRead == 0)
// 		{
// 			std::cerr << "Error: No result CGI" << std::endl;
// 			setError(500);
// 			close(pipe_fd[0]);
// 			return ;
// 		}
// 		while (bytesRead > 0)
// 		{
// 			bytesRead = read(pipe_fd[0], buffer, 500);
// 			if (bytesRead < 0)
// 			{
// 				std::cerr << strerror(errno) << std::endl;
// 				setError(500);
// 				close(pipe_fd[0]);
// 				return ;
// 			}
// 			setResult(getResult() + buffer);
// 		}
// 		close(pipe_fd[0]);
// 		setContentType();
// 		setHtml();
// }

// // CHILD: Send result from cgi via pipe.
// void	CGIGet::execute_cgi(int pipe_fd[], Request const & request)
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
