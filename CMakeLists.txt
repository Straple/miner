cmake_minimum_required(VERSION 3.23)
project(miner)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ROOT "C:/Program Files/boost/boost_1_82_0")

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

#IF (OpenSSL_FOUND)
#ENDIF()

add_executable(miner
        main.cpp sha256/openssl_sha256.cpp bits_manipulation.cpp block.cpp utils.cpp pool_client.cpp miner.cpp logger.cpp)

target_link_libraries(miner OpenSSL::SSL)

target_link_libraries(miner ${Boost_LIBRARIES})

