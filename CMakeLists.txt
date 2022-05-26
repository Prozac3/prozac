cmake_minimum_required(VERSION 3.0.0)
project(prozac VERSION 0.1.0)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O2 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
include_directories(./)
set(LIB_SRC 
    ./prozac/fiber.cpp
    ./prozac/util.cpp)

add_library(prozac SHARED ${LIB_SRC})
add_executable(tests tests/test_main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)