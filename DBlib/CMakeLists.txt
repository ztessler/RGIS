FILE(GLOB sources src/*.C)
add_library(DB22 ${sources})
target_include_directories(DB22 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include)
