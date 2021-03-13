cmake_minimum_required(VERSION 3.16)
project(Parallel_Sorting)

set(CMAKE_C_STANDARD 11)

add_executable(main main.c ../merge.c ../data_generators.c)
target_link_libraries(main m)
