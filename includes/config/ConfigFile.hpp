/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 19:33:44 by yliew             #+#    #+#             */
/*   Updated: 2024/07/10 19:33:46 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include "webserv.hpp"
# include "ValidConfig.hpp"

class ValidConfig;
class ServerConfig;
class LocationConfig;

typedef std::vector<std::string>		t_strvec;
typedef std::map<std::string, t_strvec>	t_strmap;

/* Class definition */
class ConfigFile
{
	private:
		std::ifstream					_config;
		std::vector<ServerConfig*>		_servers;
		int								_open_braces;

	public:
		enum e_context
		{
			HTTP,
			SERVER,
			LOCATION
		};

		/* Constructors */
		ConfigFile();
		ConfigFile(const char *file);
		ConfigFile(const ConfigFile& src);

		/* Operator overload */
		ConfigFile&	operator=(const ConfigFile& src);

		/* Destructor */
		~ConfigFile();

		/* Member functions */
		void	openFile(const char *file);
		void	readFile(void);
		void	readServerContext(ServerConfig *server);
		void	readLocationContext(LocationConfig* location);
		void	addKeyValues(t_strvec& tokens, t_strmap& map);

		/* Utils */
		void	printContexts(std::vector<ServerConfig*>& vec);
		void	printMap(t_strmap& map);
		t_strvec	tokenizeLine(std::string& line);
		void	removeComments(std::string& line);
		void	trimSemicolon(t_strvec& tokens);
		void	checkBraces(t_strvec& tokens);
		int		checkContext(std::string& context);

		/* Exception handling */
		class ConfigReadError : public std::exception
		{
			private:
				std::string	_message;

			public:
				ConfigReadError(const std::string& message);
				virtual	~ConfigReadError() throw();
				virtual const char	*what() const throw();
		};
};

#endif
