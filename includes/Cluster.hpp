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

struct PortInfo
{
	int						fd;
	std::vector<Webserver*>	servers;
};

class Cluster
{
	private:
		static Cluster* 							_instance;
		static ConfigFile*							_config_file;
		static int									_epoll_fd;
		std::map<int/*port no.*/, struct PortInfo>	_server_sockets;

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
		bool			portIsFound(int port);
		void			initServerSocket(int port, struct addrinfo *addr, int backlog);
		void			addServerSocket(int port, int socket_fd);
		void			addServer(int port, Webserver *new_server);

		/* Methods */
		void			runServers(void);
		void			handle_client_events(int client_socket, uint32_t event_type);

		/* Utils */
		bool			is_server_socket(int fd);
		void			check(int num);
		static void		signal_handler(int signum);
		int				countServers(int port);
		void			printServerSockets(void);
};

#endif
