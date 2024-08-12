/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Print.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yliew <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 19:33:44 by yliew             #+#    #+#             */
/*   Updated: 2024/07/10 19:33:46 by yliew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_HPP
# define PRINT_HPP

# include "webserv.hpp"

template <typename T>
inline void	printValue(std::string message, T& value);

/* Organize template functions for printing */
class Print
{
	private:
		Print() {}; //should not be constructed
		Print(const Print& src);
		Print&	operator=(const Print& src);

	public:
		virtual ~Print() = 0;

		template <typename T>
		static void	printLine(std::string message, T value)
		{
			printValue(message, value);
			std::cout << '\n';
		}

		template <typename T, typename V>
		static void	printMap(std::string message, std::map<T, V>& map)
		{
			if (map.empty())
				return ;

			if (message.size() > 0)
				std::cout << CYAN << std::setw(20) << message << RESET << '\n';

			typename std::map<T, V>::iterator	it;
			for (it = map.begin(); it != map.end(); it++)
			{
				std::cout << GREEN << std::setw(21) << "[" << it->first << "] = " << RESET;
				printValue("", it->second);
				std::cout << '\n';
			}
		}

		template <typename T, typename V>
		static void	printMultimap(std::multimap<T, V>& mmap)
		{
			if (mmap.empty())
				return ;

			typename std::multimap<T, V>::iterator	it;
			for (it = mmap.begin(); it != mmap.end(); it++)
			{
				std::cout << GREEN << std::setw(21) << "[" << it->first << "] = " << RESET;
				printValue("", it->second);
				std::cout << '\n';
			}
		}

		template <typename T>
		static void	printVector(std::string message, T& vec)
		{
			if (vec.empty())
				return ;

			if (message.size() > 0)
				std::cout << CYAN << std::setw(20) << message << RESET;

			for (size_t i = 0; i < vec.size(); i++)
			{
				std::cout << vec[i];
				if (i != vec.size() - 1)
					std::cout << ' ';
				else
					std::cout << '\n';
			}
		}
};

/* Generic template function for printing map values */
template <typename T>
inline void	printValue(std::string message, T& value)
{
	if (message.size() > 0)
		std::cout << CYAN << std::setw(20) << message << RESET;
	std::cout << value;
}

/* Specialized template function for printing map values of std::vector<string> */
template <>
inline void	printValue<std::vector<std::string> >(std::string message, std::vector<std::string>& vec)
{
	Print::printVector(message, vec);
}

#endif
