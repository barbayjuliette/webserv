/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 14:56:13 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/29 14:50:31 by jbarbay          ###   ########.fr       */
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
# include "Request.hpp"
# include "LocationConfig.hpp"

enum cgi_status
{
	NO_CGI,
	CGI_GET,
	CGI_POST,
	CGI_POST_READ,
	CGI_DONE
};

class Request;
class LocationConfig;

class CGIHandler
{
	protected:
		const Request&		_request;
		LocationConfig*		_location;
		std::vector<int>	_request_pipe; // POST
		std::vector<int>	_response_pipe; // GET, POST
		std::string			_cgi_ext;
		std::string			_cgi_exec;
		std::string			_result;
		std::string			_content_type;
		std::string			_html;
		std::string			_headers;
		std::string			_full_path;
		int					_error;
		int					_pid;

	public:
		CGIHandler();
		CGIHandler(const Request& request, LocationConfig *location, std::string cgi_ext);
		CGIHandler(CGIHandler const & src);
		virtual ~CGIHandler();
		CGIHandler &		operator=( CGIHandler const & rhs );

		/* Transfer data to/from CGI */
		void			create_response_pipe(void);
		void			create_request_pipe(void);
		void			read_cgi_result(int cgi_status);
		void			execute_cgi(int cgi_status);
		virtual void	write_cgi(int cgi_status) = 0;
		virtual void	read_cgi_request(int cgi_status) = 0;

		/* Utils */
		std::string	intToString(int num);
		std::string	get_cgi_location(std::string prefix, std::string req_path);
		void		check(int num);

		/* Accessors */
		std::string	getResult();
		std::string	getHtml();
		std::string	getContentType();
		void		setContentType();
		void		setHtml();
		void		setHeaders();
		void		setResult(std::string result);
		std::string	getFullPath();
		void		setFullPath(std::string path);
		int			getError();
		void		setError(int error);
		int			get_pid();
		std::vector<int>	get_response_pipe(void);
		std::vector<int>	get_request_pipe(void);
};