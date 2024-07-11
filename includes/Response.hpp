/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:05:36 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 14:54:31 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include "Request.hpp"
# include <errno.h>
# include <string.h>

class Response
{
	private:
		int			_status_code;
		std::string	_status_text;
		std::string	_http_version;
		std::string	_headers;
		std::string	_body;
		std::string	_full_response;
		Response();

	public:
		Response(Request *request);
		Response( Response const & src );
		~Response();

		Response &		operator=( Response const & rhs );

		int			getStatusCode();
		std::string	getStatusText();
		std::string	getHttpVersion();
		std::string	getHeaders();
		std::string	getBody();
		std::string	getFullResponse();
};