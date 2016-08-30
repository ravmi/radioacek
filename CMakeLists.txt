cmake_minimum_required(VERSION 2.8.12.2)
project(radioacek)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -g")

file(GLOB PLAYER_SRC
    "src/*.cpp")
file(GLOB MASTER_SRC
    "src/*.cpp")
find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
find_package (Boost COMPONENTS REQUIRED regex)




include_directories(${CMAKE_SOURCE_DIR}/include)


enable_testing()
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(src)
#find_package(GTest QUIET)
#if (GTest_FOUND)
#    add_subdirectory(test)
#else()
message(WARNING "GTest not found - skipping building tests")
#endif ()
