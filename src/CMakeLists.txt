add_executable(player main/player.cpp ${PLAYER_SRC})
set_target_properties( player PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
add_executable(master main/master.cpp ${MASTER_SRC})
set_target_properties( master PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

target_link_libraries(player ${Boost_LIBRARIES})
target_link_libraries(master ${Boost_LIBRARIES})
target_link_libraries(master pthread)
