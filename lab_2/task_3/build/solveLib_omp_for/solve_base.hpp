#ifndef __SOLVE_BASE__
#define __SOLVE_BASE__


#include "../solveLib.hpp"


void sub(matrix* a, matrix* b);
void matrix_mul(matrix* a, matrix* b, matrix* res);
void scalar_mul(matrix* a, double scalar);
double standart_deviation(matrix* a);


#endif