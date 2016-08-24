project(radioacek)

find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
find_package (Boost COMPONENTS REQUIRED regex)


include_directories(${CMAKE_SOURCE_DIR}/include/radioacek)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES_PLAYER mainPlayer.cpp
    Connection.cpp
    UDPConnection.cpp
    TCPConnection.cpp
    BufferOverflowException.cpp
    ConnectionError.cpp
    Player.cpp
    ServerClosedError.cpp
    Timer.cpp)
set(SOURCE_FILES_MASTER mainMaster.cpp
    Connection.cpp
    UDPConnection.cpp
    TCPConnection.cpp
    BufferOverflowException.cpp
    ConnectionError.cpp
    Player.cpp
    ServerClosedError.cpp
    ParseException.cpp
    RemotePlayer.cpp
    RemotePlayerCollection.cpp
    RemotePlayerException.cpp
    Listener.cpp
    Timer.cpp)


add_executable(player ${SOURCE_FILES_PLAYER})
add_executable(master ${SOURCE_FILES_MASTER})

target_link_libraries(player ${Boost_LIBRARIES})
target_link_libraries(master ${Boost_LIBRARIES})
target_link_libraries(master pthread)
