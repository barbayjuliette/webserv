/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:35 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/12 20:22:10 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include "Webserver.hpp"
#include "Cluster.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << RED << "./webserv [configuration file]\n" << RESET;
		return (0);
	}

	try
	{
		ConfigFile	*config_file;
		if (argc == 2)
			config_file = new ConfigFile(argv[1]);
		else
			config_file = new ConfigFile("./config_files/default.conf");

		Cluster		*cluster = new Cluster(config_file);
		cluster->runServers();
	}
	catch (std::exception& e)
	{
		std::cerr << RED << e.what() << ".\n" << RESET;
	}
	return (0);
}
