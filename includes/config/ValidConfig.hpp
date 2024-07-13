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

#ifndef VALID_CONFIG_HPP
# define VALID_CONFIG_HPP

# include "webserv.hpp"
# include "ConfigFile.hpp"

class ConfigFile;
class ValidConfig;
class LocationConfig;

typedef std::vector<std::string>		t_strvec;
typedef std::map<std::string, t_strvec>	t_strmap;
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

		std::map<int, std::string>	_error_page;

	public:
		/* Constructors */
		ValidConfig();
		ValidConfig(const ValidConfig& src);

		/* Operator overload */
		ValidConfig&	operator=(const ValidConfig& src);

		/* Destructor */
		virtual ~ValidConfig() = 0;

		/* Init functions */
		// virtual void	initValidDirectives(void);
		void			initErrorPages(void);

		/* Validation functions */
		void	validateDirectives(void);
		void	setListenPort(t_strvec& tokens);
		void	setClientMaxBodySize(t_strvec& tokens);
		void	setAutoindex(t_strvec& tokens);
		void	setHost(t_strvec& tokens);
		void	setRoot(t_strvec& tokens);
		void	setAlias(t_strvec& tokens);
		void	setRedirect(t_strvec& tokens);
		void	setServerName(t_strvec& tokens);
		void	setIndex(t_strvec& tokens);
		void	setAllowedMethods(t_strvec& tokens);
		void	setErrorPages(t_strvec& tokens);

		/* Accessors */
		t_strmap&	getDirectives(void);

		/* Exception handling */
		class InvalidConfigError : public std::exception
		{
			private:
				std::string	_message;

			public:
				InvalidConfigError(const std::string& message);
				virtual	~InvalidConfigError() throw();
				virtual const char	*what() const throw();
		};
};

/* Each server block can contain multiple location blocks */
class ServerConfig : public ValidConfig
{
	private:
		std::vector<LocationConfig*>		_locations;
		std::map<std::string, t_directives>	_validDirectives;
	
	public:
		/* Constructors */
		ServerConfig();
		ServerConfig(const ServerConfig& src);

		/* Operator overload */
		ServerConfig&	operator=(const ServerConfig& src);

		/* Destructor */
		~ServerConfig();

		/* Member functions */
		void	setLocation(LocationConfig* location);

		/* Accessors */
		std::vector<LocationConfig*>	getLocations(void);
};

/* Each location block is nested inside a server block
- path: the location identifier to compare with a requested url
- path modifiers:
	=: equal sign: match a location block exactly against a requested URI.
	~: tilde: case-sensitive regular expression match against a requested URI.
	~*: tilde followed by asterisk: case insensitive regular expression match against a requested URI. */
class LocationConfig : public ValidConfig
{
	private:
		std::string	_path; //location-specific directive
		bool		_match_exact;
		bool		_case_sensitive;
		std::map<std::string, t_directives>	_validDirectives;

	public:
		enum e_modifier
		{
			MATCH_EXACT,
			CASE_SENSITIVE,
			CASE_INSENSITIVE
		};

		/* Constructors */
		LocationConfig();
		LocationConfig(const LocationConfig& src);

		/* Operator overload */
		LocationConfig&	operator=(const LocationConfig& src);

		/* Destructor */
		~LocationConfig();

		/* Validation functions */
		void	setPath(t_strvec& tokens);
		int		checkPathModifier(std::string& path);
		int		setPathModifier(std::string& token);

		/* Accessors */
		std::string	getPath(void);
		bool		getMatchExact(void);
		bool		getCaseSensitive(void);
};

#endif
