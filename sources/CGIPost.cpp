/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIPost.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 21:18:39 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 15:53:47 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIPost.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CGIPost::CGIPost(const Request& request, LocationConfig *location, std::string cgi_ext) :
CGIHandler(request, location, cgi_ext)
{
	create_response_pipe();
	create_request_pipe();
}

CGIPost::CGIPost(CGIPost const & src) : CGIHandler(src)
{
	(void)src;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CGIPost &	CGIPost::operator=( CGIPost const & rhs )
{
	CGIHandler::operator=(rhs);
	return(*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CGIPost::~CGIPost() {}


/*
** --------------------------------- METHODS ----------------------------------
*/

/* if event & EPOLLOUT:
if cgi status is CGI_POST: fork, send request to cgi
1. PARENT:
	1. close request_pipe[0] → write to request_pipe[1] → close request_pipe[1] → wait
2. CHILD:
	1. close request_pipe[1] → dup2 request_pipe[0] to stdin → close request_pipe[0]
	2. close response_pipe[0] → dup2 response_pipe[1] to stdout → close response_pipe[1]
	3. execve */
void	CGIPost::write_cgi(int cgi_status)
{
	if (cgi_status != CGI_POST)
		return ;

	std::cout << "INSIDE CGIPOST WRITE_CGI: " << cgi_status << '\n';

	int	pid = fork();
	if (pid == -1)
	{
		std::cerr << "Error fork(): " << strerror(errno) << std::endl;
		setError(500);
		return ;
	}
	if (pid == 0)
	{
		/* CHILD: read form data from request_pipe */
		close(_request_pipe[1]);
		dup2(_request_pipe[0], STDIN_FILENO);
		close(_request_pipe[0]);

		/* CHILD: write result to response_pipe */
		close(_response_pipe[0]);
		dup2(_response_pipe[1], STDOUT_FILENO);
		close(_response_pipe[1]);

		execute_cgi(cgi_status);
	}
	else
	{
		/* PARENT: write form data to request_pipe */
		close(_request_pipe[0]);
		std::vector<unsigned char> body = _request.getBody();

		int bytes = write(_request_pipe[1], reinterpret_cast<const char *>(body.data()), body.size());
		if (bytes <= 0)
		{
			std::cerr << "Error write(): " << strerror(errno) << std::endl;
			setError(500);
			close(_response_pipe[0]);
			close(_request_pipe[1]);
			return ;
		}
		close(_request_pipe[1]);
		waitpid(pid, NULL, 0);
	}
}





// CGIPost::CGIPost(Request const & request, LocationConfig* location, std::string ext) : CGIHandler()
// {
// 	int	pipe_fd[2];
// 	int	pipe_data[2];

// 	this->_cgi_exec = location->getCGIExec(ext);
// 	setFullPath(get_cgi_location(location->getPrefix(), request.getPath()));
// 	std::cout << "set full path: " << getFullPath() << '\n';
// 	if (access(getFullPath().c_str(), F_OK) != 0)
// 	{
// 		setError(404);
// 		return ;
// 	}

// 	if (pipe(pipe_fd) == -1 || pipe(pipe_data) == -1)
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
// 		execute_cgi(pipe_fd, pipe_data, request);
// 	else
// 		process_result_cgi(pid, pipe_fd, pipe_data, request);
// }

// // PARENT: Writes the form data to the pipe, then waits for the child to send the result from cgi.
// void	CGIPost::process_result_cgi(int pid, int pipe_fd[], int pipe_data[], Request const & request)
// {
// 		close(pipe_fd[1]); //response pipe
// 		close(pipe_data[0]); //request pipe
// 		int	bytes;

// 		std::vector<unsigned char> body = request.getBody();

// 		bytes = write(pipe_data[1], reinterpret_cast<const char *>(body.data()), body.size());
// 		if (bytes <= 0)
// 		{
// 			std::cerr << "Error write(): " << strerror(errno) << std::endl;
// 			setError(500);
// 			close(pipe_fd[0]); //response pipe
// 			close(pipe_data[1]); //request pipe
// 			return ;
// 		}
// 		close(pipe_data[1]);
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


// // CHILD: Read form data from pipe, then send result from cgi via pipe.
// void	CGIPost::execute_cgi(int pipe_fd[], int pipe_data[], Request const & request)
// {
// 	close(pipe_fd[0]);
// 	close(pipe_data[1]);

// 	dup2(pipe_data[0], STDIN_FILENO); // Read form data from the pipe
// 	dup2(pipe_fd[1], STDOUT_FILENO); // Write result of script to pipe
// 	std::string	path = getFullPath();

// 	char* const argv[] = 
// 	{
// 		const_cast<char*>(this->_cgi_exec.c_str()),
// 		const_cast<char*>(path.c_str()),
// 		NULL
// 	};

// 	std::string	content_length = "CONTENT_LENGTH=" + intToString(request.getBody().size());
// 	std::string	request_method = "REQUEST_METHOD=" + request.getMethod();
// 	std::string	content_type = "CONTENT_TYPE=" + request.getHeaders()["content-type"];
// 	// TO DO Choose which headers to put based on tester
// 	// std::string	gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
// 	// std::string	path_info = "PATH_INFO=" + path;
// 	// std::string	path_translated = "PATH_TRANSLATED";
// 	// std::string	query_string = "QUERY_STRING";
// 	// std::string	remote_addr = "REMOTE_ADDR";
// 	// std::string	remote_host = "REMOTE_HOST";
// 	// std::string	remote_ident = "REMOTE_IDENT";
// 	// std::string	remote_user = "REMOTE_USER";
// 	// std::string	auth_type = "AUTH_TYPE=null";
// 	// std::string	script_name = "SCRIPT_NAME";
// 	// std::string	server_name = "SERVER_NAME";
// 	// std::string	server_port = "SERVER_PORT";
// 	// std::string	server_protocol = "SERVER_PROTOCOL";
// 	// std::string	server_software = "SERVER_SOFTWARE";

// 	const char* env[] = {
// 		content_length.c_str(),
// 		request_method.c_str(),
// 		content_type.c_str(),
// 		NULL
// 	};
// 	close(pipe_fd[1]);
// 	close(pipe_data[0]);
// 	execve(this->_cgi_exec.c_str(), argv, const_cast<char* const*>(env));
// 	std::cerr << "Error execve: " << strerror(errno) << std::endl;
// 	setError(500);
// }

