set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include ../NClib/include)

foreach(loop_var nc2table ncAggregate ncAttributes ncGridResample ncGridSampling ncTblCalculate)
	add_executable(${loop_var} src/NCC${loop_var}.c)
	if(${CMAKE_HOST_APPLE})
		target_link_libraries(${loop_var} LINK_PUBLIC CM10 NC10 -lnetcdf -ludunits -lm)
	else() # TODO This is truly ugly. Macport removed the backward compatibility of the udunits2 libraries.
		target_link_libraries(${loop_var} LINK_PUBLIC CM10 NC10 -lnetcdf -ludunits2 -lm)
	endif(${CMAKE_HOST_APPLE})
	install (TARGETS ${loop_var} RUNTIME DESTINATION ghaas/bin)
endforeach(loop_var)
