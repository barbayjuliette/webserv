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

	std::vector<ServerConfig*>	configs = _config_file->getServers();

	for (size_t i = 0; i < configs.size(); i++)
	{
		std::string	host = configs[i]->getHost();
		int	port = configs[i]->getPort();
		std::cout << CYAN << "\ncurrent server config: " << host << ':' << port << '\n' << RESET;

		t_mmap::iterator	res = findHostPort(host, port);
		if (res == _server_sockets.end())
			initServerSocket(host, port, configs[i]->getAddressInfo());

		Webserver	*new_server = new Webserver(configs[i]);
		addServer(host, port, new_server);
	}

	printServerSockets();
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
		this->_server_sockets = src._server_sockets;
	}
	return (*this);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cluster::~Cluster()
{
	t_mmap::iterator	it;

	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		std::vector<Webserver*>	servers = it->second.servers;
		for (size_t i = 0; i < servers.size(); i++)
			delete servers[i];
	}

	if (_config_file)
		delete _config_file;
}

/*
** ------------------------------- INIT SOCKETS -------------------------------
*/

/* Check if a socket has been created for a specific host:port combination
- Return iterator to the _server_sockets element if it exists */
t_mmap::iterator	Cluster::findHostPort(std::string& host, int port)
{
	t_mmap::iterator	it;

	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		if (it->first == port && it->second.host == host)
			return (it);
	}
	return (_server_sockets.end());
}

/* Try binding to each address in the addrinfo linked list until a match is found
- Create socket and make it non-blocking
- Set socket options to be able to reuse address
- Bind the socket to an address and a port
- Listen: wait for the client to make a connection */
void	Cluster::initServerSocket(std::string& host, int port, struct addrinfo *addr)
{
	int				socket_fd;
	struct addrinfo *tmp;

	for (tmp = addr; tmp != NULL; tmp = tmp->ai_next)
	{
		socket_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		check(socket_fd);

		check(fcntl(socket_fd, F_SETFL, O_NONBLOCK));

		int	yes = 1;
		check(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)));

		if (bind(socket_fd, tmp->ai_addr, tmp->ai_addrlen) < 0)
			close(socket_fd);
		else
			break ;
	}
	if (!tmp) // no bind attempt is successful
	{
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}
	check(listen(socket_fd, 12) < 0);

	addServerSocket(host, port, socket_fd);
}

void	Cluster::addServerSocket(std::string& host, int port, int socket_fd)
{
	struct SocketInfo	socket;

	socket.fd = socket_fd;
	socket.host = host;
	_server_sockets.insert(std::make_pair(port, socket));
	std::cout << GREEN << "created socket for port: " << port << ", socket_fd: " << findHostPort(host, port)->second.fd << '\n' << RESET;
}

void	Cluster::addServer(std::string& host, int port, Webserver *new_server)
{
	t_mmap::iterator	it = findHostPort(host, port);
	it->second.servers.push_back(new_server);
	std::cout << GREEN << "there are now " << countServers(it)
		<< " servers listening to " << host << ':' << port << ".\n" << RESET;
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

	t_mmap::iterator	it;
	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		struct epoll_event	ep_event;
		int					socket_fd = it->second.fd;

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
		std::cerr << "addToEpoll: " << strerror(errno) << std::endl;
		exit(1);
	}
}

void	Cluster::removeFromEpoll(int socket_fd)
{
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL) < 0)
	{
		std::cerr << "removeFromEpoll: " << strerror(errno) << std::endl;
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
	std::vector<ServerConfig*>	configs = _config_file->getServers();

	while (true)
	{
		int	num_of_events = epoll_wait(_epoll_fd, ep_events, MAX_EVENTS, TIMEOUT);
		check(num_of_events);

		for (int i = 0; i < num_of_events; i++)
		{
			int	fd = ep_events[i].data.fd;
			int	event_type = ep_events[i].events;

			if (is_server_socket(fd) && (event_type & EPOLLIN))
			{
				int	client_socket = accept_new_connections(fd);
				std::cout << client_socket << '\n';
			}
			else
			{
				char	buffer[BUFFER_SIZE];
				memset(buffer, 0, sizeof(buffer));
				int		bytes_read = recv(fd, buffer, BUFFER_SIZE, 0);

				if (bytes_read <= 0)
				{
					if (bytes_read < 0)
						std::cerr << strerror(errno) << std::endl;
					else if (DEBUG)
						std::cout << "Client closed the connection\n";
				}
				else
				{
					Request*	new_request = new Request(buffer, configs[0]);
					delete new_request;
				}
			}
			// 	handle_client_events(fd, event_type);
		}
	}
}

/* Add new client to the clients map
- Create epoll_event struct for the new client socket and register it to be monitored */
int	Cluster::accept_new_connections(int server_socket)
{
	int	client_socket = accept(server_socket, NULL, NULL);
	check(client_socket);
	check(fcntl(server_socket, F_SETFL, O_NONBLOCK));

	struct epoll_event	ep_event;

	ep_event.data.fd = client_socket;
	ep_event.events = EPOLLIN | EPOLLOUT;
	addToEpoll(client_socket, &ep_event);

    _clients[client_socket] = new Client(client_socket);
    return (client_socket);
}

// Webserver*	Cluster::get_server_instance(int fd)
// {
// 	t_mmap::iterator	it = _server_sockets.find(fd);
// 	if (it == _server_sockets.end())
// 		return (NULL);

// 	int	count = countServers(it->second.servers);
// 	if (count <= 0)
// 		return (NULL);
// 	else if (count == 1)
// 		return (it->second.servers[0]);
// 	//match server
// }

/* Search each server's clients map for the current client socket fd
- Call handle_connections() on the corresponding server */
// void	Cluster::handle_client_events(int client_socket, uint32_t event_type)
// {
// 	std::multimap<int, Webserver*>::iterator	it;
// 	for (it = _servers.begin(); it != _servers.end(); it++)
// 	{
// 		Webserver	*server = it->second;
// 		Client		*found_client = server->getClient(client_socket);

// 		if (found_client)
// 		{
// 			server->handle_connections(client_socket, event_type);
// 			break ;
// 		}
// 	}
// }

/*
** ---------------------------------- UTILS -----------------------------------
*/

bool	Cluster::is_server_socket(int fd)
{
	t_mmap::iterator	it;

	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		if (fd == it->second.fd)
			return (true);
	}
	return (false);
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

int	Cluster::countServers(std::string& host, int port)
{
	int	count = 0;
	t_mmap::iterator	res = findHostPort(host, port);
	if (res == _server_sockets.end())
		return (-1);

	std::vector<Webserver*>::iterator	it;

	for (it = res->second.servers.begin(); it != res->second.servers.end(); it++)
		count++;

	return (count);
}

int	Cluster::countServers(t_mmap::iterator res)
{
	int	count = 0;
	std::vector<Webserver*>::iterator	it;

	for (it = res->second.servers.begin(); it != res->second.servers.end(); it++)
		count++;

	return (count);
}

void	Cluster::printServerSockets(void)
{
	t_mmap::iterator	it;

	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		std::cout << CYAN << "\nHOST:PORT: " << it->second.host << ':' << it->first << '\n' << RESET;
		std::cout << "- Socket fd: " << it->second.fd << '\n';
		std::cout << "- No. of servers listening: " << countServers(it) << "\n\n";
	}
}
