/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:06:01 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 14:20:18 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>

class Request
{
	private:
		std::string	_full_request;
		std::string	_path;
		std::string	_http_version;
		std::string	_method; // GET POST DELETE
		int			_port;
		std::map<std::string, std::string>	_headers;
		std::string	_body;
		Request();

	public:
		Request(std::string request);
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );
		std::string		getFullRequest();
		std::string		getPath();
		std::string		getHttpVersion();
		std::string		getMethod();
		std::string		getHeaders();
		std::string		getBody();
};