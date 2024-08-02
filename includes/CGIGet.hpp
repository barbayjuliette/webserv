/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIGet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 20:33:16 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/02 20:39:50 by jbarbay          ###   ########.fr       */
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

class CGIGet
{
	private:
		std::string		_result;
		std::string		_content_type;
		std::string		_html;
		std::string		_headers;

	public:
		CGIGet();
		CGIGet(Request const & request);
		CGIGet( CGIGet const & src );
		~CGIGet();
		CGIGet &		operator=( CGIGet const & rhs );

		void			execute_cgi(std::string path, int pipe_fd[], Request const & request);
		void			process_result_cgi(int pid, int pipe_fd[]);
		std::string		intToString(int num);


		std::string	getResult();
		std::string	getHtml();
		std::string	getContentType();
		void		setContentType();
		void		setHtml();
		void		setHeaders();

};