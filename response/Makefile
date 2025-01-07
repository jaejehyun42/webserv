NAME := a.out
CXX := c++
RM := rm -f
# CXXFLAGS := -Wall -Wextra -Werror -std=c++98
DBG = -g -fsanitize=address
SRCS := main.cpp\
			Response.cpp\
			StatusLine.cpp\
			Header.cpp\
			GeneralHeader.cpp\
			ResponseHeader.cpp\
			EntityHeader.cpp\
			Body.cpp\
			$(filter-out ../request/main.cpp, $(wildcard ../request/*.cpp))\
			$(filter-out ../config/main.cpp, $(wildcard ../config/*.cpp))
HEADERS := Response.hpp\
			StatusLine.hpp\
			Header.hpp\
			GeneralHeader.hpp\
			ResponseHeader.hpp\
			EntityHeader.hpp\
			Body.hpp\
			$(wildcard ../request/*.hpp)\
			$(wildcard ../config/*.hpp)
# TESTFILE := test.sh

OBJS = $(SRCS:.cpp=.o)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

d : $(SRCS) $(HEADERS)
	$(CXX) $(DBG) $(SRCS) 

# test : $(NAME) $(TESTFILE)
# 	bash $(TESTFILE)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJS) input.txt out

fclean : clean
	$(RM) $(NAME)

re : fclean $(NAME)

.PHONY: all clean fclean re