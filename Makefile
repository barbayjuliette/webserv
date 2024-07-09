SRCS = srcs/ListeningSocket.cpp \
		srcs/main.cpp

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
RM = rm -rf
NAME = webserv
OBJS = ${SRCS:.cpp=.o}

all: ${NAME}

$(NAME): ${OBJS}
	$(CC) $(CFLAGS) ${OBJS} -o ${NAME}

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) ${OBJS}

fclean: clean
	$(RM) ${NAME}

re: fclean all

.PHONY: all clean fclean re
