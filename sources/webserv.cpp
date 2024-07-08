/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 19:30:36 by agan              #+#    #+#             */
/*   Updated: 2024/07/08 19:30:37 by agan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int webserv::startServer()
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		throw std::runtime_error("start server error");
		return 1;
	}

	if (bind(socket_fd, (sockaddr *)&socketAddr, socketAddr_len) < 0)
	{
		throw std::runtime_error("binding error");
		return 1;
	}
	return 0;
}

void webserv::startListen()
{
	if (listen(socket_fd, 20) < 0)
	{
		throw std::runtime_error("socket listen failed");
	}

	std::ostringstream ss;
	ss << "\n***Listening on ADDRESS: "
		<< ipToString(socketAddr.sin_addr.s_addr)
		<< " PORT: " << ntohs(socketAddr.sin_port)
		<< "***\n\n";
	std::cout << ss.str();
}

void webserv::initSocket()
{
	port = 8080;
	socketAddr_len = sizeof(socketAddr);
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(8080);
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

webserv::webserv()
{
	this->initSocket();
	this->startServer();
}

webserv::~webserv()
{
	close(socket_fd);
}


