cmake_minimum_required(VERSION 3.1)

project(hlcup2017_server)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(. nlohmann sqlite_orm sqlite_modern_cpp/hdr)

set(SOURCE main.cpp jsonstorage.cpp sqlitestorage.cpp structures.cpp)
set(HEADER server.h storae.h urlparser.h structures.h tests.h json.hpp sqlite_modern_cpp.h) # sqlite_orm.h

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

find_package(sqlite3 REQUIRED)
if (SQLITE3_FOUND)
    include_directories(${SQLITE3_INCLUDE_DIRS})
else()
    message(SEND_ERROR "Failed to find sqlite3")
    return()
endif (SQLITE3_FOUND)


find_package(Boost COMPONENTS system REQUIRED)
if(NOT Boost_FOUND)
	message(SEND_ERROR "Failed to find boost::system.")
	return()
else()
	include_directories(${Boost_INCLUDE_DIRS})
endif()



add_executable(server ${SOURCE})

target_link_libraries (server Threads::Threads ${Boost_SYSTEM_LIBRARY} ${SQLITE3_LIBRARIES})
