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

Client::Client(int socket) : _socket(socket), _request(NULL), _response(NULL)
{
	if (CTRACE)
		std::cout << CYAN << "Client created: " << socket << std::endl << RESET;
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
	delete (_response);
	_response = NULL;
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

void	Client::setRequest(Request* request)
{
	_request = request;
}

void	Client::setResponse(Response& response)
{
	_response = &response;
}

int		Client::getSocket()
{
	return (_socket);
}

/* ************************************************************************** */