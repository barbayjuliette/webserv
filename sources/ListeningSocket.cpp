/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:44 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 15:06:34 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"

ListeningSocket* ListeningSocket::_instance = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

void	ListeningSocket::initialize(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	// Address
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(interface);
	
	// Create socket
	_server_socket = socket(domain, type, protocol);
	check(_server_socket);

	// Options to be able to reuse address.
	int	tr = 1;
	check(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)));

	// Bind the socket to an address and a port
	check(bind(_server_socket, (struct sockaddr*)&_address, sizeof(_address)));

	// Listen: wait for the client to make a connection
	check(listen(_server_socket, backlog) < 0);
}

ListeningSocket::ListeningSocket()
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	_instance = this;
	
	initialize(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 1024);
	
	// this->connect();

}

ListeningSocket::ListeningSocket(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	_instance = this;

	initialize(domain, type, protocol, port, interface, backlog);

	// this->connect();
}

ListeningSocket::ListeningSocket( const ListeningSocket & src ):
_server_socket(src._server_socket),
_address(src._address),
_current_sockets(src._current_sockets)
{
	(void)src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ListeningSocket::~ListeningSocket()
{
	close(_server_socket);
	FD_CLR(_server_socket, &_current_sockets);
	FD_ZERO(&_current_sockets);

	std::map<int, Client*>::iterator	it;
	
	for (it = _clients.begin() ; it != _clients.end(); it++)
	{
		delete it->second;
	}
	_clients.clear();

	std::cout << "Socket closed." << std::endl;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ListeningSocket &				ListeningSocket::operator=( ListeningSocket const & rhs )
{
	if ( this != &rhs )
	{
		this->_instance = rhs._instance;
		this->_server_socket = rhs._server_socket;
		this->_current_sockets = rhs._current_sockets;
		this->_address = rhs._address;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

int	ListeningSocket::accept_new_connections(void)
{
	socklen_t addrlen = sizeof(_address);

	int	new_socket = accept(_server_socket, (struct sockaddr*)&_address, &addrlen);
	if (new_socket < 0)
	{
		std::cerr << strerror(errno);
		exit(1);
	}
	_clients[new_socket] = new Client(new_socket);
	return (new_socket);
}

void	ListeningSocket::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno);
		exit(1);
	}
}

void	ListeningSocket::handle_read_connection(int client_socket)
{
	// // Code for testing, will all be changed later
	(void)client_socket;
	// char	buffer[5000];
	// int		bytes_read;

	// if ((bytes_read = read(client_socket, buffer, 5000)) > 0)
	// 	std::cout << buffer << std::endl;
	// write(client_socket, "HELLO", 5);
	// // fflush(stdout);
}

void	ListeningSocket::handle_write_connection(int client_socket)
{
	(void)client_socket;
}

void ListeningSocket::signal_handler(int signum)
{
	if (_instance)
	{
		// close(_instance->_server_socket);
		// FD_CLR(_instance->_server_socket, &(_instance->_current_sockets));
		delete (_instance);
	}
    std::cout << "\nSignal received, webserver closed. Bye bye!" << std::endl;
    exit(signum);
}

void	ListeningSocket::run(void)
{
	fd_set	read_sockets, write_sockets;
	int		client_socket;
	// int		max_socket = _server_socket;

	FD_ZERO(&_current_sockets);
	FD_SET(_server_socket, &_current_sockets);

	while (true)
	{
		read_sockets = _current_sockets;
		write_sockets = _current_sockets;
		
		if (select(FD_SETSIZE, &read_sockets, &write_sockets, NULL, NULL) < 0)
		{
			std::cerr << strerror(errno);
			exit(1);
		}
		int	i = 0;
		while (i < FD_SETSIZE)
		{
			if (FD_ISSET(i, &read_sockets))
			{
				if (i == _server_socket) // New connection
				{
					client_socket = accept_new_connections();
					FD_SET(client_socket, &_current_sockets);
					// if (client_socket > max_socket)
					// 	max_socket = client_socket;
				}
				else
				{
					handle_read_connection(i);
					FD_CLR(i, &_current_sockets);
				}
			}
			if (FD_ISSET(i, &write_sockets))
				handle_write_connection(i);
			i++;
		}
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	ListeningSocket::getServerSocket()
{
	return (_server_socket);
}

struct sockaddr_in	ListeningSocket::getAddress()
{
	return (_address);
}

fd_set	ListeningSocket::getCurrentSockets()
{
	return (_current_sockets);
}

ListeningSocket* ListeningSocket::getInstance()
{
	return (_instance);
}

std::map<int, Client*>		ListeningSocket::getClients()
{
	return (_clients);
}

Client*		ListeningSocket::getClient(int socket)
{
	return (_clients[socket]);
}


/* ************************************************************************** */