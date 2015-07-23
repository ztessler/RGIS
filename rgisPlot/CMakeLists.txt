FILE(GLOB sources src/*.C)
add_executable(rgisPlot EXCLUDE_FROM_ALL ${sources})
target_link_libraries(rgisPlot DB22 CM10 -lcpgplot -lpgplot -lpng -lX11 -lm -ludunits2 -lnetcdf)
target_include_directories(rgisPlot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../DBlib/include ../CMlib/include)
