/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:44 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/08 22:51:02 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ListeningSocket.hpp"

ListeningSocket* ListeningSocket::instance = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ListeningSocket::ListeningSocket()
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	// Address
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// Create socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		perror("Error creating socket: ");
		exit(1);
	}
	int	tr = 1;
	if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}
	// Bind the socket to an address and a port
	if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("Error bind: ");
		exit(1);
	}
	// Listen: wait for the client to make a connection
	if (listen(server_socket, 1024) < 0)
	{
		perror("Error listen: ");
		exit(1);
	}
	instance = this;
	this->accept_connections();

}

ListeningSocket::ListeningSocket(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	// Address
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);
	
	// Create socket
	server_socket = socket(domain, type, protocol);
	if (server_socket == -1)
	{
		perror("Error creating socket: ");
		exit(1);
	}
	int	tr = 1;
	if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}
	// Bind the socket to an address and a port
	if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("Error bind: ");
		exit(1);
	}
	// Listen: wait for the client to make a connection
	if (listen(server_socket, backlog) < 0)
	{
		perror("Error listen: ");
		exit(1);
	}
	instance = this;
	this->accept_connections();
}

ListeningSocket::ListeningSocket( const ListeningSocket & src )
{
	(void)src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ListeningSocket::~ListeningSocket()
{
	close(server_socket);
	FD_CLR(server_socket, &current_sockets);
	std::cout << "Socket closed." << std::endl;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ListeningSocket &				ListeningSocket::operator=( ListeningSocket const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	(void)rhs;
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

const char *ListeningSocket::SocketCreationFailure::what() const throw()
{
	return ("Error creating socket");
}

int	ListeningSocket::accept_new_connections(int socket)
{
	socklen_t addrlen = sizeof(address);

	int	new_socket = accept(socket, (struct sockaddr*)&address, &addrlen);
	if (new_socket < 0)
	{
		perror("Error accept: ");
		exit(1);
	}
	return (new_socket);
}

void	ListeningSocket::handle_connection(int client_socket)
{
	char	buffer[5000];
	int		bytes_read;

	if ((bytes_read = read(client_socket, buffer, 5000)) > 0)
		std::cout << buffer << std::endl;
	write(client_socket, "HELLO", 5);
	fflush(stdout);
}

void ListeningSocket::signal_handler(int signum)
{
    std::cout << "Signal received, webserver closed. Bye bye!" << std::endl;
	
	if (instance)
	{
		close (instance->server_socket);
		FD_CLR(instance->server_socket, &(instance->current_sockets));
	}
    exit(signum);
}

void	ListeningSocket::accept_connections(void)
{
	fd_set	ready_sockets;
	int		client_socket;

	FD_ZERO(&current_sockets);
	FD_SET(server_socket, &current_sockets);

	while (true)
	{
		ready_sockets = current_sockets;
		// Only for reading now, change the first NULL for also writing
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			perror("Error select: ");
			exit(1);
		}
		int	i = 0;
		while (i < FD_SETSIZE)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == server_socket) // New connection
				{
					client_socket = accept_new_connections(server_socket);
					FD_SET(client_socket, &current_sockets);
				}
				else
				{
					handle_connection(i);
					FD_CLR(i, &current_sockets);
				}
			}
			i++;
		}
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */