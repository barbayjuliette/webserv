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

class ValidConfig;

typedef std::vector<std::string>		t_strvec;
typedef std::map<std::string, t_strvec>	t_strmap;
typedef void (ValidConfig::*t_directives)(const t_strvec&);
typedef std::map<std::string, t_directives> t_dirmap;

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
		std::map<std::string, t_directives>	_validKeys;

	public:
		/* Constructors */
		ValidConfig();
		ValidConfig(const ValidConfig& src);

		/* Operator overload */
		ValidConfig&	operator=(const ValidConfig& src);

		/* Destructor */
		virtual ~ValidConfig();

		/* Init functions */
		virtual void	initValidKeys(void) = 0;
		// void			initErrorPages(void);

		/* Validation functions */
		void	validateKeys(void);
		void	setListenPort(const t_strvec& tokens);
		void	setClientMaxBodySize(const t_strvec& tokens);
		void	setAutoindex(const t_strvec& tokens);
		void	setHost(const t_strvec& tokens);
		void	setRoot(const t_strvec& tokens);
		void	setAlias(const t_strvec& tokens);
		void	setRedirect(const t_strvec& tokens);
		void	setServerName(const t_strvec& tokens);
		void	setIndex(const t_strvec& tokens);
		void	setAllowedMethods(const t_strvec& tokens);
		void	setErrorPages(const t_strvec& tokens);

		/* Utils */
		int		convertToInt(const std::string& str);

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

#endif
