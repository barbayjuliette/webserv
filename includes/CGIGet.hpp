/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIGet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 20:33:16 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/06 19:35:16 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "CGIHandler.hpp"

class CGIGet : public CGIHandler
{
	private:
		std::string		_cgi_exec;

		CGIGet();

	public:
		CGIGet(Request const & request, LocationConfig* location, std::string ext);
		CGIGet( CGIGet const & src );
		~CGIGet();
		CGIGet &		operator=( CGIGet const & rhs );

		void			execute_cgi(int pipe_fd[], Request const & request);
		void			process_result_cgi(int pid, int pipe_fd[]);
		std::string		get_cgi_location(std::string location);
};