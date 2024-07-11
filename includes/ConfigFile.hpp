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

typedef std::vector <std::string>			t_strvec;
typedef std::map <std::string, t_strvec>	t_strmap;

/* Structs to store details of the blocks/context hierarchy */
/* Each location block is nested inside a server block
- directives: instructions to configure the server behaviour, stored in key-value pairs
- pattern: the identifier that comes after "location", before the open brace '{' */
struct s_LocationContext
{
	t_strmap	directives;
	std::string	pattern;
};

/* Each server block can contain multiple location blocks */
struct s_ServerContext
{
	t_strmap	directives;
	// int			listen_port;
	// int			client_max_body_size;
	// std::string	host;
	// std::string	root;
	// t_strvec	server_name;
	// t_strvec	index;
	// t_strvec	error_page;
	std::vector <s_LocationContext>	locations;
};

/* Class definition */
class ConfigFile
{
	private:
		std::ifstream					_config;
		std::vector <s_ServerContext>	_contexts;

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
		void	readServerContext(s_ServerContext& server);
		void	readLocationContext(s_LocationContext& location);
		void	addKeyValues(t_strvec& tokens, t_strmap& map);

		/* Utils */
		void	printContexts(std::vector <s_ServerContext>& vec);
		void	printMap(t_strmap& map);
		t_strvec	tokenizeLine(std::string& line);
		void	removeComments(std::string& line);
		void	trimSemicolon(t_strvec& tokens);
		int		checkContext(std::string& context);

		/* Exception handling */
		class ConfigFailure : public std::exception
		{
			private:
				std::string	_message;

			public:
				ConfigFailure(const std::string& message);
				virtual	~ConfigFailure() throw();
				virtual const char	*what() const throw();
		};
};

#endif
