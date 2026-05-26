#include "grayscale_simd.h"
#include <immintrin.h>
#include <cstring>
#include <cstdio>

void grayscale_simd(const unsigned char* input, unsigned char* output, int width, int height, int channels) {
    int img_size = width * height;
    int i = 0;
    const float r_weight = 0.21f, g_weight = 0.72f, b_weight = 0.07f;
    __m128 r_w = _mm_set1_ps(r_weight);
    __m128 g_w = _mm_set1_ps(g_weight);
    __m128 b_w = _mm_set1_ps(b_weight);
    for (; i <= img_size - 4; i += 4) {
        __m128i px = _mm_loadu_si128((__m128i*)(input + i * channels));
        float r[4], g[4], b[4], a[4];
        for (int j = 0; j < 4; ++j) {
            r[j] = input[(i + j) * channels + 0];
            g[j] = input[(i + j) * channels + 1];
            b[j] = input[(i + j) * channels + 2];
            a[j] = input[(i + j) * channels + 3];
        }
        __m128 r_v = _mm_loadu_ps(r);
        __m128 g_v = _mm_loadu_ps(g);
        __m128 b_v = _mm_loadu_ps(b);
        __m128 gray = _mm_add_ps(_mm_add_ps(_mm_mul_ps(r_v, r_w), _mm_mul_ps(g_v, g_w)), _mm_mul_ps(b_v, b_w));
        float gray_f[4];
        _mm_storeu_ps(gray_f, gray);
        for (int j = 0; j < 4; ++j) {
            unsigned char gray_val = (unsigned char)(gray_f[j]);
            output[(i + j) * channels + 0] = gray_val;
            output[(i + j) * channels + 1] = gray_val;
            output[(i + j) * channels + 2] = gray_val;
            output[(i + j) * channels + 3] = a[j];
        }
    }
    for (; i < img_size; ++i) {
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
