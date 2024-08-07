/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:00:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/06 19:45:53 by jbarbay          ###   ########.fr       */
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
_socket(socket), _addr(addr), _request(NULL), _response(NULL)
{
}

Client::Client( const Client & src ) :
_socket(src._socket),
_addr(src._addr),
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
	delete (_response);
	_response = NULL;
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

/*struct sockaddr_in {
    short            sin_family;
    unsigned short   sin_port;
    struct in_addr   sin_addr;
    char             sin_zero[8];
};
struct in_addr {
    unsigned long s_addr;
};*/
unsigned short	Client::getPort()
{
	return (this->_addr.sin_port);
}

unsigned long	Client::getIPAddress()
{
	return (this->_addr.sin_addr.s_addr);
}

/* ************************************************************************** */