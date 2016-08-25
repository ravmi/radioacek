file(GLOB tests_SRC
	"*.cpp"
	)

add_executable( testMaster main.cpp ${tests_SRC})
add_test( testMaster testMaster )
# Link test executable against gtest & gtest_main
target_link_libraries(testMaster gtest gtest_main)
target_link_libraries(testMaster pthread)
