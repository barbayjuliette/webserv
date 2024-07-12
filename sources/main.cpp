/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:35 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/12 15:00:42 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include "Webserver.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << RED << "./webserv [configuration file]\n" << RESET;
		return (0);
	}
	try
	{
		ConfigFile	config(argv[1]);

		Webserver *server = new Webserver(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 12);
		server->run();

		delete (server);
	}
	catch (std::exception& e)
	{
		std::cerr << RED << "Error: " << e.what() << ".\n" << RESET;
	}
	return 0;
}
