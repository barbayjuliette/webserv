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
# include "Print.hpp"

/* Errors */
# define PARAM_COUNT_ERR "Invalid number of parameters"

class ValidConfig;

typedef std::vector<std::string>		t_strvec;
typedef std::map<std::string, t_strvec>	t_strmap;
typedef void (ValidConfig::*t_directive)(const t_strvec&);
typedef std::map<std::string, t_directive> t_dirmap;

/* Abstract class for ServerConfig and LocationConfig to inherit from
- directives: instructions to configure the server behaviour, stored in key-value pairs */
class ValidConfig
{
	protected:
		/* Key-value pairs extracted from the config file */
		t_strmap		_directives;

		/* Values after parsing _directives */
		int				_port;
		size_t			_body_max_length;
		bool			_autoindex; //init false - toggles directory listing for when no index file is found

		struct addrinfo	*_address_info;
		std::string		_host;
		std::string		_root;
		std::string		_redirect;
		std::string		_index;

		t_strvec		_server_name;
		t_strvec		_allowed_methods;
		t_strvec		_cgi_ext; //cgi file extensions
		t_strvec		_cgi_exec; //paths to cgi executable

		std::map<int, std::string>			_error_page;
		std::map<std::string, t_directive>	_validKeys;

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

		/* Validation functions */
		virtual void	validateKeys(void) = 0;
		void			parseListenPort(const t_strvec& tokens);
		void			parseBodyMaxLength(const t_strvec& tokens);
		void			parseAutoindex(const t_strvec& tokens);
		void			parseHost(const t_strvec& tokens);
		void			parseAddressInfo(std::string& host, std::string port);
		void			parseRoot(const t_strvec& tokens);
		void			parseRedirect(const t_strvec& tokens);
		void			parseServerName(const t_strvec& tokens);
		void			parseIndex(const t_strvec& tokens);
		void			parseAllowedMethods(const t_strvec& tokens);
		void			parseErrorPages(const t_strvec& tokens);

		/* Utils */
		int				strToInt(const std::string& str);
		int				strToSizet(const std::string& str);
		std::string		intToStr(const int nb);
		bool			isStatusCode(const std::string& str);
		int				isDirectory(const std::string& str);
		static int		isRegularFile(const std::string& str);
		bool			isValidMethod(const std::string& str);

		/* Accessors */
		t_strmap&		getDirectives(void);
		int				getPort(void);
		int				getBodyMaxLength(void);
		bool			getAutoindex(void);
		struct addrinfo	*getAddressInfo(void);
		std::string		getHost(void);
		std::string		getRoot(void);
		std::string		getRedirect(void);
		std::string		getIndex(void);
		t_strvec		getServerName(void);
		t_strvec		getAllowedMethods(void);
		std::string		getErrorPage(int status_code);

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
