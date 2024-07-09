# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agan <marvin@42.fr>                        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/09 14:32:15 by agan              #+#    #+#              #
#    Updated: 2024/07/09 14:32:16 by agan             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# program name
NAME	= webserv

# compiler
CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 -g
RM		= rm -fr

# directories
INC_DIR = ./includes
INC 	= -I $(INC_DIR)
SRC_DIR = ./sources

# build files
SRCS	= $(addprefix $(SRC_DIR)/, \
		main.cpp \
		ListeningSocket.cpp)
OBJS 	= ${SRCS:.cpp=.o}

#------------------------------------------------------------------------

# colours
GREEN = \033[0;32m
B_GREEN = \033[1;32m
BROWN = \033[0;33m
B_BROWN = \033[1;33m
END = \033[0m

#------------------------------------------------------------------------

all: $(NAME)

# build program
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(B_GREEN)$(NAME) compiled$(END)"

# build objects
.cpp.o:
	@${CC} ${CFLAGS} -c $< -o ${<:.cpp=.o} $(INC)
	@echo "$(B_GREEN)$< compiled.$(END)"

# clean rules
clean:
	@rm -fr $(OBJS)

fclean: clean
	@rm -fr $(NAME)
	@echo "$(B_GREEN)fclean completed$(END)"

re: fclean all

.PHONY: all clean fclean re
