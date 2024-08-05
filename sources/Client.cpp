/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:00:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/16 17:40:22 by jbarbay          ###   ########.fr       */
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
}

void	Client::setRequest(Request* request)
{
	_request = request;
}

void	Client::setResponse(Response* response)
{
	_response = response;
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