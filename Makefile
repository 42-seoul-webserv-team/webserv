.DEFAULT_GOAL := all
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
INC = ./include
NAME = webserv
BUILD = ./src
TARGET = Location.cpp \
	   Server.cpp \
	   ft.cpp \
	   Response.cpp	\
	   Request.cpp	\
	   Connection.cpp \
	   WebServ.cpp \
	   Logger.cpp \
	   Kqueue.cpp \
	   ManagerException.cpp \
	   ConnectionException.cpp \
	   RedirectionException.cpp \
	   HTTPSender.cpp \
	   main.cpp
SRC = $(TARGET:%=$(BUILD)/%) 
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

.PHONY: all clean fclean re

-include $(DEP)

all: $(NAME)

clean:
	rm -f $(OBJ)
	rm -f $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -I$(INC) -c -o $@ $<
