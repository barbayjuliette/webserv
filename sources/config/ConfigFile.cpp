/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 19:33:34 by yliew             #+#    #+#             */
/*   Updated: 2024/07/25 16:34:06 by jbarbay          ###   ########.fr       */
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
	// if (TRACE)
	// 	printContexts(this->_servers);
	validateConfig();
}

ConfigFile::ConfigFile(const ConfigFile& src)
{
	*this = src;
}

ConfigFile&	ConfigFile::operator=(const ConfigFile& other)
{
	if (this != &other)
	{
		this->_servers = other._servers;
		this->_open_braces = other._open_braces;
	}
	return (*this);
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
	{
		throw ConfigReadError("Failed to open " + std::string(file));
	}
	if (!isRegularFile(file))
	{
		throw ConfigReadError(std::string(file) + " is not a regular file");
	}
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
			LocationConfig	*location = new LocationConfig(server);

			location->parsePrefix(tokens);
			server->setLocation(location->getPrefix(), location);
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
	std::vector<ServerConfig*>::iterator	it;

	for (it = this->_servers.begin(); it != this->_servers.end(); it++)
	{
		// if (TRACE)
		// 	std::cout << CYAN << "\nCHECKING SERVER DIRECTIVES:\n" << RESET;
		(*it)->validateKeys();

		std::map<std::string, LocationConfig*>	locations = (*it)->getLocations();
		std::map<std::string, LocationConfig*>::iterator	loc;

		for (loc = locations.begin(); loc != locations.end(); loc++)
		{
			// if (TRACE)
			// 	std::cout << CYAN << "\nCHECKING LOCATION DIRECTIVES: " << loc->second->getPrefix() << '\n' << RESET;
			loc->second->validateKeys();
		}
	}
	checkDuplicateServers();
}

void	ConfigFile::checkDuplicateServers(void)
{
	for (size_t i = 0; i < _servers.size() - 1; i++)
	{
		for (size_t j = i + 1; j < _servers.size(); j++)
		{
			if (_servers[i]->getPort() == _servers[j]->getPort()
				&& _servers[i]->getHost() == _servers[j]->getHost())
			{
				t_strvec	names_1 = _servers[i]->getServerName();
				t_strvec	names_2 = _servers[j]->getServerName();

				if (names_1.empty() || names_2.empty())
					throw ValidConfig::InvalidConfigError(_servers[i]->getHost(), _servers[i]->getPort(), "no unique server name");

				for (size_t k = 0; k < names_1.size(); k++)
				{
					if (std::find(names_2.begin(), names_2.end(), names_1[k]) != names_2.end())
						throw ValidConfig::InvalidConfigError(_servers[i]->getHost(), _servers[i]->getPort(), names_1[k]);
				}
			}
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

int	ConfigFile::isRegularFile(const std::string& str)
{
	struct stat	buffer;
	const char	*path = str.c_str();

	stat(path, &buffer);
	return (S_ISREG(buffer.st_mode));
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

		std::map<std::string, LocationConfig*>	locations = server->getLocations();
		std::map<std::string, LocationConfig*>::iterator	loc;

		for (loc = locations.begin(); loc != locations.end(); loc++)
		{
			std::cout << CYAN << "--> LOCATION: " << loc->second->getPrefix();
			std::cout << "\n--> match exact: " << loc->second->getMatchExact();
			std::cout << "; case sensitive: " << loc->second->getCaseSensitive();
			std::cout << '\n' << RESET;
			printMap(loc->second->getDirectives());
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
	: _message("Config error: " + message) {};

ConfigFile::ConfigReadError::~ConfigReadError() throw() {}

const char	*ConfigFile::ConfigReadError::what() const throw()
{
	return (_message.c_str());
}
