FROM ubuntu:16.04

WORKDIR /home/aliaksandr/sandbox/hlcup2017

RUN apt-get -qq update
RUN apt-get install g++ make -y
RUN apt-get install -qq libboost-all-dev -y
RUN apt-get install sqlite3 -y
RUN apt-get install cmake -y


#RUN mkdir /alex
COPY . .
#COPY *.cpp *.h storage Makefile Dockerfile /alex
RUN rm -rf CMakeFiles
RUN cmake .
RUN make -f Makefile


EXPOSE 80

# Запускаем наш сервер
#CMD ./server #$COMPILED_FILE_PATH
CMD uname -a; ("./server" 80 || echo "Solution has crashed")

