cmake_minimum_required(VERSION 3.16)

project(cuda01)


find_package(fmt CONFIG REQUIRED)

enable_language(CXX CUDA)


include_directories(
    ${MPI_INCLUDE}
)


add_executable(cuda01
    main.cpp
    kernel.cu
)

target_include_directories(cuda01 PRIVATE 
    ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

target_link_libraries(cuda01 PRIVATE 
    fmt::fmt    

)
