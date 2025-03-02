#include <chrono>
#include <omp.h>
#include <iostream>
#include <fstream>

#include "matrix.hpp"


unsigned int nthreads;
unsigned int matrix_shape;
unsigned int iters;


int parse_args(int argc, char** argv){
    if(argc != 4){
        return 1;
    }
    nthreads = atol(argv[1]);
    matrix_shape = atol(argv[2]);
    iters = atol(argv[3]);
    return 0;
}


int main(int argc, char **argv){
    if(parse_args(argc, argv) == 1){
        std::cout << "Invalid num of args!\n";
        return 1;
    }

    double T, Tp;
    double avg_T = 0, avg_Tp = 0, avg_Sp = 0;

    omp_set_num_threads(nthreads);

    mat::matrix a, b, c;

    mat::init_matrix(&a, matrix_shape, matrix_shape);
    mat::fill_matrix(&a);

    mat::init_matrix(&b, matrix_shape, 1);
    mat::fill_matrix(&b);

    mat::init_matrix(&c, a.lines, b.columns);

    for(int i = 0; i < iters; i++){
        omp_set_num_threads(1);
        T = mat::mul_parallel(&a, &b, &c);
        avg_T += T;

        omp_set_num_threads(nthreads);
        Tp = mat::mul_parallel(&a, &b, &c);
        avg_Tp += Tp;
        
        avg_Sp += T / Tp;
    }

    avg_T /= iters;
    avg_Tp /= iters;
    avg_Sp /= iters;

    std::fstream output("res.csv", std::ios::app | std::ios::out);
    output << matrix_shape << "," << nthreads << "," <<
        avg_T << "," << avg_Tp << "," << avg_Sp << "\n";
    output.close();

    delete[] a.data;
    delete[] b.data;
    delete[] c.data;

    return 0;
}