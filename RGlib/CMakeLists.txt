FILE(GLOB sources src/*.C)
add_library(RG22 ${sources})
target_include_directories(RG22 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../DBlib/include ../CMlib/include ../MFlib/include)
