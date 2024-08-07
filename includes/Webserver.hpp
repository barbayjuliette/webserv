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

# include "webserv.hpp"
# include "Client.hpp"
# include "ConfigFile.hpp"
# include "Cluster.hpp"

# define BUFFER_SIZE 50000

class Webserver
{
	protected:
		int							_server_socket;
		std::string					_host;
		int							_port;
		std::vector<std::string>	_server_name;
		struct sockaddr_in*			_address;
		ServerConfig*				_config;
		std::map<int, Client*>		_clients;

	public:
		/* Constructors */
		Webserver();
		Webserver(ServerConfig *config);
		Webserver( Webserver const & src );

		/* Destructor */
		~Webserver();

		/* Operator overload */
		Webserver &		operator=( Webserver const & rhs );

		/* Init sockets */
		void			initServerSocket(struct addrinfo *addr, int backlog);
		void			setAddress(struct sockaddr_in* addr);

		/* Connections */
		void			accept_new_connections(void);
		void			handle_connections(int client_socket, uint32_t event_type);
		void			handle_read_connection(int client_socket);
		void			handle_write_connection(int client_socket);
		void			create_response(Request *request, Client *client);
		void			removeClient(int client_socket);

		/* Utils */
		void			check(int num);
		void			printServerNames(void);

		// Accessors
		int							getServerSocket();
		int							getPort();
		std::vector<std::string>	getServerName();
		struct sockaddr_in*			getAddress();
		std::map<int, Client*>		getClients();
		Client*						getClient(int socket);
		ServerConfig*				getConfig();
};

