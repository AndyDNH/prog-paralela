cmake_minimum_required(VERSION 3.16)

project(EjemploOpenMp)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp")
# set(CMAKE_C_FLAGS) Flags para c

find_package(fmt CONFIG REQUIRED)

add_executable(ejemplo-openmp
main.cpp

)

target_link_libraries(ejemplo-openmp PRIVATE 
fmt::fmt


)
	