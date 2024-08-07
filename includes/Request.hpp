/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:06:01 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/02 19:44:27 by jbarbay          ###   ########.fr       */
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
	POST_MISSING_BODY,
	INVALID_SIZE,
	INVALID_EMPTY_REQ,
	BODY_TOO_LONG,
	NO_HOST,
	INVALID_PORT,
};

class Request
{
	private:
		Webserver							*_server;
		std::vector<unsigned char>			_raw;
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
		std::string							_host;
		int									_port;
		std::map<std::string, std::string>	_headers;
		std::vector<unsigned char>			_body;
		error_type							_error;
		ServerConfig*						_config;
		std::map<std::string, std::string>	_formData;
		std::map<std::string, std::string> 	_bodyMap;
		std::map<std::string, std::string>	_fileMap;

		Request();

		// Member functions
		int 		parseRequest(std::string header);
		void 		parseHeader(std::string header);
		void 		parsePort(std::string header);
		void		parseContentType();

		void 		checkMethod();
		void 		checkPath();
		std::string extractHeader();

		void		initBody();
		void 		initRequest();

		// Helper function
		size_t		convert_sizet(std::string str);
		bool 		is_header_complete();
		void		boundary_found();
		void		copyRawRequest(char *buf, int bytes_read);
		bool 		findSequence(const std::vector <unsigned char> &vec, \
			const std::vector<unsigned char>& seq);

		// Error handling
		void 			printError(std::string error_msg);
		static void		printMap(std::map<std::string, std::string> map);
		void 			print_vector(std::vector<unsigned char> vec);

		// Debug
		void 			print_variables() const;

	public:
		Request(char *full_request, int bytes_read);
		Request( Request const & src );
		~Request();

		Request &							operator=( Request const & rhs );
		std::vector<unsigned char>			getRaw() const;
		std::string							getPath() const;
		std::string							getHttpVersion() const;
		std::string							getMethod() const;
		std::vector<unsigned char>			getBody() const;
		std::string							getHost() const;
		int									getPort() const;
		std::map<std::string, std::string>	getHeaders() const;
		ssize_t								getHeaderLength() const;
		bool								getReqComplete() const;
		error_type							getError() const;
		Webserver*							getServer();
		
		void								setBodyMaxLength(size_t len);
		void								setServer(Webserver* server);

		void		handle_incomplete_header(int bytes_read, char *buffer);
		bool		handle_chunk(char *buffer, int bytes_read);
		static void	parseHostPort(char *buffer, std::string& host, int& port);
		void 		parseBody();
		void 		checkBodyLength();
};