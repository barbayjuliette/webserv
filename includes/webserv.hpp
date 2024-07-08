/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 19:30:22 by agan              #+#    #+#             */
/*   Updated: 2024/07/08 19:30:23 by agan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __WEBSERV_HPP__
#define __WEBSERV_HPP__

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

class webserv {
	public:
		webserv();
		~webserv();

		void startListen();

	private:
		int socket_fd;
		int port;
		sockaddr_in socketAddr;
		int socketAddr_len;

		int startServer();
		void initSocket();
};

std::string ipToString(uint32_t ip);

#endif
