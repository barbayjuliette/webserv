/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:05:36 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/06 19:43:35 by jbarbay          ###   ########.fr       */
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
# include "CGIGet.hpp"
# include "CGIPost.hpp"

class CGIHandler;

class Response
{
	private:
		int									_status_code;
		std::string							_status_text;
		std::string							_http_version;
		std::string							_body;
		std::string							_full_response;
		std::string							_path;
		std::map<std::string, std::string>	_headers;
		ServerConfig*						_config;
		LocationConfig*						_location;
		Response();

	public:
		Response(Request &request, ServerConfig *conf);
		Response( Response const & src );
		~Response();
		Response &		operator=( Response const & rhs );

		std::string	get_error_page(int num);
		void		respond_get_request(const Request &request);
		void		respond_post_request(const Request &request);
		void		respond_delete_request(void);
		std::string	intToString(int num);
		void		getDate();
		void		set_error(int code, std::string text);
		void		set_allow_methods(bool flag);
		int			is_directory(std::string req_path);
		std::string	get_index_page(void);
		void		create_directory_listing(std::string path, std::string req_path);
		std::string	create_html(std::string source, std::string path);
		void		cgi_post_form(const Request &request);

		int									getStatusCode() const;
		std::string							getStatusText() const;
		std::string							getHttpVersion() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getBody() const;
		std::string							getPath() const;
		std::string							getFullResponse() const;
		void								setFullResponse();
		void								setPath(std::string new_path);
		void								setContentType(std::string path);
		int									method_is_allowed(std::string, std::vector<std::string> allowed);

};