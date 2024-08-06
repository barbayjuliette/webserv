/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIPost.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 21:15:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/06 18:09:49 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "CGIHandler.hpp"

class CGIPost : public CGIHandler 
{

	public:
		CGIPost();
		CGIPost(Request const & request);
		CGIPost(CGIPost const & src);
		~CGIPost();
		CGIPost &		operator=( CGIPost const & rhs );

		void			execute_cgi(int pipe_fd[], int pipe_data[], Request const & request);
		void			process_result_cgi(int pid, int pipe_fd[], int pipe_data[], Request const & request);
};