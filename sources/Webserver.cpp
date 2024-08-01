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

Webserver::Webserver( const Webserver & src ):
_server_socket(src._server_socket),
_address(src._address),
_clients(src._clients)
{}

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

void	Webserver::create_response(Request &request, int client_socket)
{
	Response	*_response = new Response(request, this->_config);
	getClient(client_socket)->setResponse(*_response);
	// Delete request
	getClient(client_socket)->setRequest(NULL);
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

ServerConfig*	Webserver::getConfig()
{
	return (_config);
}

/* ************************************************************************** */