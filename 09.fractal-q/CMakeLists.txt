cmake_minimum_required(VERSION 3.16)

project(cuda01)


find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)

enable_language(CXX CUDA)


include_directories(
    ${MPI_INCLUDE}
)


add_executable(fractal-cuda
    main.cpp
    palette.cpp
    kernel.cu
)

target_include_directories(fractal-cuda PRIVATE 
    ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

target_link_libraries(fractal-cuda PRIVATE 
    fmt::fmt
    SFML::System
    SFML::Graphics
    SFML::Window    

)
