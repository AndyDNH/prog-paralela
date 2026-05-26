#include "grayscale_openmp.h"
#include <omp.h>

void grayscale_openmp(const unsigned char* input, unsigned char* output, int width, int height, int channels) {
    int img_size = width * height;
    int num_threads = omp_get_max_threads();
    int block_size = img_size / num_threads;
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * block_size;
        int end = (tid == num_threads - 1) ? img_size : start + block_size;
        for (int i = start; i < end; ++i) {
            float r = input[i * channels + 0];
            float g = input[i * channels + 1];
            float b = input[i * channels + 2];
            float a = input[i * channels + 3];
            unsigned char gray = (unsigned char)(0.21f * r + 0.72f * g + 0.07f * b);
            output[i * channels + 0] = gray;
            output[i * channels + 1] = gray;
            output[i * channels + 2] = gray;
            output[i * channels + 3] = (unsigned char)a;
        }
    }
}
