cmake_minimum_required(VERSION 3.1)

project(hlcup2017_server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(. nlohmann sqlite_orm)

set(SOURCE main.cpp storage.cpp structures.cpp)
set(HEADER server.h storae.h urlparser.h structures.h tests.h json.hpp) # sqlite_orm.h

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

#find_package(libsqlite3 REQUIRED)

find_package(Boost COMPONENTS system REQUIRED)
if(NOT Boost_FOUND)
	message(SEND_ERROR "Failed to find boost::system.")
	return()
else()
	include_directories(${Boost_INCLUDE_DIRS})
endif()


add_executable(server ${SOURCE})

target_link_libraries (server Threads::Threads ${Boost_SYSTEM_LIBRARY})

