/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidConfig.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 18:47:46 by yliew             #+#    #+#             */
/*   Updated: 2024/07/12 18:57:40 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"
# include "ConfigFile.hpp"

typedef void (ConfigFile::*t_directives)(const std::vector<std::string>&);

/* Abstract class for ServerConfig and LocationConfig to inherit from */
class ValidConfig
{
	protected:
		/* Key-value pairs extracted from the config file */
		t_strmap	_directives;

		/* Values after parsing */
		int			_listen_port;
		int			_client_max_body_size;
		bool		_autoindex; //init false

		std::string	_host;
		std::string	_root;
		std::string	_alias;
		std::string	_return; //redirect uri

		t_strvec	_server_name;
		t_strvec	_index;
		t_strvec	_error_page;
		t_strvec	_allow_methods;
		t_strvec	_cgi_path;

	public:
		/* Constructors */
		ValidConfig();
		ValidConfig(const char *file);
		ValidConfig(const ValidConfig& src);

		/* Operator overload */
		ValidConfig&	operator=(const ValidConfig& src);

		/* Destructor */
		virtual ~ValidConfig() = 0;

		/* Member functions */
		virtual void	initValidDirectives(void);
}

/* Each server block can contain multiple location blocks */
class ServerConfig : public ValidConfig
{
	private:
		std::vector <LocationConfig*>			_locations;
		std::map <std::string, t_directives>	_validDirectives;
	
	public:
		/* Constructors */
		ServerConfig();
		ServerConfig(const char *file);
		ServerConfig(const ServerConfig& src);

		/* Operator overload */
		ServerConfig&	operator=(const ServerConfig& src);

		/* Destructor */
		~ServerConfig();
}

/* Each location block is nested inside a server block
- directives: instructions to configure the server behaviour, stored in key-value pairs
- pattern: the identifier that comes after "location", before the open brace '{' */
class LocationConfig : public ServerConfig
{
	private:
		std::string	_pattern;
		std::map <std::string, t_directives>	_validDirectives;
	
	public:
		/* Constructors */
		LocationConfig();
		LocationConfig(const char *file);
		LocationConfig(const LocationConfig& src);

		/* Operator overload */
		LocationConfig&	operator=(const LocationConfig& src);

		/* Destructor */
		~LocationConfig();
}

#endif
