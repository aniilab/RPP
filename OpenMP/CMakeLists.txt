cmake_minimum_required(VERSION 3.27)
project(RPP_1)

add_executable(RPP_1 open_mp_main.cpp)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message("OpenMp found")
    target_link_libraries(RPP_1 OpenMP::OpenMP_CXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
else()
    message("Not found")
    target_link_libraries(RPP_1)
endif ()
