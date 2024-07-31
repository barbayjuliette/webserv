/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:06:01 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/16 16:35:33 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <map>
# include <cstdlib>
# include <algorithm>
# include "webserv.hpp"
# include "ServerConfig.hpp"

#define DEFAULT_BODY_MAX 300000

enum connection_type {
	KEEP_ALIVE,
	CLOSE
};

enum error_type {
	NO_ERR,
	INVALID_METHOD,
	INVALID, // 400
	NOT_SUPPORTED, // 405
	CHUNK_AND_LENGTH,
	REQ_TOO_LONG,
	POST_MISSING_BODY,
	INVALID_SIZE,
	INVALID_EMPTY_REQ,
	CURR_LENGTH_TOO_LONG,
	NO_HOST,
	INVALID_PORT,
};

class Request
{
	private:
		std::string							_raw;
		ssize_t								_header_length;
		bool								_req_complete;
		ssize_t								_body_max_length;
		ssize_t								_content_length;
		bool								_is_chunked;
		std::string							_method;
		std::string							_path;
		std::string							_http_version;
		std::string							_content_type;
		std::string							_boundary;
		int									_port;
		ssize_t								_curr_length;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		error_type							_error;
		ServerConfig*						_config;
		std::map<std::string, std::string>	_formData;

		Request();

		// Member functions
		int 		parseRequest();
		void 		parseHeader();
		void 		parsePort();
		void		parseContentType();
		// void 		parseBody();

		void 		checkMethod();
		void 		checkPath();
		std::string extractHeader();

		void		initBody();
		void 		initRequest();

		// Helper function
		size_t		convert_sizet(std::string str);
		bool 		is_header_complete();
		void		boundary_found();

		// Error handling
		void 		printError(std::string error_msg);
		void 		printHeaders(const std::map<std::string, std::string>& headers);

		// Debug
		void 		print_variables() const;

	public:
		Request(char *full_request);
		Request( Request const & src );
		~Request();

		Request &							operator=( Request const & rhs );
		std::string							getRaw() const;
		std::string							getPath() const;
		std::string							getHttpVersion() const;
		std::string							getMethod() const;
		std::string							getBody() const;
		std::string							getHost() const;
		int									getPort() const;
		std::map<std::string, std::string>	getHeaders() const;
		ssize_t								getHeaderLength() const;
		bool								getReqComplete() const;

		void		handle_incomplete_header(int bytes_read, char *buffer);
		bool		handle_chunk(char *buffer, int bytes_read);
		static void	parseHostPort(char *buffer, std::string& host, int& port);
};