#include "escalar_vectorial.h"

#include <vector>
#include <cstring>
#include <complex>
#include <immintrin.h>

double producto_escalar_simd(const std::vector<double> &x, const std::vector<double> &y)
{

    size_t n = x.size();

    for (int i = 0; i <= n; i += 4)
    {
        __m256d px = _mm256_loadu_pd(x.data() + i);
        __m256d py = _mm256_loadu_pd(y.data() + i);
    }
}