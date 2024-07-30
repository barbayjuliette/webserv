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
# define TIMEOUT 1000

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
		static void		addToEpoll(int socket_fd, struct epoll_event *ep_event);
		static void		removeFromEpoll(int socket_fd);

		/* Server socket methods */
		t_mmap::iterator	findHostPort(std::string& host, int port);
		Webserver*			getServerByPort(std::string& name, std::string& host, int port);
		Webserver*			getServerByName(std::vector<Webserver*>& servers, std::string& name);
		void				initServerSocket(std::string& host, int port, struct addrinfo *addr);
		void				addServerSocket(std::string& host, int port, int socket_fd);
		void				addServer(std::string& host, int port, Webserver *new_server);

		/* Methods */
		void			runServers(void);
		int				accept_new_connections(int server_socket);
		void			handle_read_connection(int client_socket);
		void			handle_client_events(int client_socket, uint32_t event_type);

		/* Utils */
		bool			is_server_socket(const int fd);
		bool			isIPAddress(const std::string& str);
		std::string		getClientIPAddress(const int socket_fd);
		void			check(int num);
		static void		signal_handler(int signum);
		int				countServers(std::string& host, int port);
		int				countServers(t_mmap::iterator res);
		void			printServerSockets(void);
};

#endif
