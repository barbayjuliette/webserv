/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:35 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/09 13:32:56 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include "ConfigFile.hpp"

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
	// ListeningSocket server(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 12);
	}
	catch (std::exception& e)
	{
		std::cerr << RED << "Error: " << e.what() << ".\n" << RESET;
	}
	return 0;
}
