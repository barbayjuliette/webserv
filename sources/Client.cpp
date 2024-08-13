/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:00:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 13:27:08 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"
# include "webserv.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client()
{
}

Client::Client(int socket, struct sockaddr_in addr) :
_socket(socket), _addr(addr), _server(NULL), _request(NULL), _response(NULL)
{
}

Client::Client( const Client & src ) :
_socket(src._socket),
_addr(src._addr),
_server(src._server),
_request(src._request),
_response(src._response)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client()
{
	if (_response)
		delete (_response);
	if (_request)
		delete (_request);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &				Client::operator=( Client const & rhs )
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->_addr = rhs._addr;
		this->_server = rhs._server;
		this->_request = rhs._request;
		this->_response = rhs._response;
	}
	return (*this);
}


/*
** --------------------------------- METHODS ----------------------------------
*/
void	Client::reset(void)
{
	// delete (_request);
	// _request = NULL;
	if (_response)
		delete (_response);
	_response = NULL;
	delete (_request);
}

void	Client::setRequest(Request* request)
{
	_request = request;
}

void	Client::setResponse(Response* response)
{
	_response = response;
}

void	Client::setServer(Webserver* server)
{
	_server = server;
}

void	Client::deleteRequest(void)
{
	if (_request)
		delete _request;
	_request = NULL;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

Request*	Client::getRequest()
{
	return (this->_request);
}

Response*	Client::getResponse()
{
	return (this->_response);
}

int		Client::getSocket()
{
	return (this->_socket);
}

Webserver *Client::getServer()
{
	return (this->_server);
}

unsigned short	Client::getPort()
{
	return (this->_addr.sin_port);
}

unsigned long	Client::getIPAddress()
{
	return (this->_addr.sin_addr.s_addr);
}

/* ************************************************************************** */