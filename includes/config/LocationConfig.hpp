/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 18:47:46 by yliew             #+#    #+#             */
/*   Updated: 2024/07/12 18:57:40 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_HPP
# define LOCATION_CONFIG_HPP

# include "ValidConfig.hpp"
# include "ServerConfig.hpp"

class ConfigFile;
class ValidConfig;
class ServerConfig;

/* Each location block is nested inside a server block
- path: the location identifier to compare with a requested url
- path modifiers:
	=: equal sign: match a location block exactly against a requested URI.
	~: tilde: case-sensitive regular expression match against a requested URI.
	~*: tilde followed by asterisk: case insensitive regular expression match against a requested URI. */
class LocationConfig : public ValidConfig
{
	private:
		ServerConfig*	_server;
		std::string		_path;
		std::map<std::string, std::string>	_cgi_path; //[ext] = path
		bool			_match_exact;
		bool			_case_sensitive;

		LocationConfig(); //should not be constructed without server

	public:
		enum e_modifier
		{
			MATCH_EXACT,
			CASE_SENSITIVE,
			CASE_INSENSITIVE
		};

		/* Constructors */
		LocationConfig(ServerConfig* server);
		LocationConfig(const LocationConfig& src);

		/* Operator overload */
		LocationConfig&	operator=(const LocationConfig& src);

		/* Destructor */
		~LocationConfig();

		/* Validation functions */
		void	initValidKeys(void); //overload
		void	validateKeys(void); //overload
		void	parseCGIPath(t_strvec& exts, t_strvec& paths);
		void	parsePath(t_strvec& tokens);
		int		checkPathModifier(std::string& path);
		int		parsePathModifier(std::string& token);
		size_t	comparePath(const std::string& path);

		/* Print */
		void			printConfig(void);

		/* Accessors */
		std::string	getPath(void);
		bool		getMatchExact(void);
		bool		getCaseSensitive(void);
};

#endif
