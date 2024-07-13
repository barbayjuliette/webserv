/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 18:47:46 by yliew             #+#    #+#             */
/*   Updated: 2024/07/12 18:57:40 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include "ValidConfig.hpp"
# include "LocationConfig.hpp"

class ConfigFile;
class ValidConfig;
class LocationConfig;

/* Each server block contains a vector of pointers to location blocks */
class ServerConfig : public ValidConfig
{
	private:
		std::vector<LocationConfig*>		_locations;
		// std::map<std::string, t_directives>	_validDirectives;
	
	public:
		/* Constructors */
		ServerConfig();
		ServerConfig(const ServerConfig& src);

		/* Operator overload */
		ServerConfig&	operator=(const ServerConfig& src);

		/* Destructor */
		~ServerConfig();

		/* Member functions */
		void	initValidKeys(void); //overload
		void	setLocation(LocationConfig* location);

		/* Accessors */
		std::vector<LocationConfig*>	getLocations(void);
};

#endif
