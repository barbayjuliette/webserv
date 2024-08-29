/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIGet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 20:33:16 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/29 13:26:18 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "CGIHandler.hpp"

class CGIGet : public CGIHandler
{
	private:
		CGIGet();

	public:
		CGIGet(const Request& request, LocationConfig *location, std::string cgi_ext);
		CGIGet( CGIGet const & src );
		~CGIGet();
		CGIGet &		operator=( CGIGet const & rhs );

		void		write_cgi(int cgi_status);
		void		read_cgi_request(int cgi_status);
};