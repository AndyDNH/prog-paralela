cmake_minimum_required(VERSION 3.16)

project(fractalMPI)


# set(CMAKE_C_FLAGS) Flags para c

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)
find_package(Freetype REQUIRED)

# ---config mpi
set(MPI_ROOT "c:/oneapi/mpi/latest/")
set(MPI_INCLUDE "${MPI_ROOT}/include")
set(MPI_LIB "${MPI_ROOT}/lib")

include_directories(
    ${MPI_INCLUDE}
)


add_executable(fractal_mpi
main.cpp
fractal_serial_mpi.cpp
palette.cpp
draw_text.cpp

)

target_link_libraries(fractal_mpi PRIVATE 
fmt::fmt
SFML::System
SFML::Graphics
SFML::Window
Freetype::Freetype
${MPI_LIB}/impi.lib

)
