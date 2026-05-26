cmake_minimum_required(VERSION 3.16)

project(fltroGris)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")
# set(CMAKE_C_FLAGS) Flags para c

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)

add_executable(filtro-gris
grayscale_simd.cpp
grayscale_openmp.cpp
main.cpp
)

target_link_libraries(filtro-gris PRIVATE 
fmt::fmt
SFML::System
SFML::Graphics
SFML::Window
)
	