FROM ubuntu:latest

RUN apt-get update && \
apt-get upgrade -y && \
apt-get install -y sudo iputils-ping curl telnet vim wget

ENTRYPOINT ["/bin/bash"]

## docker build -t my_ubuntu ubuntu
## docker run --name my_ubuntu my_ubuntu

## docker network inspect 에서 nginx ip 확인
## ubuntu에서 telnet <nginx_ip> 80

## docker pull nginx
## docker run -t nginx -p 8080 80 nginx (호스트 8080과 80 연결)