FROM ubuntu:16.04

WORKDIR /root

RUN apt-get -qq update
RUN apt-get install g++ make -y
RUN apt-get install -qq libboost-all-dev -y
RUN apt-get install sqlite3 -y

ADD . .

#COPY main.cpp storage Makefile data /root/
RUN make -f Makefile


EXPOSE 80

# Запускаем наш сервер
#CMD ./server #$COMPILED_FILE_PATH
CMD uname -a; ("./server" 80 || echo "Solution has crashed")

