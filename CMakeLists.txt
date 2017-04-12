cmake_minimum_required(VERSION 3.1)
project (multiplexing_srv CXX)

set(CMAKE_CXX_STANDARD 11)
#find pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED FATAL_ERROR)

#find jsoncpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

set(SOURCES multiplexer.cpp main.cpp server.cpp)

#this program uses argparse for argument parsing

set(HEADERS argparse.hpp multiplexer.h server.h unixobject.h)

add_executable(multiplexing_srv ${SOURCES} ${HEADERS})

target_link_libraries(multiplexing_srv Threads::Threads "crypt" ${JSONCPP_LIBRARIES})
