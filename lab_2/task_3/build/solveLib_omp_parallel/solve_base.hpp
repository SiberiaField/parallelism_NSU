#ifndef __SOLVE_BASE__
#define __SOLVE_BASE__


#include "../solveLib.hpp"


void sub(matrix* a, matrix* b, int low, int high);
void matrix_mul(matrix* a, matrix* b, matrix* res, int low, int high);
void scalar_mul(matrix* a, double scalar, int low, int high);
double standart_deviation(matrix* a, int low, int high);


#endif