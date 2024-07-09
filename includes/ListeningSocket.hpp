/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:53 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/09 13:49:31 by jbarbay          ###   ########.fr       */
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
# include <errno.h>

class ListeningSocket
{
	private:
		int						_server_socket;
		struct sockaddr_in		_address;
		fd_set					_current_sockets;
		static ListeningSocket* _instance;
	public:

		ListeningSocket();
		ListeningSocket(int domain, int type, int protocol, int port, u_long interface, int backlog);
		void	initialize(int domain, int type, int protocol, int port, u_long interface, int backlog);
		ListeningSocket( ListeningSocket const & src );
		~ListeningSocket();

		ListeningSocket &		operator=( ListeningSocket const & rhs );

		class SocketCreationFailure : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		void		connect(void);
		void		check(int num);
		int			accept_new_connections(int socket);
		void		handle_read_connection(int i);
		void		handle_write_connection(int client_socket);
		static void	signal_handler(int signum);

		// Accessors
		int					getServerSocket();
		struct sockaddr_in	getAddress();
		fd_set				getCurrentSockets();
		ListeningSocket*	getInstance();
};

