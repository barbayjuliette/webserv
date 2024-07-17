/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 19:18:04 by yliew             #+#    #+#             */
/*   Updated: 2024/07/17 19:18:06 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

Cluster*	Cluster::_instance = NULL;
ConfigFile*	Cluster::_config_file = NULL;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cluster::Cluster() {}

Cluster::Cluster(ConfigFile* config_file)
{
	_instance = this;
	_config_file = config_file;
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

	std::vector<ServerConfig*>	servers = this->_config_file->getServers();

	for (size_t i = 0; i < servers.size(); i++)
	{
		// Webserver *server = new Webserver(servers[i], AF_INET, SOCK_STREAM, 0, INADDR_ANY, 12);
		Webserver *server = new Webserver(servers[i]);
		this->_servers.push_back(server);
	}
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cluster::~Cluster()
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		delete this->_servers[i];
	}
	delete this->_config_file;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Cluster::runServers(void)
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		int	pid = fork();
		if (pid == 0)
		{
			this->_servers[i]->run();
		}
	}
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		waitpid(-1, NULL, 0);
	}
}

void	Cluster::signal_handler(int signum)
{
	if (_instance)
	{
		delete (_instance);
	}
    std::cout << "\nSignal received, webserver closed. Bye bye!" << std::endl;
    exit(signum);
}
