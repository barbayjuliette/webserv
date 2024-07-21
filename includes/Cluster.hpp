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

class Cluster
{
	private:
		static Cluster* 			_instance;
		static ConfigFile*			_config_file;
		static int					_epoll_fd;
		std::map<int, Webserver*>	_servers;

	public:
		/* Constructors */
		Cluster();
		Cluster(ConfigFile* config_file);
		Cluster(const Cluster& src);

		/* Operator overload */
		Cluster&	operator=(const Cluster& src);

		/* Destructor */
		~Cluster();

		/* Epoll utils */
		void			initEpoll(void);
		static void		addToEpoll(int socket_fd, struct epoll_event *ep_event);
		static void		removeFromEpoll(int socket_fd);

		/* Methods */
		void			runServers(void);
		void			handle_client_events(int client_socket, uint32_t event_type);

		/* Utils */
		bool			is_server_socket(int fd);
		void			check(int num);
		static void		signal_handler(int signum);
};

#endif
