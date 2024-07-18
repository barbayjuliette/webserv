/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 19:33:34 by yliew             #+#    #+#             */
/*   Updated: 2024/07/10 19:33:35 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigFile::ConfigFile() {}

ConfigFile::ConfigFile(const char *file)
{
	openFile(file);
	readFile();
	if (DEBUG)
		printContexts(this->_servers);
	validateConfig();
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigFile::~ConfigFile()
{
	for (std::vector<ServerConfig*>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
	{
		delete *it;
	}
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ConfigFile::openFile(const char *file)
{
	this->_config.open(file, std::ios::in);
	if (!this->_config)
		throw ConfigReadError("Failed to open configuration file");
}

/*
- Skip empty lines and comments
- Identify valid contexts:
	1. http
	2. server (may be nested in http block)
	3. location (must be nested in server block)
- Check syntax: matching open/close braces for each block */
void	ConfigFile::readFile(void)
{
	std::string	curr_line;
	this->_open_braces = 0;

	while (std::getline(this->_config, curr_line))
	{
		t_strvec tokens = tokenizeLine(curr_line);
		checkBraces(tokens);
		if (tokens.empty() || (tokens.size() == 1 && (tokens[0] == "{" || tokens[0] == "}")))
			continue ;

		ServerConfig	*server = NULL;
		switch (checkContext(tokens[0]))
		{
			case HTTP:
				continue ;
			case SERVER:
				server = new ServerConfig();
				this->_servers.push_back(server);
				readServerContext(server);
				break ;
			default:
				throw ConfigReadError("Invalid context or syntax");
		}
	}
	if (this->_open_braces != 0)
		throw ConfigReadError("Braces not closed");
	this->_config.close();
}

/* Get server details to be inserted in the server.directives map
- If an open brace is found, validate and parse the context */
void	ConfigFile::readServerContext(ServerConfig* server)
{
	std::string	curr_line;

	while (std::getline(this->_config, curr_line))
	{
		t_strvec tokens = tokenizeLine(curr_line);
		checkBraces(tokens);
		if (tokens.empty() || (tokens.size() == 1 && tokens[0] == "{"))
			continue ;
		if (tokens.size() == 1 && tokens[0] == "}")
			return ;

		if (checkContext(tokens[0]) == LOCATION)
		{
			LocationConfig	*location = new LocationConfig();

			server->setLocation(location);
			location->setPath(tokens);
			readLocationContext(location);
		}
		else
			addKeyValues(tokens, server->getDirectives());
	}
	throw ConfigReadError("Braces not closed");
}

/* Get location details to be inserted the server.location[].directives map */
void	ConfigFile::readLocationContext(LocationConfig* location)
{
	std::string	curr_line;

	while (std::getline(this->_config, curr_line))
	{
		t_strvec tokens = tokenizeLine(curr_line);
		checkBraces(tokens);
		if (tokens.empty() || (tokens.size() == 1 && tokens[0] == "{"))
			continue ;
		if (tokens.size() == 1 && tokens[0] == "}")
			return ;
		addKeyValues(tokens, location->getDirectives());
	}
	throw ConfigReadError("Braces not closed");
}

/* Create pair of std::string and std::vector<string> to insert in the given map */
void	ConfigFile::addKeyValues(t_strvec& tokens, t_strmap& map)
{
	std::string	key = tokens[0];

	t_strvec	values;
	for (size_t i = 1; i < tokens.size(); i++)
		values.push_back(tokens[i]);

	map.insert(map.end(), std::make_pair(key, values));
}

void	ConfigFile::validateConfig(void)
{
	for (std::vector<ServerConfig*>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
	{
		if (DEBUG)
			std::cout << CYAN << "\nCHECKING SERVER DIRECTIVES:\n" << RESET;
		(*it)->validateKeys();

		std::vector<LocationConfig*>	locations = (*it)->getLocations();

		for (size_t i = 0; i < locations.size(); i++)
		{
			if (DEBUG)
				std::cout << CYAN << "\nCHECKING LOCATION DIRECTIVES: " << locations[i]->getPath() << '\n' << RESET;
			locations[i]->validateKeys();
		}
	}
}

/*
** --------------------------- TOKENIZATION UTILS -----------------------------
*/

/* Split string by whitespaces and store the tokens in a vector */
t_strvec	ConfigFile::tokenizeLine(std::string& line)
{
	t_strvec	tokens;

	removeComments(line);
	for (size_t i = 0; i < line.size(); i++)
	{
		if (!std::isspace(line[i]))
		{
			size_t	end = line.find_first_of(WHITESPACE, i);
			if (end == std::string::npos)
				end = line.size();
			tokens.push_back(line.substr(i, end - i));
			i = end;
		}
	}
	// std::cout << CYAN << "PRINTING TOKENS:\n"<< RESET;
	// for (t_strvec::iterator it = tokens.begin(); it != tokens.end(); it++)
	// 	std::cout << "token: " << *it << '\n';
	trimSemicolon(tokens);
	return (tokens);
}

void	ConfigFile::removeComments(std::string& line)
{
	size_t	i = line.find("#");

	if (i != std::string::npos)
	{
		line.erase(i, std::string::npos);
	}
}

void	ConfigFile::trimSemicolon(t_strvec& tokens)
{
	if (tokens.empty())
		return ;

	t_strvec::iterator	last = tokens.end();
		last--;

	int	lastIndex = last->size() - 1;
	if ((*last)[lastIndex] == ';')
		last->erase(lastIndex, 1);
}

void	ConfigFile::checkBraces(t_strvec& tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "{")
			this->_open_braces++;
		else if (tokens[i] == "}")
		{
			this->_open_braces--;
			if (this->_open_braces < 0)
				throw ConfigReadError("Invalid syntax");
		}
	}
}

int	ConfigFile::checkContext(std::string& context)
{
	std::string	arr[3] = {"http", "server", "location"};

	for (int i = 0; i < 3; i++)
	{
		if (context == arr[i])
			return (i);
	}
	return (-1);
}

/*
** ---------------------------------- PRINT -----------------------------------
*/

void	ConfigFile::printContexts(std::vector<ServerConfig*>& vec)
{
	std::cout << CYAN << "\nPRINTING CONTEXTS\n" << RESET;

	for (std::vector<ServerConfig*>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		std::cout << CYAN << "\nSERVER:\n" << RESET;

		ServerConfig	*server = *it;
		printMap(server->getDirectives());
		std::vector<LocationConfig*>	locations = server->getLocations();

		for (size_t i = 0; i < locations.size(); i++)
		{
			std::cout << CYAN << "--> LOCATION: " << locations[i]->getPath();
			std::cout << "\n--> match exact: " << locations[i]->getMatchExact();
			std::cout << "; case sensitive: " << locations[i]->getCaseSensitive();
			std::cout << '\n' << RESET;
			printMap(locations[i]->getDirectives());
		}
	}
}

void	ConfigFile::printMap(t_strmap& map)
{
	for (t_strmap::iterator it = map.begin(); it != map.end(); it++)
	{
		std::cout << GREEN << '\t' << it->first << ":\n" << RESET;
		for (size_t i = 0; i < it->second.size(); i++)
			std::cout << "\t\tvalue [" << i << "]: " << it->second[i] << '\n';
	}
}

/*
** -------------------------------- ACCESSORS ---------------------------------
*/

std::vector<ServerConfig*>	ConfigFile::getServers(void)
{
	return (this->_servers);
}

/*
** -------------------------------- EXCEPTIONS --------------------------------
*/

ConfigFile::ConfigReadError::ConfigReadError(const std::string& message) \
	: _message("Configuration file: " + message) {};

ConfigFile::ConfigReadError::~ConfigReadError() throw() {}

const char	*ConfigFile::ConfigReadError::what() const throw()
{
	return (_message.c_str());
}
