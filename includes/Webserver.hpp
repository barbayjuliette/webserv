/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:53 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 15:53:20 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Client.hpp"
# include "ConfigFile.hpp"
# include "Cluster.hpp"

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <exception>
# include <netinet/in.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/select.h>
# include <stdio.h>
# include <cstring>
# include <sys/socket.h>
# include <csignal>
# include <errno.h>
# include <map>
# include <fcntl.h>

# define BUFFER_SIZE 50000

class Webserver
{
	protected:
		int						_server_socket;
		int						_port;
		struct sockaddr_in		_address;
		static Webserver* 		_instance;
		static ServerConfig*	_config;
		std::map<int, Client*>	_clients;

	public:
		fd_set	read_sockets;
		fd_set	write_sockets;
		
		Webserver();
		Webserver(ServerConfig *config);
		Webserver( Webserver const & src );
		~Webserver();

		Webserver &		operator=( Webserver const & rhs );

		void			run(void);
		void			check(int num);
		int				accept_new_connections();
		void			handle_read_connection(int i);
		void			handle_write_connection(int client_socket);
		static void		signal_handler(int signum);

		void			initialize(struct addrinfo *addr, int backlog);
		void			setAddress(struct addrinfo* addr);

		// Accessors
		int							getServerSocket();
		struct sockaddr_in			getAddress();
		Webserver*					getInstance();
		std::map<int, Client*>		getClients();
		Client*						getClient(int socket);
};

