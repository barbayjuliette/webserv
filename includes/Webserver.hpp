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

# define BUFFER_SIZE 50000

class Client;

class Webserver
{
	protected:
		int							_server_socket;
		std::string					_host;
		int							_port;
		std::vector<std::string>	_server_name;
		struct sockaddr_in*			_address;
		ServerConfig*				_config;
		// std::map<int, Client*>		_clients;

		Webserver(); //should not be constructed without server config

	public:
		/* Constructors */
		Webserver(ServerConfig *config);
		Webserver( Webserver const & src );

		/* Destructor */
		~Webserver();

		/* Operator overload */
		Webserver&	operator=( Webserver const & rhs );

		/* Connections */
		void		create_response(Request *request, Client *client);

		/* Utils */
		void		check(int num);
		void		printServerNames(void);
		void		printConfig(void);

		// Accessors
		int							getServerSocket();
		int							getPort();
		std::string					getHost();
		std::vector<std::string>	getServerName();
		struct sockaddr_in*			getAddress();
		// std::map<int, Client*>		getClients();
		// Client*						getClient(int socket);
		ServerConfig*				getConfig();
		int							getBodyMaxLength(void);
};

