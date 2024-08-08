/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RobotomyRequestForm.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 16:48:31 by jbarbay           #+#    #+#             */
/*   Updated: 2024/05/28 17:37:47 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/RobotomyRequestForm.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

RobotomyRequestForm::RobotomyRequestForm() : AForm("RobotomyRequestForm", 72, 45), _target("none")
{
	std::cout << "Created RobotomyRequestForm with target " << this->getTarget() << std::endl;
}

RobotomyRequestForm::RobotomyRequestForm(std::string target) : AForm("RobotomyRequestForm", 72, 45), _target(target)
{
	std::cout << "Created RobotomyRequestForm with target " << this->getTarget() << std::endl;
}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm& src) : AForm(src), _target(src._target)
{
	std::cout << "Copy constructed RobotomyRequestForm with target " << this->getTarget() << std::endl;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

RobotomyRequestForm::~RobotomyRequestForm()
{
	std::cout << "Destructed RobotomyRequestForm " << this->getTarget() << std::endl;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

RobotomyRequestForm &	RobotomyRequestForm::operator=( RobotomyRequestForm const & rhs )
{
	std::cout << "RobotomyRequestForm assignment operator called" << std::endl;
	if (this != &rhs)
	{
		AForm::operator=(rhs);
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	RobotomyRequestForm::execute(Bureaucrat const &executor) const
{
	this->execution_rights(executor);
	std::cout << "DRILLING NOISES" <<std::endl;
	static bool success = true;
	if (success == true)
		std::cout <<  this->getTarget() << " has been robotomized" << std::endl;
	else
		std::cout << "Robotomy failed" << std::endl;
	success = !success;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string	RobotomyRequestForm::getTarget(void) const
{
	return (this->_target);
}