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

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Webserver::Webserver()
{
	// signal(SIGINT, signal_handler);
	// signal(SIGTERM, signal_handler);
	// _instance = this;
	_port = 8080;

	// initServerSocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 1024);
}

Webserver::Webserver(ServerConfig* config)
{
	_config = config;
	_host = _config->getHost();
	_port = _config->getPort();
	_server_name = _config->getServerName();

	// initServerSocket(_config->getAddressInfo(), 12);
}

/* Try binding to each address in the addrinfo linked list until a match is found
- Create socket and make it non-blocking
- Set socket options to be able to reuse address
- Bind the socket to an address and a port
- Listen: wait for the client to make a connection */
// void	Webserver::initServerSocket(struct addrinfo *addr, int backlog)
// {
// 	struct addrinfo *tmp;

// 	for (tmp = addr; tmp != NULL; tmp = tmp->ai_next)
// 	{
// 		_server_socket = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
// 		check(_server_socket);

// 		check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));

// 		int	yes = 1;
// 		check(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)));

// 		if (bind(_server_socket, tmp->ai_addr, tmp->ai_addrlen) < 0)
// 			close(_server_socket);
// 		else
// 			break ;
// 	}
// 	if (!tmp) // no bind attempt is successful
// 	{
// 		std::cerr << strerror(errno) << std::endl;
// 		exit(1);
// 	}

// 	check(listen(_server_socket, backlog) < 0);
// 	setAddress((struct sockaddr_in*)(tmp->ai_addr));
// }

// void	Webserver::setAddress(struct sockaddr_in *addr)
// {
// 	_address = addr;
// }

Webserver::Webserver( const Webserver & src ):
_server_socket(src._server_socket),
_address(src._address),
_clients(src._clients)
{

}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Webserver&	Webserver::operator=( Webserver const & rhs )
{
	if ( this != &rhs )
	{
		this->_server_socket = rhs._server_socket;
		this->_address = rhs._address;
		this->_config = rhs._config;
		this->_clients = rhs._clients;
	}
	return (*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Webserver::~Webserver()
{
	close(_server_socket);

	std::map<int, Client*>::iterator	it;
	for (it = _clients.begin() ; it != _clients.end(); it++)
	{
		delete (it->second);
	}
	_clients.clear();
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* Add new client to the clients map
- Create epoll_event struct for the new client socket and register it to be monitored */
void	Webserver::accept_new_connections(void)
{
	int	client_socket = accept(_server_socket, NULL, NULL);
	check(client_socket);
	check(fcntl(_server_socket, F_SETFL, O_NONBLOCK));

	struct epoll_event	ep_event;

	ep_event.data.fd = client_socket;
	ep_event.events = EPOLLIN | EPOLLOUT;
	Cluster::addToEpoll(client_socket, &ep_event);

    _clients[client_socket] = new Client(client_socket);
}

/* Process events for the server's clients
- If EPOLLIN is set for client socket: there is data to read from that client socket
- If EPOLLOUT is set: the socket is ready to send data */
void	Webserver::handle_connections(int client_socket, uint32_t event_type)
{
	if (event_type & EPOLLIN)
		handle_read_connection(client_socket);
	if (event_type & EPOLLOUT)
		handle_write_connection(client_socket);
}

void	Webserver::create_response(Request &request, int client_socket)
{
	Response	*_response = new Response(request, this->_config);
	getClient(client_socket)->setResponse(*_response);
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
		removeClient(client_socket);
	}
	else // valid bytes read
	{
		// If not existing request -> create new request
		if (!_clients[client_socket]->getRequest())
		{
			Request*	new_request = new Request(buffer, _config);
			getClient(client_socket)->setRequest(new_request);
			if (new_request->getReqComplete() == true)
				create_response(*new_request, client_socket);
			return ;
		}

		/* If existing request -> check if header is complete
			-> If incomplete, handle header
				-> Check again if header complete */
		Request*	request = _clients[client_socket]->getRequest();
		if (request->getHeaderLength() == -1)
		{
			request->handle_incomplete_header(bytes_read, buffer);
			if (request->getReqComplete()) // If request complete, create response
				create_response(*request, client_socket);
		}
		else // if chunked -> process chunk -> create response
			_clients[client_socket]->getRequest()->handle_chunk(buffer, bytes_read);
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
			client->reset();
		}
		else
		{
			removeClient(client_socket);
		}
	}
	else
		std::cerr << RED << "Error sending response to client " << client->getSocket() << std::endl << RESET;
}

void	Webserver::removeClient(int client_socket)
{
	close(client_socket);
	Cluster::removeFromEpoll(client_socket);
	delete _clients[client_socket];
	_clients.erase(client_socket);
}

void	Webserver::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}
}

void	Webserver::printServerNames(void)
{
	std::cout << CYAN << "SERVER NAME(S): " << RESET;

	for (size_t i = 0; i < _server_name.size(); i++)
	{
		std::cout << _server_name[i];
		if (i != _server_name.size() - 1)
			std::cout << ' ';
		else
			std::cout << '\n';
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Webserver::getServerSocket()
{
	return (_server_socket);
}

int	Webserver::getPort()
{
	return (_port);
}

std::vector<std::string>	Webserver::getServerName()
{
	return (_server_name);
}

struct sockaddr_in*	Webserver::getAddress()
{
	return (_address);
}

std::map<int, Client*>		Webserver::getClients()
{
	return (_clients);
}

Client*		Webserver::getClient(int socket)
{
	if (_clients.find(socket) == _clients.end())
		return (NULL);
	return (_clients[socket]);
}

/* ************************************************************************** */