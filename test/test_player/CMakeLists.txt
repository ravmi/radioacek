file(GLOB tests_SRC
	"*.cpp"
	)

add_executable( testPlayer main.cpp ${tests_SRC} ${PLAYER_SRC})
add_test( testPlayer testPlayer )
# Link test executable against gtest & gtest_main
target_link_libraries(testPlayer gtest gtest_main)
target_link_libraries(testPlayer pthread)
