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
		std::map<std::string, LocationConfig*>	_locations;

	public:
		/* Constructors */
		ServerConfig();
		ServerConfig(const ServerConfig& src);

		/* Operator overload */
		ServerConfig&	operator=(const ServerConfig& src);

		/* Destructor */
		~ServerConfig();

		/* Member functions */
		void			initValidKeys(void); //overload
		void			validateKeys(void); //overload
		void			setLocation(const std::string& path, LocationConfig* location);
		LocationConfig*	matchLocation(const std::string& path);
		void			setCGIPath(std::map<std::string, std::string> cgi_map);


		/* Print */
		void			printConfig(void);

		/* Accessors */
		std::map<std::string, LocationConfig*>	getLocations(void);
		LocationConfig*	getLocation(const std::string& path);
};

#endif
