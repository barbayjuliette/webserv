/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 14:56:13 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 15:38:00 by jbarbay          ###   ########.fr       */
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
# include "Response.hpp"
# include "LocationConfig.hpp"

class Request;
class Response;
class LocationConfig;

class CGIHandler
{
	private:
		const Request&		_request;
		Response&			_response;
		LocationConfig*		_location;
		std::vector<int>	_request_pipes; // POST
		std::vector<int>	_response_pipes; // GET, POST
		std::string			_cgi_ext;
		std::string			_cgi_exec;
		std::string			_result;
		std::string			_content_type;
		std::string			_html;
		std::string			_headers;
		std::string			_full_path;
		int					_error;

		CGIHandler();

	public:
		CGIHandler(const Request& request, Response& response, std::string cgi_ext);
		CGIHandler(CGIHandler const & src);
		~CGIHandler();
		CGIHandler &		operator=( CGIHandler const & rhs );

		/* Transfer data to/from CGI */
		void		create_response_pipes(void);
		void		create_request_pipes(void);
		void		read_cgi(const Request &request, Response *response, int cgi_status);
		void		write_cgi(const Request &request, Response *response);

		/* Utils */
		std::string	intToString(int num);
		std::string	get_cgi_location(std::string prefix, std::string req_path);
		void		check(int num);

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
		std::vector<int>	get_response_pipes(void);
		std::vector<int>	get_request_pipes(void);
};