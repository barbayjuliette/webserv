/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIPost.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 21:15:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/29 13:35:27 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "CGIHandler.hpp"

class CGIPost : public CGIHandler
{
	private:
		CGIPost();

	public:
		CGIPost(const Request& request, LocationConfig *location, std::string cgi_ext);
		CGIPost(CGIPost const & src);
		~CGIPost();
		CGIPost &		operator=( CGIPost const & rhs );

		void		write_cgi(int cgi_status);
		void		read_cgi_request(int cgi_status);
};