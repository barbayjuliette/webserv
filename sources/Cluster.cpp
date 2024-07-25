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

/*
** -------------------------- INIT STATIC VARIABLES ---------------------------
*/

Cluster*	Cluster::_instance = NULL;
ConfigFile*	Cluster::_config_file = NULL;
int			Cluster::_epoll_fd = -1;

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
		Webserver *server = new Webserver(servers[i]);
		this->_servers[server->getServerSocket()] = server;
	}
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Cluster&	Cluster::operator=(const Cluster& src)
{
	if (this != &src)
	{
		this->_instance = src._instance;
		this->_config_file = src._config_file;
		this->_epoll_fd = src._epoll_fd;
		this->_servers = src._servers;
	}
	return (*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cluster::~Cluster()
{
	for (size_t i = 0; i < _servers.size(); i++)
		delete _servers[i];

	if (_config_file)
		delete _config_file;
}

/*
** ---------------------------------- EPOLL -----------------------------------
*/

/* Create epoll instance
- Register each server socket fd to be monitored for specified events */
void	Cluster::initEpoll(void)
{
	_epoll_fd = epoll_create(1);
	check(_epoll_fd);

	std::map<int, Webserver*>::iterator	it;
	for (it = _servers.begin(); it != _servers.end(); it++)
	{
		struct epoll_event	ep_event;
		int					socket_fd = it->first;

		ep_event.data.fd = socket_fd;
		ep_event.events = EPOLLIN | EPOLLOUT;
		addToEpoll(socket_fd, &ep_event);
	}
}

/* int epoll_ctl(int epfd, int op, int fd, struct epoll_event *_Nullable event);
	- op: EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL */
void	Cluster::addToEpoll(int socket_fd, struct epoll_event *ep_event)
{
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, socket_fd, ep_event) < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}
}

void	Cluster::removeFromEpoll(int socket_fd)
{
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL) < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* Monitor all sockets for the specified events
- If EPOLLIN flag is set for the server socket: there is a new client connection incoming
- Call the corresponding Webserver instance to handle the events */
void	Cluster::runServers(void)
{
	initEpoll();
	struct epoll_event	ep_events[MAX_EVENTS];

	while (true)
	{
		int	num_of_events = epoll_wait(_epoll_fd, ep_events, MAX_EVENTS, TIMEOUT);
		check(num_of_events);

		for (int i = 0; i < num_of_events; i++)
		{
			int	fd = ep_events[i].data.fd;
			int	event_type = ep_events[i].events;

			if (event_type & EPOLLERR || event_type & EPOLLHUP)
			{
				std::cerr << strerror(errno) << std::endl;
			}

			if (is_server_socket(fd) && (event_type & EPOLLIN))
				_servers[fd]->accept_new_connections();
			else
				handle_client_events(fd, event_type);
		}
	}
}

/* Search each server's clients map for the current client socket fd
- Call handle_connections() on the corresponding server */
void	Cluster::handle_client_events(int client_socket, uint32_t event_type)
{
	std::map<int, Webserver*>::iterator	it;
	for (it = _servers.begin(); it != _servers.end(); it++)
	{
		Webserver	*server = it->second;
		Client		*found_client = server->getClient(client_socket);

		if (found_client)
		{
			server->handle_connections(client_socket, event_type);
			break ;
		}
	}
}

/*
** ---------------------------------- UTILS -----------------------------------
*/

bool	Cluster::is_server_socket(int fd)
{
	return (_servers.find(fd) != _servers.end());
}

void	Cluster::check(int num)
{
	if (num < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
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
