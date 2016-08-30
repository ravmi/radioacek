file(GLOB tests_SRC
	"*.cpp"
	)

find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
find_package (Boost COMPONENTS REQUIRED regex)
add_executable( testPlayer main.cpp ${tests_SRC} ${PLAYER_SRC})
add_test( testPlayer testPlayer )
# Link test executable against gtest & gtest_main
target_link_libraries(testPlayer gtest gtest_main)
target_link_libraries(testPlayer pthread)
