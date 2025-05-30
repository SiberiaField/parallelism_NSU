#ifndef __MATRICES__
#define __MATRICES__


#include <memory>
#include <random>
#include <iostream>

namespace matrices {
    typedef struct matrix{
        std::size_t size;
        std::size_t lines;
        std::size_t columns;
        double* data;
    } matrix;

    void fill(matrix& mat, const double value) noexcept;
    void random_fill(matrix& mat, const double p, const double low, const double high) noexcept;
    void reshape(matrix& mat, std::size_t lines, std::size_t columns);
};


std::ostream& operator<<(std::ostream& os, const matrices::matrix& mat);


#endif
