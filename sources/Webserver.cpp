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

// Webserver* Webserver::_instance = NULL;
// ServerConfig*	Webserver::_config = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Webserver::Webserver()
{
	// signal(SIGINT, signal_handler);
	// signal(SIGTERM, signal_handler);
	// _instance = this;
	_port = 8080;

	// initialize(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 1024);
}

Webserver::Webserver(ServerConfig* config)
{
	// _instance = this;
	_config = config;
	_port = _config->getPort();

	initialize(_config->getAddressInfo(), 12);
}

void	Webserver::initialize(struct addrinfo *addr, int backlog)
{
	struct addrinfo *tmp;

	// Try binding to each address in the addrinfo linked list until a match is found
	for (tmp = addr; tmp != NULL; tmp = tmp->ai_next)
	{
		// Create socket
		_server_socket = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		check(_server_socket);

		// Make it non-blocking
		check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));

		// Options to be able to reuse address.
		int	yes = 1;
		check(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)));

		// Bind the socket to an address and a port
		if (bind(_server_socket, tmp->ai_addr, tmp->ai_addrlen) < 0)
			close(_server_socket);
		else
			break ;
	}
	if (!tmp) // if no bind attempt is successful
	{
		std::cerr << strerror(errno);
		exit(1);
	}
	// Listen: wait for the client to make a connection
	check(listen(_server_socket, backlog) < 0);
	setAddress(tmp);
}

void	Webserver::setAddress(struct addrinfo *addr)
{
	if (addr->ai_family == AF_INET)
	{
		struct sockaddr_in* host = (struct sockaddr_in*)addr->ai_addr;
		_address.sin_family = host->sin_family;
		_address.sin_port = host->sin_port;
		_address.sin_addr = host->sin_addr;
	}
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

	// delete (_config);
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
		// this->_instance = rhs._instance;
		this->_clients = rhs._clients;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Webserver::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno);
		exit(1);
	}
}

/* Add new client to the clients map
- Create pollfd struct for the new client socket and add it to _poll_fds to be monitored */
void	Webserver::accept_new_connections(void)
{
	int	client_socket = accept(_server_socket, NULL, NULL);
	check(client_socket);
	check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));
	_clients[client_socket] = new Client(client_socket);

	struct pollfd	client_pfd;

	client_pfd.fd = client_socket;
	client_pfd.events = POLLIN | POLLOUT;
	_poll_fds.push_back(client_pfd);
}

// void	Webserver::update_client_event(int client_socket, short event)
// {
// }

/* Iterate over all fds to check if fd is readable/writable
- If POLLIN flag is set for the server socket: there is a new client connection incoming
- If POLLIN is set for other sockets: there is data to read from that client socket
- If POLLOUT is set: the socket is ready to send data */
void	Webserver::handle_connections(std::vector<struct pollfd> poll_fds)
{
	_poll_fds = poll_fds;

	for (size_t i = 0; i < _poll_fds.size(); i++)
	{
		if (_poll_fds[i].revents & POLLIN)
		{
			accept_new_connections(size);
		}
		else
		{
			handle_read_connection(_poll_fds[i].fd);
		}

		if (_poll_fds[i].revents & POLLOUT)
		{
			handle_write_connection(_poll_fds[i].fd);
		}
	}
}

void	Webserver::handle_read_connection(int client_socket)
{
	char	buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int		bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);

	if (bytes_read < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		close(client_socket);
		FD_CLR(client_socket, &read_sockets);
		FD_CLR(client_socket, &write_sockets);
		_clients.erase(_clients.find(client_socket));
	}
	else if (bytes_read == 0)
	{
		if (DEBUG)
			std::cout << "Client closed the connection\n";
		close(client_socket);
		FD_CLR(client_socket, &read_sockets);
		FD_CLR(client_socket, &write_sockets);
		_clients.erase(_clients.find(client_socket));
	}
	else
	{
		Request*	request = new Request(buffer);
		getClient(client_socket)->setRequest(*request);


		if (DEBUG)
		{
			std::cout << RED << "---- Request received from client " << client_socket << " ----\n" << RESET;
			std::cout << request->getRaw();
			std::cout << RED << "End of request\n\n" << RESET;
		}
		Response	*response = new Response(*request);
		getClient(client_socket)->setResponse(*response);
		FD_CLR(client_socket, &read_sockets);
		FD_SET(client_socket, &write_sockets);
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

// void Webserver::signal_handler(int signum)
// {
// 	if (_instance)
// 	{
// 		delete (_instance);
// 	}
//     std::cout << "\nSignal received, webserver closed. Bye bye!" << std::endl;
//     exit(signum);
// }

// int	Webserver::accept_new_connections(void)
// {
// 	socklen_t addrlen = sizeof(_address);

// 	int	new_socket = accept(_server_socket, (struct sockaddr*)&_address, &addrlen);
// 	check(new_socket);
// 	check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));
// 	_clients[new_socket] = new Client(new_socket);
// 	return (new_socket);
// }

// void	Webserver::run(void)
// {
// 	fd_set	read_sockets_copy;
// 	fd_set	write_sockets_copy;

// 	int		client_socket;
// 	int		max_socket = _server_socket;

// 	FD_ZERO(&read_sockets);
// 	FD_ZERO(&write_sockets);
// 	FD_SET(_server_socket, &read_sockets);

// 	while (true)
// 	{
// 		read_sockets_copy = read_sockets;
// 		write_sockets_copy = write_sockets;
		
// 		if (select(max_socket + 1, &read_sockets_copy, &write_sockets_copy, NULL, NULL) < 0)
// 		{
// 			std::cerr << strerror(errno);
// 			exit(1);
// 		}
// 		int	i = 0;
// 		while (i <= max_socket)
// 		{
// 			if (FD_ISSET(i, &read_sockets_copy))
// 			{
// 				if (i == _server_socket) // New connection
// 				{
// 					client_socket = accept_new_connections();
// 					FD_SET(client_socket, &read_sockets);
// 					if (client_socket > max_socket)
// 						max_socket = client_socket;
// 				}
// 				else
// 				{
// 					handle_read_connection(i);
// 				}
// 			}
// 			if (FD_ISSET(i, &write_sockets_copy))
// 			{
// 				handle_write_connection(i);
// 			}
// 			i++;
// 		}
// 	}
// }

// void	Webserver::handle_read_connection(int client_socket)
// {
// 	char	buffer[BUFFER_SIZE];
// 	memset(buffer, 0, sizeof(buffer));
// 	int		bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);

// 	if (bytes_read < 0)
// 	{
// 		std::cerr << strerror(errno) << std::endl;
// 		close(client_socket);
// 		FD_CLR(client_socket, &read_sockets);
// 		FD_CLR(client_socket, &write_sockets);
// 		_clients.erase(_clients.find(client_socket));
// 	}
// 	else if (bytes_read == 0)
// 	{
// 		if (DEBUG)
// 			std::cout << "Client closed the connection\n";
// 		close(client_socket);
// 		FD_CLR(client_socket, &read_sockets);
// 		FD_CLR(client_socket, &write_sockets);
// 		_clients.erase(_clients.find(client_socket));
// 	}
// 	else
// 	{
// 		Request*	request = new Request(buffer);
// 		getClient(client_socket)->setRequest(*request);


// 		if (DEBUG)
// 		{
// 			std::cout << RED << "---- Request received from client " << client_socket << " ----\n" << RESET;
// 			std::cout << request->getRaw();
// 			std::cout << RED << "End of request\n\n" << RESET;
// 		}
// 		Response	*response = new Response(*request);
// 		getClient(client_socket)->setResponse(*response);
// 		FD_CLR(client_socket, &read_sockets);
// 		FD_SET(client_socket, &write_sockets);
// 	}

// }

// void		Webserver::handle_write_connection(int client_socket)
// {
// 	Client			*client = getClient(client_socket);
// 	Response		*response = client->getResponse();
// 	unsigned int	bytes_sent;

// 	if (!response)
// 		return ;

// 	bytes_sent = send(client->getSocket(), response->getFullResponse().c_str(), response->getFullResponse().size(), 0);
// 	if (bytes_sent == response->getFullResponse().size())
// 	{
// 		if (DEBUG)
// 		{
// 			std::cout << GREEN << "---- Response sent to client ----\n" << RESET;
// 			std::cout << response->getFullResponse() << std::endl;
// 			std::cout << GREEN << "End of response\n" << RESET;
// 		}
// 		if ((response->getHeaders())["Connection"] == "keep-alive")
// 		{
// 			FD_CLR(client_socket, &write_sockets);
// 			FD_SET(client_socket, &read_sockets);
// 			client->reset();
// 		}
// 		else
// 		{
// 			FD_CLR(client_socket, &write_sockets);
// 			close(client_socket);
// 			_clients.erase(_clients.find(client_socket));
// 		}
// 	}
// 	else
// 		std::cerr << RED << "Error sending response to client " << client->getSocket() << std::endl << RESET;
// }

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

// Webserver* Webserver::getInstance()
// {
// 	return (_instance);
// }

std::map<int, Client*>		Webserver::getClients()
{
	return (_clients);
}

Client*		Webserver::getClient(int socket)
{
	return (_clients[socket]);
}

/* ************************************************************************** */