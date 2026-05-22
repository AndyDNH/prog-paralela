#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <complex>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <omp.h>
#include <vector>

#include "escalar_vectorial.h"
#include "escalar_openmp.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::complex<double> c(-0.7, 0.27015);
uint32_t *pixel_buffer = nullptr;


static bool leer_tamanio(const std::string& prompt, size_t& value)
{
    while (true)
    {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line))
        {
            return false;
        }
        try
        {
            value = std::stoull(line);
            return true;
        }
        catch (const std::exception&)
        {
            std::cout << "Entrada no válida. Intente nuevamente.\n";
        }
    }
}

static std::vector<double> leer_consola(size_t n, const std::string &name)
{
    std::vector<double> v(n);
    std::cout << "Ingrese los " << n << " componentes del vector " << name << ":\n";
    for (size_t i = 0; i < n; ++i)
    {
        while (true)
        {
            std::cout << name << "[" << i + 1 << "] = ";
            std::string line;
            if (!std::getline(std::cin, line))
            {
                throw std::runtime_error("Error al leer la entrada.");
            }
            try
            {
                v[i] = std::stod(line);
                break;
            }
            catch (const std::exception &)
            {
                std::cout << "Valor no válido. Intente nuevamente.\n";
            }
        }
    }
    return v;
}


int main()
{
    std::cout << "Integrantes: Andres Cueva, Luis Perenguez\n";
    std::cout << "Producto escalar de vectores\n";
    std::cout << "===========================\n";

    size_t n = 0;
    if (!leer_tamanio("Ingrese el tamanio n de los vectores: ", n) || n == 0)
    {
        std::cout << "Tamanio invalido. El programa finaliza.\n";
        return 1;
    }

    std::vector<double> x;
    std::vector<double> y;

    x = leer_consola(n, "X");
    y = leer_consola(n, "Y");


    auto print_sample = [&](const std::vector<double> &v, const std::string &name)
    {
        const size_t sample = std::min<size_t>(5, v.size());
        std::cout << name << " sample: ";
        for (size_t i = 0; i < sample; ++i)
        {
            std::cout << v[i] << (i + 1 < sample ? ", " : "\n");
        }
        if (v.size() > sample)
        {
            std::cout << "(... " << (v.size() - sample) << " valores mas)\n";
        }
    };

    print_sample(x, "X");
    print_sample(y, "Y");

    const auto start1 = std::chrono::high_resolution_clock::now();
    const double result_vectorial = producto_escalar_serial(x, y);
    const auto end1 = std::chrono::high_resolution_clock::now();

    const auto start2 = std::chrono::high_resolution_clock::now();
    const double result_openmp = producto_escalar_openmp(x, y);
    const auto end2 = std::chrono::high_resolution_clock::now();

    const auto duration_vectorial = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    const auto duration_openmp = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);

    std::cout << "\nResultados:\n";
    std::cout << "1) Producto escalar usando operaciones vectoriales: " << result_vectorial << "\n";
    std::cout << "   Tiempo: " << duration_vectorial.count() << " ms\n";

    std::cout << "2) Producto escalar usando OpenMP: " << result_openmp << "\n";
    std::cout << "   Tiempo: " << duration_openmp.count() << " ms\n";

    std::cout << "\nComparacion: ";
    if (std::abs(result_vectorial - result_openmp) < 1e-9)
    {
        std::cout << "Los resultados coinciden.\n";
    }
    else
    {
        std::cout << "Hubo una diferencia numerica: " << std::abs(result_vectorial - result_openmp) << "\n";
    }

    return 0;
}
