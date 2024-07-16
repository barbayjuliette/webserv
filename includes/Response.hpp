/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:05:36 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/15 22:39:15 by jbarbay          ###   ########.fr       */
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

class Response
{
	private:
		int									_status_code;
		std::string							_status_text;
		std::string							_http_version;
		std::string							_body;
		std::string							_full_response;
		std::map<std::string, std::string>	_headers;
		Response();

	public:
		Response(Request &request);
		Response( Response const & src );
		~Response();

		Response &		operator=( Response const & rhs );

		std::string	get_error_page(int num);
		void		respond_get_request(const Request &request);
		void		respond_post_request(const Request &request);
		void		respond_delete_request(const Request &request);
		void		respond_wrong_request(const Request &request);
		std::string	intToString(int num);
		void		getDate();


		int			getStatusCode() const;
		std::string	getStatusText() const;
		std::string	getHttpVersion() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string	getBody() const;
		std::string	getFullResponse() const;
		void		setFullResponse();
		void		setContentType(Request &request);

};