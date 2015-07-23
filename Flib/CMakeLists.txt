FILE(GLOB sources src/*.c)
add_library(F10 ${sources})
target_include_directories(F10 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include)
