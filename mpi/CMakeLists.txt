cmake_minimum_required(VERSION 3.27)
project(RPP_2)

set(CMAKE_CXX_STANDARD 17)
find_package(MPI REQUIRED)
if(MPI_CXX_FOUND)
    message("MPI")
else()
    message("Fail")
endif ()

add_executable(RPP_2 mpi_main.cpp)
target_link_libraries(RPP_2 PUBLIC MPI::MPI_CXX)