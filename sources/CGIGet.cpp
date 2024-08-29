/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIGet.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:00:25 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/29 14:58:30 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIGet.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CGIGet::CGIGet() : CGIHandler()
{

}

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

	_pid = fork();
	if (_pid == -1)
	{
		std::cerr << "Error fork(): " << strerror(errno) << std::endl;
		setError(500);
		return ;
	}
	if (_pid == 0)
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
		waitpid(_pid, NULL, 0);
	}
}

void	CGIGet::read_cgi_request(int cgi_status)
{
	(void)cgi_status;
}
