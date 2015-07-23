FILE(GLOB sources src/*.C)
add_library(UI22 ${sources})
target_include_directories(UI22 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../DBlib/include ../CMlib/include)
