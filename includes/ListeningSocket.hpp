/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:53 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/08 22:08:28 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <exception>
# include <netinet/in.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/select.h>
# include <sys/time.h>
# include <sys/types.h>
# include <stdio.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <csignal>

class ListeningSocket
{
	private:
		int					server_socket;
		struct sockaddr_in	address;
		fd_set				current_sockets;
		// static ListeningSocket* instance;
	public:

		ListeningSocket();
		ListeningSocket(int domain, int type, int protocol, int port, u_long interface, int backlog);
		ListeningSocket( ListeningSocket const & src );
		~ListeningSocket();

		ListeningSocket &		operator=( ListeningSocket const & rhs );

		class SocketCreationFailure : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		void	accept_connections(void);
		int		accept_new_connections(int socket);
		void	handle_connection(int i);
		// static void	signal_handler(int signum);
		// void 	cleanup();

};

