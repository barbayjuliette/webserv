/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:44 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 15:48:05 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserver.hpp"
#include "webserv.hpp"

Webserver* Webserver::_instance = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

void	Webserver::initialize(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	// Address
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(interface);
	
	// Create socket
	_server_socket = socket(domain, type, protocol);
	check(_server_socket);

	// Make it non-blocking
	check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));

	// Options to be able to reuse address.
	int	yes = 1;
	check(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)));

	// Bind the socket to an address and a port
	check(bind(_server_socket, (struct sockaddr*)&_address, sizeof(_address)));

	// Listen: wait for the client to make a connection
	check(listen(_server_socket, backlog) < 0);
}

Webserver::Webserver()
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	_instance = this;
	
	initialize(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 1024);
}

Webserver::Webserver(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
	_instance = this;

	initialize(domain, type, protocol, port, interface, backlog);
}

Webserver::Webserver( const Webserver & src ):
_server_socket(src._server_socket),
_address(src._address),
_clients(src._clients)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Webserver::~Webserver()
{
	close(_server_socket);
	FD_CLR(_server_socket, &read_sockets);
	FD_ZERO(&read_sockets);
	FD_CLR(_server_socket, &write_sockets);
	FD_ZERO(&write_sockets);

	std::map<int, Client*>::iterator	it;
	
	for (it = _clients.begin() ; it != _clients.end(); it++)
	{
		delete (it->second);
	}
	_clients.clear();
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Webserver &				Webserver::operator=( Webserver const & rhs )
{
	if ( this != &rhs )
	{
		this->_server_socket = rhs._server_socket;
		this->_address = rhs._address;
		this->_instance = rhs._instance;
		this->_clients = rhs._clients;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

int	Webserver::accept_new_connections(void)
{
	socklen_t addrlen = sizeof(_address);

	int	new_socket = accept(_server_socket, (struct sockaddr*)&_address, &addrlen);
	check(new_socket);
	check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));
	_clients[new_socket] = new Client(new_socket);
	return (new_socket);
}

void	Webserver::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno);
		exit(1);
	}
}

void Webserver::signal_handler(int signum)
{
	if (_instance)
	{
		delete (_instance);
	}
    std::cout << "\nSignal received, webserver closed. Bye bye!" << std::endl;
    exit(signum);
}

void	Webserver::run(void)
{
	fd_set	read_sockets_copy;
	fd_set	write_sockets_copy;

	int		client_socket;
	int		max_socket = _server_socket;

	FD_ZERO(&read_sockets);
	FD_ZERO(&write_sockets);
	FD_SET(_server_socket, &read_sockets);

	while (true)
	{
		read_sockets_copy = read_sockets;
		write_sockets_copy = write_sockets;
		
		if (select(max_socket + 1, &read_sockets_copy, &write_sockets_copy, NULL, NULL) < 0)
		{
			std::cerr << strerror(errno);
			exit(1);
		}
		int	i = 0;
		while (i <= max_socket)
		{
			if (FD_ISSET(i, &read_sockets_copy))
			{
				if (i == _server_socket) // New connection
				{
					client_socket = accept_new_connections();
					FD_SET(client_socket, &read_sockets);
					if (client_socket > max_socket)
						max_socket = client_socket;
				}
				else
				{
					handle_read_connection(i);
				}
			}
			if (FD_ISSET(i, &write_sockets_copy))
			{
				handle_write_connection(i);
			}
			i++;
		}
	}
}

void	Webserver::create_response(Request &request, int client_socket)
{
	Response	*_response = new Response(request);
	getClient(client_socket)->setResponse(*_response);
	FD_CLR(client_socket, &read_sockets);
	FD_SET(client_socket, &write_sockets);
	// Delete request
	getClient(client_socket)->setRequest(NULL);
}

void	Webserver::handle_read_connection(int client_socket)
{
	char	buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int		bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			std::cerr << strerror(errno) << std::endl;
		else if (DEBUG)
			std::cout << "Client closed the connection\n";
		close(client_socket);
		FD_CLR(client_socket, &read_sockets);
		FD_CLR(client_socket, &write_sockets);
		_clients.erase(_clients.find(client_socket));
	}
	else // valid bytes read
	{
		// If not existing request -> create new request
		if (!_clients[client_socket]->getRequest())
		{
			Request*	new_request = new Request(buffer, bytes_read);
			getClient(client_socket)->setRequest(new_request);
			if (new_request->getReqComplete() == true)
				create_response(*new_request, client_socket);
			return ;
		}

		/* If existing request -> check if header is complete
			-> If incomplete, handle header
				-> Check again if header complete */
		if (_clients[client_socket]->getRequest()->getHeaderLength() == -1)
		{
			_clients[client_socket]->getRequest()->handle_incomplete_header(bytes_read, buffer);
			if (_clients[client_socket]->getRequest()->getReqComplete()) // If request complete, create response
				create_response(*_clients[client_socket]->getRequest(), client_socket);
		}
		// else
		// {
		// 	// if chunked -> process chunk -> create response
		// 	handle_chunk(client_socket);
		// 	// if not chunked -> process body
		// }
	}
}

void		Webserver::handle_write_connection(int client_socket)
{
	Client			*client = getClient(client_socket);
	Response		*response = client->getResponse();
	unsigned int	bytes_sent;

	if (!response)
		return ;

	bytes_sent = send(client->getSocket(), response->getFullResponse().c_str(), response->getFullResponse().size(), 0);
	if (bytes_sent == response->getFullResponse().size())
	{
		if (DEBUG)
		{
			std::cout << GREEN << "---- Response sent to client ----\n" << RESET;
			std::cout << response->getFullResponse() << std::endl;
			std::cout << GREEN << "End of response\n" << RESET;
		}
		if ((response->getHeaders())["Connection"] == "keep-alive")
		{
			FD_CLR(client_socket, &write_sockets);
			FD_SET(client_socket, &read_sockets);
			client->reset();
		}
		else
		{
			FD_CLR(client_socket, &write_sockets);
			close(client_socket);
			_clients.erase(_clients.find(client_socket));
		}
	}
	else
		std::cerr << RED << "Error sending response to client " << client->getSocket() << std::endl << RESET;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Webserver::getServerSocket()
{
	return (_server_socket);
}

struct sockaddr_in	Webserver::getAddress()
{
	return (_address);
}

Webserver* Webserver::getInstance()
{
	return (_instance);
}

std::map<int, Client*>		Webserver::getClients()
{
	return (_clients);
}

Client*		Webserver::getClient(int socket)
{
	return (_clients[socket]);
}

/* ************************************************************************** */