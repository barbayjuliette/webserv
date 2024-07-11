/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:00:42 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 14:58:54 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client()
{

}

Client::Client(int socket) : _socket(socket)
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
	delete (_response);
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

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

Request&	Client::getRequest()
{
	return *(this->_request);
}

Response&	Client::getResponse()
{
	return *(this->_response);
}

void	Client::setRequest(Request& request)
{
	_request = &request;
}

void	Client::setResponse(Response& response)
{
	_response = &response;
}

/* ************************************************************************** */