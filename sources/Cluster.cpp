/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 19:18:04 by yliew             #+#    #+#             */
/*   Updated: 2024/08/13 10:56:25 by jbarbay          ###   ########.fr       */
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

Cluster::Cluster(const Cluster& src) :
_server_sockets(src._server_sockets),
_clients(src._clients)
{

}

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

		t_mmap::iterator	res = findHostPort(host, port);
		if (res == _server_sockets.end())
			initServerSocket(host, port, configs[i]->getAddressInfo());

		Webserver	*new_server = new Webserver(configs[i]);
		addServer(host, port, new_server);
	}

	if (CTRACE)
		printServerSockets();
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Cluster&	Cluster::operator=(const Cluster& src)
{
	if (this != &src)
	{
		this->_server_sockets = src._server_sockets;
		this->_clients = src._clients;
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

	std::map<int, Client*>::iterator	it_client;
	for (it_client = _clients.begin() ; it_client != _clients.end(); it_client++)
	{
		delete (it_client->second);
	}

	if (_config_file)
		delete _config_file;
}

/*
** ------------------------------- INIT SOCKETS -------------------------------
*/

/* Check if a socket has been created for a specific host:port combination
- Return iterator to the _server_sockets element if it exists */
t_mmap::iterator	Cluster::findHostPort(const std::string& host, int port)
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
		if (socket_fd < 0)
		{
			std::cerr << RED << "socket(): " << strerror(errno) << std::endl << RESET;
			exit(1);
		}
		setNonBlocking(socket_fd);
		setReuseAddr(socket_fd);
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
}

void	Cluster::addServer(std::string& host, int port, Webserver *new_server)
{
	t_mmap::iterator	it = findHostPort(host, port);
	it->second.servers.push_back(new_server);
}

void	Cluster::setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << RED << "fcntl(): " << strerror(errno) << std::endl << RESET;
		exit(1);
	}
}

void	Cluster::setNonBlocking(std::vector<int> fds)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		setNonBlocking(fds[i]);
	}
}

void	Cluster::setReuseAddr(int socket_fd)
{
	int	yes = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
	{
		std::cerr << RED << "setsockopt(): " << strerror(errno) << std::endl << RESET;
		exit(1);
	}
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
		addToEpoll(it->second.fd, EPOLLIN | EPOLLOUT);
	}
}

/* int epoll_ctl(int epfd, int op, int fd, struct epoll_event *_Nullable event);
	- op: EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL */
void	Cluster::addToEpoll(int fd, uint32_t events)
{
	struct epoll_event	ep_event;

	ep_event.data.fd = fd;
	ep_event.events = events;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ep_event) < 0)
	{
		std::cerr << "addToEpoll: " << strerror(errno) << std::endl;
		exit(1);
	}
}

void	Cluster::removeFromEpoll(int fd)
{
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
	{
		std::cerr << "removeFromEpoll: " << strerror(errno) << std::endl;
		exit(1);
	}
}

void Cluster::checkTimeout(void)
{
	std::map<int /*socket fd*/, Client*>::iterator it;
	time_t now = time(NULL);

	// if (TIMEOUT_DEBUG)
	// {
	// 	std::cout << "checking timeout 1" << std::endl;
	// }
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (TIMEOUT_DEBUG)
		{
			std::cout << "checking timeout" << std::endl;
		}
		if (now - it->second->getRequest()->getTimeout() > REQ_TIMEOUT)
		{
			if (TIMEOUT_DEBUG)
			{
				std::cout << "Closing connection due to timeout" << std::endl;
			}
			removeClient(it->first);
		}
	}
}

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
		if (num_of_events == 0)
			checkTimeout();
		for (int i = 0; i < num_of_events; i++)
		{
			int	fd = ep_events[i].data.fd;
			int	event_type = ep_events[i].events;

			if (is_server_socket(fd) && (event_type & EPOLLIN))
				accept_new_connections(fd);
			else if (is_cgi_pipe(fd))
				handle_cgi(_cgi_pipes[fd], event_type);
			else
			{
				if (event_type & EPOLLIN)
				{
					try
					{
						handle_read_connection(fd);
					}
					catch (std::exception& e)
					{
						std::cerr << RED << "handle_read_connection(): " << e.what() << ".\n" << RESET;
					}
				}
				if (event_type & EPOLLOUT)
				{
					handle_write_connection(fd);
				}
			}
		}
	}
}

/*
** ----------------------------- HANDLE CONNECTIONS ----------------------------
*/

/* Add new client to the clients map
- Create epoll_event struct for the new client socket and register it to be monitored */
void	Cluster::accept_new_connections(int server_socket)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len = sizeof(client_addr);

	int	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
	check(client_socket);
	check(fcntl(server_socket, F_SETFL, O_NONBLOCK));

	if (getExistingClient(&client_addr) != -1)
	{
		std::cout << RED << "\nIs existing client. Closing duplicate socket.\n\n" << RESET;
		close(client_socket);
	}

	addToEpoll(client_socket, EPOLLIN | EPOLLOUT);

	_clients[client_socket] = new Client(client_socket, client_addr);
	if (CTRACE)
		std::cout << GREEN << "\nNew client created: " << client_socket << "\n\n" << RESET;
}

int	Cluster::getExistingClient(struct sockaddr_in *addr)
{
	std::map<int, Client*>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getPort() == addr->sin_port
			&& it->second->getIPAddress() == addr->sin_addr.s_addr)
			return (it->first);
	}
	return (-1);
}

void	Cluster::removeClient(int client_socket)
{
	close(client_socket);
	// Cluster::removeFromEpoll(client_socket);
	Client *to_delete = _clients[client_socket];
	_clients.erase(client_socket);
	delete to_delete;
}

Client*		Cluster::getClient(int socket)
{
	if (_clients.find(socket) == _clients.end())
		return (NULL);
	return (_clients[socket]);
}

void	Cluster::assignServer(int client_socket)
{
	Client*		client = _clients[client_socket];
	Request*	request = client->getRequest();

	std::string name = request->getHost();
	std::string host = request->getHost();
	if (!isIPAddress(host))
	{
		host = getClientIPAddress(client_socket);
	}
	client->setServer((getServerByPort(name, host, request->getPort())));
	if (!client->getServer())
		throw std::runtime_error("No server matched the request");
	// If server is valid -> set server for request
	client->getRequest()->setServer(client->getServer());
	if (CTRACE)
	{
		std::cout << GREEN << "found server match\n" << RESET;
		client->getServer()->printServerNames();
	}
	// Set request's body max length to server config's body max len
	request->setBodyMaxLength(client->getServer()->getConfig()->getBodyMaxLength());
}

/* Preliminary request parsing: extract host and port to determine which server to route to */
void	Cluster::handle_read_connection(int client_socket)
{
	char	buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int		bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			std::cerr << strerror(errno) << std::endl;
		else if (DEBUG)
			std::cout << "Client closed the connection\n";
		removeClient(client_socket);
	}
	else // valid bytes read
	{
		// If not existing request -> create new request
		if (!_clients[client_socket]->getRequest())
		{
			Request*	new_request = new Request(buffer, bytes_read);
			_clients[client_socket]->setRequest(new_request);
			if (new_request->getHeaderLength() != -1)
				assignServer(client_socket);
			if (new_request->getReqComplete() == false)
				return;
		}
		/* If existing request -> check if header is complete
			-> If incomplete, handle header
				-> Check again if header complete */
		Request*	request = _clients[client_socket]->getRequest();

		if (request->getHeaderLength() == -1)
		{
			request->handle_incomplete_header(bytes_read, buffer);
			if (request->getReqComplete() == false) // If request complete, create response
				return;
		}
		else if (!request->getReqComplete()) // if chunked -> process chunk -> create response
			request->handle_chunk(buffer, bytes_read);
		if (request->getHeaderLength() != -1 && request->getReqComplete() == true) 
		{
			if (!_clients[client_socket]->getServer())
				assignServer(client_socket);
			request->checkBodyLength();
			std::cout << CYAN << "[PORT " << request->getPort() << "]: " << RESET << request->getMethod() << " request received for " << request->getServer()->getServerName()[0] << std::endl;
		// std::cout << request->getServer()->getServerName()[0] << " received a " << request->getMethod() << " request, on PORT "  << std::endl;
			if (TIMEOUT_DEBUG)
			{
				request->print_variables();
			}

			request->getServer()->create_response(request, _clients[client_socket]);

			int	cgi_status = _clients[client_socket]->getResponse()->getCGIStatus();
			if (cgi_status != NO_CGI)
				add_cgi_pipes(_clients[client_socket], _clients[client_socket]->getResponse(), cgi_status);
		}
	}
}

void	Cluster::handle_write_connection(int client_socket)
{
	Client			*client = getClient(client_socket);
	if (!client)
		return ;
	Response		*response = client->getResponse();
	if (!response)
		return ;
	if (response->getCGIStatus() != NO_CGI && response->getCGIStatus() != CGI_DONE)
		return ;

	unsigned int	bytes_sent;

	bytes_sent = send(client->getSocket(), response->getFullResponse().c_str(), response->getFullResponse().size(), 0);
	if (response->getCGIStatus() == CGI_DONE)
		remove_cgi_pipes(client->getRequest(), response);
	if (bytes_sent == response->getFullResponse().size())
	{
		if (CTRACE)
			std::cout << CYAN << "\ninside handle_write_connection: fd " << client_socket << "\n\n" << RESET;
		if (DEBUG)
		{
			std::cout << GREEN << "---- Response sent to client ----\n" << RESET;
			std::cout << response->getFullResponse() << std::endl;
			std::cout << GREEN << "End of response\n" << RESET;
		}
		if ((response->getHeaders())["Connection"] == "keep-alive")
		{
			client->reset();
		}
		else
		{
			removeClient(client_socket);
		}
	}
	else
	{
		std::cerr << RED << "Error sending response to client " << client->getSocket() << std::endl << RESET;
		removeClient(client_socket);
		// Error, so we need to remove client??
	}
}

/*
** ------------------------------------- CGI -----------------------------------
*/

void	Cluster::handle_cgi(Client *client, uint32_t event_type)
{
	Request		*request = client->getRequest();
	Response	*response = client->getResponse();
	if (!request || !response)
		return ;

	int	cgi_status = response->getCGIStatus();
	if (event_type & EPOLLOUT && (cgi_status == CGI_GET || cgi_status == CGI_POST))
	{
		try
		{
			response->getCGIHandler()->write_cgi(cgi_status);
			if (cgi_status == CGI_POST && response->getCGIHandler()->get_pid() == 0)
			{
				response->setCGIStatus(CGI_POST_READ);
				return ;
			}
		}
		catch (std::exception& e)
		{
			std::cerr << RED << "write_cgi(): " << e.what() << ".\n" << RESET;
		}
	}
	else if (event_type & EPOLLIN && (cgi_status == CGI_GET
		|| cgi_status == CGI_POST || cgi_status == CGI_POST_READ))
	{
		try
		{
			if (cgi_status == CGI_POST_READ)
			{
				response->getCGIHandler()->read_cgi_request(cgi_status);
			}
			else
			{
				response->getCGIHandler()->read_cgi_result(cgi_status);
				response->process_cgi_response(*request);
			}
		}
		catch (std::exception& e)
		{
			std::cerr << RED << "read_cgi(): " << e.what() << ".\n" << RESET;
		}
	}
}

void	Cluster::add_cgi_pipes(Client *client, Response *response, int cgi_status)
{
	if (cgi_status == CGI_GET)
	{
		std::vector<int>	response_pipe = response->getCGIHandler()->get_response_pipe();
		setNonBlocking(response_pipe);

		_cgi_pipes[response_pipe[0]] = client;
		addToEpoll(response_pipe[0], EPOLLIN);

		_cgi_pipes[response_pipe[1]] = client;
		addToEpoll(response_pipe[1], EPOLLOUT);
	}
	else if (cgi_status == CGI_POST)
	{
		std::vector<int>	response_pipe = response->getCGIHandler()->get_response_pipe();
		std::vector<int>	request_pipe = response->getCGIHandler()->get_request_pipe();
		setNonBlocking(response_pipe);
		setNonBlocking(request_pipe);

		_cgi_pipes[response_pipe[0]] = client;
		addToEpoll(response_pipe[0], EPOLLIN);

		_cgi_pipes[request_pipe[0]] = client;
		addToEpoll(request_pipe[0], EPOLLIN);

		_cgi_pipes[request_pipe[1]] = client;
		addToEpoll(request_pipe[1], EPOLLOUT);
	}
}

void	Cluster::remove_cgi_pipes(Request *request, Response *response)
{
	if (!request || !response || !response->getCGIHandler())
		return ;
	(void)request;

	std::vector<int>	response_pipe = response->getCGIHandler()->get_response_pipe();
	for (size_t i = 0; i < response_pipe.size(); i++)
	{
		_cgi_pipes.erase(response_pipe[i]);
	}

	std::vector<int>	request_pipe = response->getCGIHandler()->get_request_pipe();
	for (size_t i = 0; i < request_pipe.size(); i++)
	{
		_cgi_pipes.erase(request_pipe[i]);
	}
}

bool	Cluster::is_cgi_pipe(const int fd)
{
	if (_cgi_pipes.find(fd) != _cgi_pipes.end())
		return (true);
	return (false);
}

/*
** -------------------------------- SERVER UTILS --------------------------------
*/

Webserver*	Cluster::getServerByPort(const std::string& name, const std::string& host, int port)
{
	t_mmap::iterator	it = findHostPort(host, port);
	if (it == _server_sockets.end())
		return (NULL);
	int	count = countServers(it);
	if (count <= 0)
		return (NULL);
	else if (count == 1)
		return (it->second.servers[0]);
	return (getServerByName(it->second.servers, name));
}

Webserver*	Cluster::getServerByName(std::vector<Webserver*>& servers, const std::string& name)
{
	if (servers.empty())
		return (NULL);

	for (size_t i = 0; i < servers.size(); i++)
	{
		std::vector<std::string>	server_names = servers[i]->getServerName();

		for (size_t j = 0; j < server_names.size(); j++)
		{
			// if (CTRACE)
			// 	std::cout << "now comparing " << server_names[j] << " with " << name << '\n';
			if (server_names[j] == name)
				return (servers[i]);
		}
	}
	return (servers[0]);
}

bool	Cluster::is_server_socket(const int fd)
{
	t_mmap::iterator	it;

	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		if (fd == it->second.fd)
			return (true);
	}
	return (false);
}

bool	Cluster::isIPAddress(const std::string& str)
{
	std::stringstream			stream(str);
	std::vector<std::string>	tokens;
	std::string					token;

	while (std::getline(stream, token, '.'))
	{
		for (size_t i = 0; i < token.size(); i++)
		{
			if (!std::isdigit(token[i]))
				return (false);
		}
		tokens.push_back(token);
	}
	if (tokens.size() == 4)
		return (true);
	return (false);
}

/* Get the client's address from the socket and convert it to a string
- Used if the Host header specifies the server url instead of the IP address */
std::string	Cluster::getClientIPAddress(const int client_socket)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len = sizeof(client_addr);

	if (getsockname(client_socket, (struct sockaddr*)&client_addr, &client_len) == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return ("");
	}

	unsigned char	*ip_bytes = (unsigned char *)&client_addr.sin_addr.s_addr;
	size_t	size = sizeof(client_addr.sin_addr.s_addr);
	std::stringstream	stream;

	for (size_t i = 0; i < size; i++)
	{
		stream << (int)ip_bytes[i];
		if (i != size - 1)
			stream << '.';
	}
	return (stream.str());
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

/*
** ---------------------------------- PRINT -----------------------------------
*/

void	Cluster::printServers(std::vector<Webserver*>& servers)
{
	size_t	i = 0;

	for (std::vector<Webserver*>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << GREEN << "\n\nSERVER [" << ++i << "]: ";
		std::cout << (*it)->getHost() << ":" << (*it)->getPort() << '\n' << RESET;
		(*it)->printConfig();
	}
}

void	Cluster::printServerSockets(void)
{
	t_mmap::iterator	it;

	for (it = _server_sockets.begin(); it != _server_sockets.end(); it++)
	{
		std::cout << "\n-------------------------------------------------------\n\n";
		std::cout << CYAN << "HOST: " << it->second.host << "; PORT: " << it->first << '\n' << RESET;
		std::cout << "- Socket fd: " << it->second.fd << '\n';
		std::cout << "- No. of servers listening: " << countServers(it) << "\n";
		std::cout << "\n-------------------------------------------------------\n\n";
		printServers(it->second.servers);
		std::cout << "\n-------------------------------------------------------\n";
	}
}

/*
** ---------------------------------- UTILS -----------------------------------
*/

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
