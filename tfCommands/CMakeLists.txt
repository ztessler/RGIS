set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include ../NClib/include)

foreach(loop_var FAccumulate FCut FSort FStat)
	add_executable(${loop_var} src/${loop_var}.c)
	target_link_libraries(${loop_var} LINK_PUBLIC CM10 DB22 F10 -lm)
	install (TARGETS ${loop_var} RUNTIME DESTINATION ghaas/bin)
endforeach(loop_var)
