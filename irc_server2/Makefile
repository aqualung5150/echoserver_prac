CXX = c++
# CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXXFLAGS = -std=c++98
INCLUDE = -I.
NAME = ft_irc

SRCS = main.cpp Server.cpp User.cpp Channel.cpp Command.cpp
OBJS = $(SRCS:.cpp=.o)

.cpp.o :
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(NAME) $(OBJS)

all : $(NAME)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY : all clean fclean re