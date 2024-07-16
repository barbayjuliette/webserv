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
# include "webserv.hpp"

enum connection_type {
	KEEP_ALIVE,
	CLOSE
};

enum error_type {
	NO_ERR,
	INVALID, // 400
	NOT_SUPPORTED // 405
};

class Request
{
	private:

		std::string							_raw;
		std::string							_method;
		std::string							_path;
		std::string							_http_version;
		int									_port;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		error_type							_error;

		Request();

		// Member functions
		int parseRequest();
		void parseHeader();
		void parsePort();
		void parseBody();

		void checkMethod();
		void checkPath();
		std::string extractHeader();
		void printHeaders(const std::map<std::string, std::string>& headers);

	public:
		Request(std::string request);
		Request( Request const & src );
		~Request();

		Request &							operator=( Request const & rhs );
		std::string							getRaw() const;
		std::string							getPath() const;
		std::string							getHttpVersion() const;
		std::string							getMethod() const;
		std::string							getBody() const;
		std::map<std::string, std::string>	getHeaders() const;
};