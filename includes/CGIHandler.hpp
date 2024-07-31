/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 14:56:13 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/31 17:52:13 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <errno.h>
# include <string.h>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <cstdio>
# include <cstdlib>
# include <fstream>
# include <iostream>
# include "webserv.hpp"
# include "Response.hpp"

class Response;

class CGIHandler
{
	private:
		std::string		_result;
		std::string		_content_type;
		std::string		_html;
		std::string		_first_line;

	public:
		CGIHandler();
		CGIHandler(Request const & request);
		CGIHandler( CGIHandler const & src );
		~CGIHandler();
		CGIHandler &		operator=( CGIHandler const & rhs );

		void		handle_cgi(std::string path);
		void		execute_cgi(std::string path, int pipe_fd[]);
		void		process_result_cgi(int pid, int pipe_fd[]);

		std::string	getResult();
		std::string	getHtml();
		std::string	getContentType();
		std::string	getFirstLine();
		void		setContentType();
		void		setHtml();
		void		setFirstLine();
};