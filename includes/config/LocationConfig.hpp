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

class ConfigFile;
class ValidConfig;

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
		// std::map<std::string, t_directives>	_validDirectives;

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
		void	initValidKeys(void); //overload
		void	setPath(t_strvec& tokens);
		int		checkPathModifier(std::string& path);
		int		setPathModifier(std::string& token);

		/* Accessors */
		std::string	getPath(void);
		bool		getMatchExact(void);
		bool		getCaseSensitive(void);
};

#endif
