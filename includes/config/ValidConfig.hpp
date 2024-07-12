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
# include "Webserver.hpp"
# include "ConfigFile.hpp"

class ConfigFile;

typedef void (ValidConfig::*t_directives)(const t_strvec&);

/* Abstract class for ServerConfig and LocationConfig to inherit from
- directives: instructions to configure the server behaviour, stored in key-value pairs */
class ValidConfig
{
	protected:
		/* Key-value pairs extracted from the config file */
		t_strmap	_directives;

		/* Values after parsing _directives */
		int			_listen_port;
		int			_client_max_body_size;
		bool		_autoindex; //init false

		std::string	_host;
		std::string	_root;
		std::string	_alias;
		std::string	_return; //redirect uri

		t_strvec	_server_name;
		t_strvec	_index;
		t_strvec	_allow_methods;
		t_strvec	_cgi_path;

		std::map <int, std::string>	_error_page;

	public:
		/* Constructors */
		ValidConfig();
		ValidConfig(const ValidConfig& src);

		/* Operator overload */
		ValidConfig&	operator=(const ValidConfig& src);

		/* Destructor */
		virtual ~ValidConfig() = 0;

		/* Init functions */
		virtual void	initValidDirectives(void);
		void			initErrorPages(void);

		/* Validation functions */
		void	setListenPort(t_strvec tokens);
		void	setClientMaxBodySize(t_strvec tokens);
		void	setAutoindex(t_strvec tokens);
		void	setHost(t_strvec tokens);
		void	setRoot(t_strvec tokens);
		void	setAlias(t_strvec tokens);
		void	setRedirect(t_strvec tokens);
		void	setServerName(t_strvec tokens);
		void	setIndex(t_strvec tokens);
		void	setAllowedMethods(t_strvec tokens);
		void	setErrorPages(t_strvec tokens);

		/* Exception handling */
		class InvalidConfig : public std::exception
		{
			private:
				std::string	_message;

			public:
				InvalidConfig(const std::string& message);
				virtual	~InvalidConfig() throw();
				virtual const char	*what() const throw();
		};
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
		ServerConfig(const ServerConfig& src);

		/* Operator overload */
		ServerConfig&	operator=(const ServerConfig& src);

		/* Destructor */
		~ServerConfig();
}

/* Each location block is nested inside a server block
- pattern: the identifier that comes after "location", before the open brace '{' */
class LocationConfig : public ServerConfig
{
	private:
		std::string	_pattern; //location-specific directive
		std::map <std::string, t_directives>	_validDirectives;

	public:
		/* Constructors */
		LocationConfig();
		LocationConfig(const LocationConfig& src);

		/* Operator overload */
		LocationConfig&	operator=(const LocationConfig& src);

		/* Destructor */
		~LocationConfig();
}

#endif
