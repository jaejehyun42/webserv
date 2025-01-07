CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

# 폴더별 객체 파일
SERVER_OBJ = server/Serv.o server/main.o
CONFIG_OBJ = config/LocBlock.o config/ServBlock.o config/ServConf.o config/utils_conf.o

# 최종 실행 파일
TARGET = Webserv

# 기본 규칙
all: $(TARGET)

# 서버 파일 컴파일
server/Serv.o: server/Serv.cpp server/Serv.hpp
	$(CC) $(CFLAGS) -c server/Serv.cpp -o server/Serv.o

server/main.o: server/main.cpp
	$(CC) $(CFLAGS) -c server/main.cpp -o server/main.o

# config 파일 컴파일
config/LocBlock.o: config/LocBlock.cpp config/LocBlock.hpp
	$(CC) $(CFLAGS) -c config/LocBlock.cpp -o config/LocBlock.o

config/ServBlock.o: config/ServBlock.cpp config/ServBlock.hpp
	$(CC) $(CFLAGS) -c config/ServBlock.cpp -o config/ServBlock.o

config/ServConf.o: config/ServConf.cpp config/ServConf.hpp
	$(CC) $(CFLAGS) -c config/ServConf.cpp -o config/ServConf.o

config/utils_conf.o: config/utils_conf.cpp config/utils_conf.hpp
	$(CC) $(CFLAGS) -c config/utils_conf.cpp -o config/utils_conf.o

# 최종 실행 파일 생성
$(TARGET): $(SERVER_OBJ) $(CONFIG_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) $(CONFIG_OBJ) -o $(TARGET) $(LDFLAGS)

# 클린
clean:
	rm -f $(SERVER_OBJ) $(CONFIG_OBJ)

fclean: clean
	rm -f $(TARGET)
