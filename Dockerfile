FROM ubuntu:16.04

WORKDIR /home/aliaksandr/sandbox/hlcup2017


ENV MYSQL_USER=mysql \
    MYSQL_DATA_DIR=/var/lib/mysql \
    MYSQL_RUN_DIR=/run/mysqld \
    MYSQL_LOG_DIR=/var/log/mysql

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-server \
 && rm -rf ${MYSQL_DATA_DIR} \
 && rm -rf /var/lib/apt/lists/*

RUN apt-get -qq update
RUN apt-get install g++ make -y
RUN apt-get install -qq libboost-all-dev -y
RUN apt-get install sqlite3 libsqlite3-dev -y
RUN apt-get install libmysqlcppconn-dev -y
RUN apt-get install cmake -y
RUN apt-get install unzip -y

COPY . .
RUN rm -rf CMakeFiles
RUN cmake .
RUN make -f Makefile
RUN rm -f data/*

COPY entrypoint.sh /sbin/entrypoint.sh
RUN chmod 755 /sbin/entrypoint.sh

EXPOSE 3306/tcp
VOLUME ["${MYSQL_DATA_DIR}", "${MYSQL_RUN_DIR}"]
ENTRYPOINT ["/sbin/entrypoint.sh"]
CMD ["/usr/bin/mysqld_safe"]


EXPOSE 80
# Запускаем наш сервер
#CMD ./server #$COMPILED_FILE_PATH
CMD uname -a; ("./server" 80 || echo "Solution has crashed")
