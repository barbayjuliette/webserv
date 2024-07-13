/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 17:42:24 by yliew             #+#    #+#             */
/*   Updated: 2024/07/13 17:42:26 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValidConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

LocationConfig::LocationConfig() : ValidConfig(), _match_exact(false), _case_sensitive(false)
{
	std::cout << "LocationConfig was created.\n";
}

// LocationConfig::LocationConfig(const LocationConfig& other) : ValidConfig(other)
// {
// }

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

LocationConfig::~LocationConfig()
{
	std::cout << "LocationConfig was destroyed.\n";
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/* Syntax: location [modifier] [URI] (+ inline open brace '{' if applicable) */
void	LocationConfig::setPath(t_strvec& tokens)
{
	t_strvec::iterator	last = tokens.end();
	last--;

	if (*last == "{")
		tokens.erase(last);
	if (tokens.size() <= 1 || tokens.size() > 3)
		throw InvalidConfigError("Invalid number of parameters for location block");

	t_strvec::iterator	current = tokens.begin();
	current++;
	if (setPathModifier(*current) == -1)
	{
		if (tokens.size() >= 3)
			throw InvalidConfigError("Invalid parameters for location block");
	}
	else
		current++;

	this->_path = *current;
}

int	LocationConfig::checkPathModifier(std::string& token)
{
	std::string	arr[3] = {"=", "~", "~*"};

	for (int i = 0; i < 3; i++)
	{
		if (token == arr[i])
			return (i);
	}
	return (-1);
}

int	LocationConfig::setPathModifier(std::string& token)
{
	int modifier = checkPathModifier(token);

	switch (modifier)
	{
		case MATCH_EXACT:
			this->_match_exact = true;
			break ;
		case CASE_SENSITIVE:
			this->_case_sensitive = true;
			break ;
		case CASE_INSENSITIVE:
			this->_case_sensitive = false;
			break ;
		default:
			break ;
	}
	return (modifier);
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

std::string	LocationConfig::getPath(void)
{
	return (this->_path);
}

bool	LocationConfig::getMatchExact(void)
{
	return (this->_match_exact);
}

bool LocationConfig::getCaseSensitive(void)
{
	return (this->_case_sensitive);
}

