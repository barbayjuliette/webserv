/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 14:56:13 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/31 20:35:03 by jbarbay          ###   ########.fr       */
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

		void		execute_cgi(std::string path, int pipe_fd[], int pipe_data[]);
		void		process_result_cgi(int pid, int pipe_fd[], int pipe_data[]);
		const char**	set_environment_cgi(std::string path);


		std::string	getResult();
		std::string	getHtml();
		std::string	getContentType();
		std::string	getFirstLine();
		void		setContentType();
		void		setHtml();
		void		setFirstLine();
		// const char*	set_environment_cgi(std::string path);

};