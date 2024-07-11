/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 12:59:40 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 14:13:53 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include "Response.hpp"
# include "Request.hpp"


class Client
{
	private:
		int			_socket;
		Request		*_request;
		Response	*_response;
		Client();

	public:
		Client(int socket);
		Client( Client const & src );
		~Client();

		Client &		operator=( Client const & rhs );
		Request&		getRequest();
		Response&		getResponse();
		void			setRequest(Request& request);
		void			setResponse(Response& response);
};