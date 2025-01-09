#-----STANDARD-----#
NAME = Webserv

CC =		c++
CFLAG =		-Wall -Wextra -Werror -std=c++98

#------COLORS------#
RED =		\033[0;31m
BLUE =		\033[0;34m
GREEN =		\033[1;32m
YELLOW =	\033[0;33m
RESET =		\033[0m

#-------FILES-------#
MAIN_FILE =		server/main			\

SERVER_DIR = 	server/
SERVER_FILE =	Server				\

CONFIG_DIR = 	config/
CONFIG_FILE =	ServConf			\
				ServBlock			\
				LocBlock			\
				utils_conf			\

REQUEST_DIR = 	request/
REQUEST_FILE =	Request				\

#RESPONCE_DIR = responce/
#RESPONCE_FILE = Responce			

HEADS += $(addprefix $(SERVER_DIR), $(SERVER_FILE))
HEADS += $(addprefix $(CONFIG_DIR), $(CONFIG_FILE))
HEADS += $(addprefix $(REQUEST_DIR), $(REQUEST_FILE))

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