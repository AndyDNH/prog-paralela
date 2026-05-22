#include "escalar_openmp.h"
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <valarray>
#include <vector>

#include <omp.h>

double producto_escalar_openmp(const std::vector<double> &x, const std::vector<double> &y)
{
    if (x.size() != y.size())
    {
        throw std::invalid_argument("Los vectores deben tener la misma longitud.");
    }

    const size_t n = x.size();
    double result = 0.0;

#pragma omp parallel
    {
        int thread_count = omp_get_num_threads();
        int thread_id = omp_get_thread_num();


        size_t chunk_size = (n + thread_count - 1) / thread_count;
        size_t start = thread_id * chunk_size;
        size_t end = std::min(start + chunk_size, n);


        double local_sum = 0.0;
        for (size_t i = start; i < end; ++i)
        {
            local_sum += x[i] * y[i];
        }


#pragma omp master
        {
            result += local_sum;
        }
    }

    return result;
}
