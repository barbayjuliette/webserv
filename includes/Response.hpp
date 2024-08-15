/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:05:36 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/08 17:05:51 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include "Request.hpp"
# include <errno.h>
# include <string.h>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <map>
# include <ctime>
# include <cstdio>
# include <cstdlib>
# include <fstream>
# include <iostream>
# include <dirent.h>
# include "ConfigFile.hpp"
# include "CGIHandler.hpp"
# include "CGIGet.hpp"
# include "CGIPost.hpp"
# include "Print.hpp"

class CGIHandler;
class CGIGet;
class CGIPost;

class Response
{
	private:
		static std::map<int, std::string>	_status_lookup;
		int									_status_code;
		std::string							_status_text;
		int									_cgi_status;
		std::string							_http_version;
		std::string							_body;
		std::string							_full_response;
		std::string							_path;
		std::map<std::string, std::string>	_headers;
		ServerConfig*						_config;
		LocationConfig*						_location;
		CGIHandler*							_cgi_handler;

		Response();

	public:
		Response(Request &request, ServerConfig *conf);
		Response( Response const & src );
		~Response();
		Response &		operator=( Response const & rhs );

		/* Error/Status codes */
		void		init_status_lookup(void);
		void		set_success(void);
		void		set_error(int code);
		std::string	get_error_page(int num);

		/* Handle HTTP methods */
		bool		method_is_allowed(std::string, std::vector<std::string> allowed);
		void		respond_get_request(const Request &request);
		void		respond_post_request(const Request &request);
		void		respond_delete_request(void);
		void		respond_redirect(const Request &request);
		int			is_directory(std::string req_path);
		void		create_directory_listing(std::string path, std::string req_path);
		std::string	create_html(std::string source, std::string path);
		std::string	get_index_page(void);
		std::string	intToString(int num);

		/* CGI utils */
		void		process_cgi_response(const Request& request);
		std::string	extract_cgi_extension(const std::string& req_path);

		/* Setters */
		void		setFullResponse();
		void		setPath(std::string new_path);
		void		setContentType(std::string path);
		void		set_allow_methods(bool flag);
		void		setHeaders(const Request &request);
		void		setCGIStatus(int flag);

		/* Accessors */
		int									getStatusCode() const;
		std::string							getStatusText() const;
		std::string							getHttpVersion() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getBody() const;
		std::string							getPath() const;
		std::string							getFullResponse() const;
		void								getDate();
		LocationConfig*						getLocation() const;
		int									getCGIStatus() const;
		CGIHandler*							getCGIHandler() const;
};