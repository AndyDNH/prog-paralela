#include "escalar_vectorial.h"

#include <vector>
#include <cstring>
#include <complex>
#include <immintrin.h>

double producto_escalar_simd(const std::vector<double> &x, const std::vector<double> &y)
{

    size_t n = x.size();
    size_t i = 0;

    __m256d acumulado = _mm256_setzero_pd();

    size_t limite_simd = n - (n % 4);

    for (; i <= n; i += 4)
    {
        __m256d px = _mm256_loadu_pd(x.data() + i);
        __m256d py = _mm256_loadu_pd(y.data() + i);

        __m256d oper = _mm256_mul_pd(px, py);

        acumulado = _mm256_add_pd(acumulado, oper);
    }

    double res_simd[4];
    _mm256_storeu_pd(res_simd, acumulado);

    double resultado = res_simd[0] + res_simd[1] + res_simd[2] + res_simd[3];

    for (; i < n; i++)
    {
        resultado += x[i] * y[i];
    }
    return resultado;
}