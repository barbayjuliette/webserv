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

Webserver::Webserver() {}

Webserver::Webserver(ServerConfig* config)
{
	_config = config;
	_host = _config->getHost();
	_port = _config->getPort();
	_server_name = _config->getServerName();
}

Webserver::Webserver( const Webserver & src ):
_server_socket(src._server_socket),
_host(src._host),
_port(src._port),
_server_name(src._server_name),
_address(src._address),
_config(src._config),
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
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_server_name = rhs._server_name;
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
	// close(_server_socket);

	// std::map<int, Client*>::iterator	it;
	// for (it = _clients.begin() ; it != _clients.end(); it++)
	// {
	// 	delete (it->second);
	// }
	// _clients.clear();
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Webserver::create_response(Request *request, Client *client)
{
	Response	*_response = new Response(*request, this->_config);
	client->setResponse(_response);
	client->deleteRequest();
}

void	Webserver::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}
}

void	Webserver::printConfig(void)
{
	_config->printConfig();

	std::map<std::string, LocationConfig*>	locations = _config->getLocations();
	std::map<std::string, LocationConfig*>::iterator	it;

	std::cout << CYAN << std::setw(20) << "LOCATIONS: " << RESET;

	for (it = locations.begin(); it != locations.end(); it++)
	{
		// if (it != locations.begin())
		// 	std::cout << "\t";
		std::cout << it->first << '\n';
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

void	Webserver::setReadPipes(void)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "Read pipe(): " << strerror(errno) << std::endl;
		exit(1);
	}
	_read_pipes.push_back(pipefd[0]);
	_read_pipes.push_back(pipefd[1]);
}

void	Webserver::setWritePipes(void)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "Write pipe(): " << strerror(errno) << std::endl;
		exit(1);
	}
	_write_pipes.push_back(pipefd[0]);
	_write_pipes.push_back(pipefd[1]);
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

std::string	Webserver::getHost()
{
	return (_host);
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

int	Webserver::getBodyMaxLength(void)
{
	return (_config->getBodyMaxLength());
}

/* ************************************************************************** */