/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 19:11:47 by yliew             #+#    #+#             */
/*   Updated: 2024/07/17 19:11:49 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "webserv.hpp"
# include "Webserver.hpp"
# include "Client.hpp"
# include "ConfigFile.hpp"

# define MAX_EVENTS 10
# define TIMEOUT 3
# define REQ_TIMEOUT 3

class ConfigFile;
class ServerConfig;
class Webserver;
class Client;

struct SocketInfo
{
	int						fd;
	std::string				host;
	std::vector<Webserver*>	servers;
};

typedef std::multimap<int, struct SocketInfo> t_mmap;

class Cluster
{
	private:
		static Cluster* 								_instance;
		static ConfigFile*								_config_file;
		static int										_epoll_fd;
		std::multimap<int /*port*/, struct SocketInfo>	_server_sockets;
		std::map<int /*socket fd*/, Client*>			_clients;
		std::map<int /*cgi pipefd*/, Client*>			_cgi_pipes;

	public:
		/* Constructors */
		Cluster();
		Cluster(ConfigFile* config_file);
		Cluster(const Cluster& src);

		/* Operator overload */
		Cluster&	operator=(const Cluster& src);

		/* Destructor */
		~Cluster();

		/* Epoll methods */
		void			initEpoll(void);
		static void		addToEpoll(int fd, uint32_t events);
		static void		removeFromEpoll(int fd);
		void			add_cgi_fds(Client *client, std::vector<int> pipefd);

		/* Server socket methods */
		void				setNonBlocking(int fd);
		void				setReuseAddr(int socket_fd);
		t_mmap::iterator	findHostPort(const std::string& host, int port);
		Webserver*			getServerByPort(const std::string& name, const std::string& host, int port);
		Webserver*			getServerByName(std::vector<Webserver*>& servers, const std::string& name);
		void				initServerSocket(std::string& host, int port, struct addrinfo *addr);
		void				addServerSocket(std::string& host, int port, int socket_fd);
		void				addServer(std::string& host, int port, Webserver *new_server);

		/* Methods */
		void			runServers(void);
		void			accept_new_connections(int server_socket);
		void			handle_read_connection(int client_socket);
		void			handle_write_connection(int client_socket);
		void			handle_client_events(int client_socket, uint32_t event_type);
		void			removeClient(int client_socket);

		/* Utils */
		bool			is_server_socket(const int fd);
		bool			is_cgi_fd(const int fd);
		bool			isIPAddress(const std::string& str);
		std::string		getClientIPAddress(const int socket_fd);
		int				getExistingClient(struct sockaddr_in *addr);
		void			check(int num);
		static void		signal_handler(int signum);
		int				countServers(std::string& host, int port);
		int				countServers(t_mmap::iterator res);
		void 			checkTimeout(void);

		/* Print */
		void			printServerSockets(void);
		void			printServers(std::vector<Webserver*>& servers);
		void			assignServer(int client_socket);

		// getter
		Client*			getClient(int socket);
};

#endif
