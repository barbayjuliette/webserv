/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:06:01 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/12 19:22:21 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <map>

enum connection_type {
	KEEP_ALIVE,
	CLOSE
}

class Request
{
	private:
		std::string							_raw;
		std::string							_path;
		std::string							_http_version;
		std::string							_method; // GET POST DELETE
		// int									_port;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		Request();

	public:
		Request(std::string request);
		Request( Request const & src );
		~Request();

		Request &							operator=( Request const & rhs );
		std::string							getRaw();
		std::string							getPath();
		std::string							getHttpVersion();
		std::string							getMethod();
		std::string							getBody();
		std::map<std::string, std::string>	getHeaders();
};