cmake_minimum_required(VERSION 3.23)
project(miner)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

#IF (OpenSSL_FOUND)
#ENDIF()

add_executable(miner
        main.cpp sha256.cpp)

target_link_libraries(miner OpenSSL::SSL)



