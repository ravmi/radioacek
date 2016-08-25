project(radioacek)

find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
find_package (Boost COMPONENTS REQUIRED regex)


include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
file(GLOB PLAYER_SRC
    "*.cpp")
file(GLOB MASTER_SRC
    "*.cpp")

add_executable(player main/player.cpp ${PLAYER_SRC})
set_target_properties( player PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
add_executable(master main/master.cpp ${MASTER_SRC})
set_target_properties( master PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

target_link_libraries(player ${Boost_LIBRARIES})
target_link_libraries(master ${Boost_LIBRARIES})
target_link_libraries(master pthread)
