#include "escalar_vectorial.h"
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <valarray>
#include <vector>


double producto_escalar_serial(const std::vector<double>& x, const std::vector<double>& y)
{
    if (x.size() != y.size())
    {
        throw std::invalid_argument("Los vectores deben tener la misma longitud.");
    }

    std::valarray<double> vx(x.data(), x.size());
    std::valarray<double> vy(y.data(), y.size());
    return (vx * vy).sum();

}


