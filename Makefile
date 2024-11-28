NAME = ircserv

INCLUDE = ./

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

FILES = main.cpp Server.cpp User.cpp Channel.cpp utils.cpp
OBJECTS = $(FILES:.cpp=.o)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INCLUDE)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -I $(INCLUDE)

clean:
	rm -rf $(OBJECTS)


fclean: clean
	rm -rf $(NAME)

re: fclean all
