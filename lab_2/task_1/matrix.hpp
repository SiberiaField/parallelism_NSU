#ifndef MATRIX
#define MATRIX


#include <random>
#include <omp.h>
#include <chrono>
#include <iostream>

namespace mat
{
    typedef struct matrix {
        double* data;
        int lines;
        int columns;
    } matrix;
    
    void init_matrix(matrix* a, unsigned int lines, unsigned int columns);
    double mul_parallel(matrix* a, matrix* b, matrix* c);
    void fill_matrix(matrix* a);
}


#endif