cmake_minimum_required(VERSION 3.16)

project(fractalJulia)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")
# set(CMAKE_C_FLAGS) Flags para c

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS System Graphics Window Network CONFIG REQUIRED)

add_executable(fractal-julia
fractal_serial.cpp
main.cpp
palette.cpp
fractal_simd.cpp
fractal_openmp.cpp
escalar_vectorial.cpp
)

target_link_libraries(fractal-julia PRIVATE 
fmt::fmt
SFML::System
SFML::Graphics
SFML::Window
SFML::Network

)
	