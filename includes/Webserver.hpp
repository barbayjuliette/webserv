/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 14:30:19 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/11 13:30:27 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "ListeningSocket.hpp"
# include "Client.hpp"

# include <iostream>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
// # include <cstdlib>

# define BUFFER_SIZE 5000

class Webserver : public ListeningSocket
{
	private:
		
	public:

		Webserver();
		Webserver(int domain, int type, int protocol, int port, u_long interface, int backlog);
		Webserver( Webserver const & src );
		~Webserver();

		void			handle_read_connection(int i);
		void			handle_write_connection(int client_socket);
		Webserver &		operator=( Webserver const & rhs );
};

