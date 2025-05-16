#ifndef __SOLVE__
#define __SOLVE__


#include <chrono>
#include <utility>
#include <unordered_map>
#include "matrices.hpp"

void init_solving_matrix(matrices::matrix& mat, unsigned plate_shape);
void init_solving_vector(matrices::matrix& vec, unsigned plate_shape);
std::unordered_map<char, double> solve(const matrices::matrix& a, matrices::matrix& b, matrices::matrix& x, 
                                       const double step, const double eps, const unsigned iters);


#endif