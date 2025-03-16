#ifndef __SOLVE__
#define __SOLVE__


#include <omp.h>
#include <math.h>
#include <chrono>
#include <iostream>

typedef struct matrix {
    double* data;
    int lines;
    int columns;
} matrix;

void init_matrix(matrix* a, unsigned int lines, unsigned int columns);
void fill_matrix(matrix* a);
void fill_vector(matrix* vec, int value);

double solve(matrix* a, matrix* b, matrix* x, double t, double eps);
void free_tmp();


#endif