cmake_minimum_required(VERSION 3.16)

project(ejemploMPI)


# set(CMAKE_C_FLAGS) Flags para c

find_package(fmt CONFIG REQUIRED)

# ---config mpi
set(MPI_ROOT "c:/oneapi/mpi/2021.18/")
set(MPI_INCLUDE "${MPI_ROOT}/include")
set(MPI_LIB "${MPI_ROOT}/lib")

include_directories(
    ${MPI_INCLUDE}
)


add_executable(ejemplo_mpi
main.cpp

)

target_link_libraries(ejemplo_mpi PRIVATE 
fmt::fmt
${MPI_LIB}/impi.lib

)


# -- matrices mult
add_executable(matrices-mult
matrices_mult.cpp

)

target_link_libraries(matrices-mult PRIVATE 
fmt::fmt
${MPI_LIB}/impi.lib

)
	