/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:06:01 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/15 18:38:30 by jbarbay          ###   ########.fr       */
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
		std::string	_headers;
		std::string	_body;
		Request();

	public:
		Request(std::string request);
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );
		std::string		getFullRequest() const;
		std::string		getPath() const;
		std::string		getHttpVersion() const;
		std::string		getMethod() const;
		std::string		getHeaders() const;
		std::string		getBody() const;
};