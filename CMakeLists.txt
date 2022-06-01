cmake_minimum_required(VERSION 3.0.0)
project(prozac VERSION 0.1.0)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
include_directories(./)
find_library(PTHREAD pthread)
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})
set(LIB_SRC 
    ./prozac/log.cpp
    ./prozac/fiber.cpp
    ./prozac/util.cpp
    ./prozac/mutex.cpp
    ./prozac/thread.cpp
    ./prozac/scheduler.cpp
    )

set(LIBS
prozac 
pthread
yaml-cpp)

add_library(prozac SHARED ${LIB_SRC})
add_executable(tests tests/test_main.cpp)
add_executable(test_queue tests/test_queue.cpp)
add_executable(test_factory tests/test_factory.cpp)


add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber prozac)
target_link_libraries(test_fiber ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler prozac)
target_link_libraries(test_scheduler ${LIBS})

add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log prozac)
target_link_libraries(test_log ${LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)