/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:35 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/10 14:52:57 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include "Webserver.hpp"

int main(void)
{
	Webserver server(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 12);
	server.run();
	return 0;
}
