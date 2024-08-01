/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 12:59:40 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/12 19:26:28 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "webserv.hpp"
# include "Response.hpp"
# include "Request.hpp"


class Client
{
	private:
		int			_socket;
		Request		*_request;
		Response	*_response;
		// bool		keepAlive;
		Client();

	public:
		Client(int socket);
		Client( Client const & src );
		~Client();

		Client &		operator=( Client const & rhs );
		Request*		getRequest();
		Response*		getResponse();
		int				getSocket();
		void			reset();
		void			setRequest(Request* request);
		void			setResponse(Response* response);
};