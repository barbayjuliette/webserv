/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 14:56:13 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 15:38:00 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <errno.h>
# include <string.h>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <cstdio>
# include <cstdlib>
# include <fstream>
# include <iostream>
# include "webserv.hpp"
# include "Request.hpp"
# include "LocationConfig.hpp"

class Request;
class LocationConfig;

class CGIHandler
{
	private:
		std::string		_result;
		std::string		_content_type;
		std::string		_html;
		std::string		_headers;
		std::string		_full_path;
		int				_error;

	public:
		CGIHandler();
		CGIHandler(CGIHandler const & src);
		virtual ~CGIHandler();
		CGIHandler &		operator=( CGIHandler const & rhs );
		std::string			intToString(int num);

		std::string	getResult();
		std::string	getHtml();
		std::string	getContentType();
		void		setContentType();
		void		setHtml();
		void		setHeaders();
		void		setResult(std::string result);
		std::string	getFullPath();
		void		setFullPath(std::string path);
		int			getError();
		void		setError(int error);
};