/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIPost.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 21:18:39 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/29 14:49:35 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIPost.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
CGIPost::CGIPost() : CGIHandler()
{}

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

void	CGIPost::read_cgi_request(int cgi_status)
{
	if (cgi_status != CGI_POST_READ)
		return ;

	if (_pid == 0)
	{
		/* CHILD: write result to response_pipe */
		close(_response_pipe[0]);
		dup2(_response_pipe[1], STDOUT_FILENO);
		close(_response_pipe[1]);

		execute_cgi(cgi_status);
	}
}

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

	_pid = fork();
	if (_pid == -1)
	{
		std::cerr << "Error fork(): " << strerror(errno) << std::endl;
		setError(500);
		return ;
	}
	if (_pid == 0)
	{
		/* CHILD: read form data from request_pipe */
		close(_request_pipe[1]);
		dup2(_request_pipe[0], STDIN_FILENO);
		close(_request_pipe[0]);
		return ;
	}
	else
	{
		/* PARENT: write form data to request_pipe */
		close(_request_pipe[0]);
		close(_response_pipe[1]);
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
		waitpid(_pid, NULL, 0);
	}
}
