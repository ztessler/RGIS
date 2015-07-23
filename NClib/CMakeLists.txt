FILE(GLOB sources src/*.c)
add_library(NC10 ${sources})
target_include_directories(NC10 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include)
