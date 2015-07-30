set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include ../NClib/include)

foreach(loop_var nc2table ncAggregate ncAttributes ncGridResample ncGridSampling ncTblCalculate)
	add_executable(${loop_var} src/NCC${loop_var}.c)
	target_link_libraries(${loop_var} LINK_PUBLIC CM10 NC10 -lnetcdf -ludunits2 -lm)
	install (TARGETS ${loop_var} RUNTIME DESTINATION ghaas/bin)
endforeach(loop_var)
