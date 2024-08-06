/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIPost.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 21:18:39 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/06 18:24:10 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIPost.hpp"

CGIPost::CGIPost( CGIPost const & src )
{
	(void)src;
}

CGIPost::~CGIPost()
{

}

CGIPost::CGIPost() : CGIHandler()
{

}

CGIPost &		CGIPost::operator=( CGIPost const & rhs )
{
	(void)rhs;
	return(*this);
}

CGIPost::CGIPost(Request const & request) : CGIHandler()
{
	int	pipe_fd[2];
	int	pipe_data[2];

	this->setFullPath("." + request.getPath());
	if (access(getFullPath().c_str(), F_OK) != 0)
	{
		setError(404);
		return ;
	}

	if (pipe(pipe_fd) == -1 || pipe(pipe_data) == -1)
	{
		std::cout << "Error: " << strerror(errno) << std::endl;
		setError(500);
		return ;
	}

	int	pid = fork();
	if (pid == -1)
	{
		std::cout << "Error: " << strerror(errno) << std::endl;
		setError(500);
		return ;
	}
	if (pid == 0)
		execute_cgi(pipe_fd, pipe_data, request);
	else
		process_result_cgi(pid, pipe_fd, pipe_data, request);
}

// PARENT: Writes the form data to the pipe, then waits for the child to send the result from cgi.
void	CGIPost::process_result_cgi(int pid, int pipe_fd[], int pipe_data[], Request const & request)
{
		close(pipe_fd[1]);
		close(pipe_data[0]);

		std::string str_body(request.getBody().begin(), request.getBody().end());

		write(pipe_data[1], str_body.c_str(), str_body.size());
		close(pipe_data[1]);
        waitpid(pid, NULL, 0);

		char buffer[4096];
		ssize_t bytesRead;

		while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) 
		{
			buffer[bytesRead] = '\0';
			setResult(getResult() += buffer);
		}
		close(pipe_fd[0]);
		setContentType();
		setHtml();
}


// CHILD: Read form data from pipe then send result from cgi via pipe.
void	CGIPost::execute_cgi(int pipe_fd[], int pipe_data[], Request const & request)
{
	close(pipe_fd[0]);
	close(pipe_data[1]);

	dup2(pipe_data[0], STDIN_FILENO); // Read form data from the pipe
	dup2(pipe_fd[1], STDOUT_FILENO); // Write result of script to pipe
	std::string	path = getFullPath();

	char* const argv[] = 
	{
		const_cast<char*>("/usr/bin/python3"),
		const_cast<char*>(path.c_str()),
		NULL
	};

	std::string	content_length = "CONTENT_LENGTH=" + intToString(request.getBody().size());
	std::string	request_method = "REQUEST_METHOD=" + request.getMethod();
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
	setError(500);
}

