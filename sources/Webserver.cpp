/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 14:32:20 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/10 17:45:44 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Webserver.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

// void	Webserver::initialize(int domain, int type, int protocol, int port, u_long interface, int backlog)
// {

// }

Webserver::Webserver() : ListeningSocket(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 12)
{


}

Webserver::Webserver(int domain, int type, int protocol, int port, u_long interface, int backlog) :
ListeningSocket(domain, type, protocol, port, interface, backlog)
{

}

Webserver::Webserver( const Webserver & src ) : ListeningSocket(src)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Webserver::~Webserver()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Webserver &				Webserver::operator=( Webserver const & rhs )
{
	ListeningSocket::operator=(rhs);
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Webserver::handle_read_connection(int client_socket)
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
	std::vector<std::string>	request;
	std::string					path;
	int							status_code;
	std::string					status_text;

	if (bytes_read <= 0)
	{
		close(client_socket);
		FD_CLR(client_socket, &_current_sockets);
	}
	std::stringstream			stream(buffer);
	std::string					content;
	std::string					str;
	char						c;

	while (stream >> content)
	{
		request.push_back(content);
	}
	if (request[0] == "GET")
		std::cout << "It's a GET request\n";
	path = request[1];
	if (path == "/")
		path = "/index.html";
	
	path = "./wwwroot" + path;

	std::ifstream	page(path.c_str());
	if (page.good())
	{
		while (page.get(c))
			str += c;
		status_code = 200;
		status_text = "OK";
		page.close();
	}
	else
	{
		std::cerr << strerror(errno) << std::endl;
		status_code = 404;
		status_text = "Not found";
	}
	// std::cout << "Status: " << status_code << std::endl;
	stream.clear();
	stream << "HTTP/1.1 " << status_code << status_text << "\r\n";
	stream << "Cache-Control: no-cache, private\r\n";
	stream << "Content-Type: text/html\r\n";
	stream << "Content-Length: " << str.size() << "\r\n";
	stream << "\r\n";
	stream << str;

	std::string	message = stream.str();
	send(client_socket, message.c_str(), message.size() + 1, 0);
}

void		Webserver::handle_write_connection(int client_socket)
{
	(void)client_socket;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/



/* ************************************************************************** */