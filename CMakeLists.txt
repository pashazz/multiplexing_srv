cmake_minimum_required(VERSION 3.1)
project (multiplexing_srv CXX)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 11)
#find pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED FATAL_ERROR)

#find jsoncpp
find_package(jsoncpp REQUIRED FATAL_ERROR)
include_directories(${JSONCPP_INCLUDE_DIRS})

#use homebrew to find openssl on osx
if(APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()

#find openssl
find_package(OpenSSL REQUIRED FATAL_ERROR)
include_directories(${OPENSSL_INCLUDE_DIR})

set(SOURCES multiplexer.cpp main.cpp server.cpp postserver.cpp)
#this program uses argparse for argument parsing
set(HEADERS argparse.hpp multiplexer.h server.h unixobject.h postserver.h)



add_executable(multiplexing_srv ${SOURCES} ${HEADERS})

target_link_libraries(multiplexing_srv Threads::Threads  ${JSONCPP_LIBRARIES} OpenSSL::Crypto)
