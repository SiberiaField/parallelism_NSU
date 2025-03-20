#include <iostream>
#include <fstream>
#include <omp.h>

#include "build/solveLib.hpp"


double t = 1e-5;
double eps = 1e-5;


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


int main(int argc, char** argv){
    if(parse_args(argc, argv) == 1){
        std::cout << "Invalid num of args!\n";
        return 1;
    }

    omp_set_num_threads(nthreads);

    matrix a;
    init_matrix(&a, matrix_shape, matrix_shape);
    fill_matrix(&a);

    matrix b;
    init_matrix(&b, matrix_shape, 1);
    fill_vector(&b, matrix_shape + 1);

    matrix x;
    init_matrix(&x, matrix_shape, 1);
    fill_vector(&x, 0);

    double T, Tp;
    double avg_Tp = 0, avg_Sp = 0;

    omp_set_num_threads(1);
    
    std::cout << "Start solving via one thread\n";
    T = solve(&a, &b, &x, t, eps);

    omp_set_num_threads(nthreads);

    std::cout << "Start main for\n";
    for(int i = 0; i < iters; i++){
        fill_vector(&x, 0);
        Tp = solve(&a, &b, &x, t, eps);
        avg_Tp += Tp;
        avg_Sp += T / Tp;
        std::cout << " Iter " << i << " complete\n";
    }
    std::cout << "End main for\n";

    avg_Tp /= iters;
    avg_Sp /= iters;

    std::cout << "Save result\n";
    std::fstream output("res.csv", std::ios::app | std::ios::out);
    output << matrix_shape << "," << nthreads << ",";
    output << t << "," << eps << ",";
    output << T << "," << avg_Tp << "," << avg_Sp << "\n";
    output.close();

    std::cout << "Free tmp\n";
    free_tmp();
    std::cout << "Free matrices\n";
    delete[] a.data;
    delete[] b.data;
    delete[] x.data;

    return 0;
}