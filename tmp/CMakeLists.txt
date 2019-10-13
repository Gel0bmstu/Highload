cmake_minimum_required(VERSION 3.0.0)
project(highload C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin)

find_package(Threads)

add_executable(highload
        ../src/main.c
        ../src/logger.c

        ../headers/serve.h 
        ../headers/logger.h)

target_link_libraries(highload event)
target_link_libraries(highload ${CMAKE_THREAD_LIBS_INIT} )
