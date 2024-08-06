/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 14:56:13 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/05 22:03:09 by jbarbay          ###   ########.fr       */
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

class CGIHandler
{
	private:
		std::string		_result;
		std::string		_content_type;
		std::string		_html;
		std::string		_headers;

	public:
		CGIHandler();
		CGIHandler( CGIHandler const & src );
		~CGIHandler();
		CGIHandler &		operator=( CGIHandler const & rhs );
		std::string			intToString(int num);

		std::string	getResult();
		std::string	getHtml();
		std::string	getContentType();
		void		setContentType();
		void		setHtml();
		void		setHeaders();
		void		setResult(std::string result);

};