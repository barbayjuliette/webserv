/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 19:34:58 by yliew             #+#    #+#             */
/*   Updated: 2024/07/16 17:55:52 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#ifndef DEBUG
# define DEBUG 1
#endif

/* C++ headers */
# include <iostream>
# include <sstream>
# include <fstream>
# include <string>
# include <exception>
/* STL */
# include <vector>
# include <map>
# include <algorithm>
# include <iterator>

/* C headers */
# include <cstring>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <csignal>
# include <cctype>
/* Networking */
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
/* System calls */
# include <unistd.h>
# include <sys/select.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>

/* Colours */
# define CYAN "\001\033[1;36m\002"
# define GREEN "\001\033[1;32m\002"
# define RED "\001\033[1;31m\002"
# define RESET "\001\033[0m\002"
# define WHITESPACE " \f\n\r\t\v"

#endif
