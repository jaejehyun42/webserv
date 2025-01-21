#-----STANDARD-----#
NAME = Conf

CC =		c++
CFLAG =		-Wall -Wextra -Werror -std=c++98 -g -fsanitize=address

#------COLORS------#
RED =		\033[0;31m
BLUE =		\033[0;34m
GREEN =		\033[1;32m
YELLOW =	\033[0;33m
RESET =		\033[0m

#-------FILES-------#
MAIN_FILE =		config/main			\

CONFIG_DIR = 	config/
CONFIG_FILE =	ServConf			\
				ServBlock			\
				LocBlock			\
				utils_conf			\

HEADS += $(addprefix $(CONFIG_DIR), $(CONFIG_FILE))

FILES += $(MAIN_FILE)
FILES += $(HEADS)


#------SOURCE------#
SRCS = $(addsuffix .cpp, $(FILES))
OBJS = $(addsuffix .o, $(FILES))
INCS = $(addsuffix .hpp, $(HEADS))


#------FUCTION------#
all: $(NAME)

%.o : %.cpp $(INCS)
	@echo "$(YELLOW)Compiling $<$(RESET)"
	@$(CC) $(CFLAG) -I. -c $< -o $@

$(NAME) : $(OBJS) $(INCS)
	@echo "$(GREEN)Linking Files...$(RESET)"
	@$(CC) $(CFLAG) $(SRCS) -o $(NAME)
	@echo "$(GREEN)Build Complete - $(NAME)$(RESET)"

clean :
	@echo "$(RED)Cleaning Object Files...$(RESET)"
	@rm -rf $(OBJS)

fclean : clean
	@echo "$(RED)Removing Executable $(NAME)...$(RESET)"
	@rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re