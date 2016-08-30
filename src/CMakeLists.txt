add_executable(player main/main_player.cpp ${PLAYER_SRC})
set_target_properties( player PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
add_executable(master main/main_master.cpp ${MASTER_SRC})
set_target_properties( master PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

target_link_libraries(player ${Boost_LIBRARIES})
target_link_libraries(master ${Boost_LIBRARIES})
target_link_libraries(master pthread)
target_link_libraries(player pthread)
