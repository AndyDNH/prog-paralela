#include <iostream>
#include <fmt/core.h>

#include <cuda_runtime.h>

const size_t VECTOR_SIZE = 1024*1024*1024;

extern void sumaVectores(float *a, float *b, float *c, int n);

int main(){
    // in de la tarjeta
    int deviceID = 0;
    cudaSetDevice(deviceID);

    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, deviceID);

    fmt::print("Device : {}\n", deviceProp.name);
    fmt::print("Total global memory : {} MB\n", deviceProp.totalGlobalMem/1024.0/1024.0);

    fmt::println("Multiprocessor count : {}", deviceProp.multiProcessorCount);
    fmt::println("Max threads per multiprocessor : {}", deviceProp.maxThreadsPerMultiProcessor);
    fmt::println("Max threads per block : {}", deviceProp.maxThreadsPerBlock);

    fmt::println("Max grid size : {} x {} x {}", deviceProp.maxGridSize[0], deviceProp.maxGridSize[1], deviceProp.maxGridSize[2]);


    float * h_A = new float[VECTOR_SIZE];
    float * h_B = new float[VECTOR_SIZE];
    float * h_C = new float[VECTOR_SIZE];

    for (size_t i = 0; i < VECTOR_SIZE; i++){
        h_A[i] = 1.0f;
        h_B[i] = 2.0f;
        h_C[i] = 0.0f;
    }

    float* d_A;
    float* d_B;
    float* d_C;

    size_t size_in_bytes = VECTOR_SIZE * sizeof(float);
    cudaMalloc((void**)&d_A, size_in_bytes);
    cudaMalloc((void**)&d_B, size_in_bytes);
    cudaMalloc((void**)&d_C, size_in_bytes);

    //copiar del host al device
    cudaMemcpy(d_A, h_A, size_in_bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size_in_bytes, cudaMemcpyHostToDevice);

    // invocar el kernel
    sumaVectores(d_A, d_B, d_C, VECTOR_SIZE);

    //copiar del device al host
    cudaMemcpy(h_C, d_C, size_in_bytes, cudaMemcpyDeviceToHost);

    //imprimir el resultado
    for (size_t i = 0; i < 10; i++){
        fmt::print("C[{}] = {}\n", i, h_C[i]);
    }

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    delete[] h_A;
    delete[] h_B;
    delete[] h_C;

    return 0;
}