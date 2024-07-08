/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:41:35 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/08 22:11:08 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ListeningSocket.hpp"

int main(void)
{
	ListeningSocket server(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 1024);
	return 0;
}
