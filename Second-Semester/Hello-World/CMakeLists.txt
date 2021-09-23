cmake_minimum_required(VERSION 3.17)
project(Hello_World C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(${PROJECT_SOURCE_DIR}
        ${OPENMP_INCLUDE_DIR})

add_executable(Hello_World
        main.c)

target_link_libraries(${PROJECT_NAME}
        ${OPENMP_LIBRARIES})
