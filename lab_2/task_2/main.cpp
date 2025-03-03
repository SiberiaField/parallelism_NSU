#include <iostream>
#include <fstream>

#include "integrate.hpp"


unsigned int nthreads;
unsigned int iters;


int parse_args(int argc, char** argv){
    if(argc != 3){
        return 1;
    }
    nthreads = atol(argv[1]);
    iters = atol(argv[2]);
    return 0;
}


int main(int argc, char** argv){
    if(parse_args(argc, argv) == 1){
        std::cout << "Invalid num of args!\n";
        return 1;
    }

    double T, Tp;
    double avg_T = 0, avg_Tp = 0, avg_Sp = 0;

    for(int i = 0; i < iters; i++){
        omp_set_num_threads(1);
        T = integ::integrate_omp();
        avg_T += T;

        omp_set_num_threads(nthreads);
        Tp = integ::integrate_omp();
        avg_Tp += Tp;

        avg_Sp += T / Tp;
    }

    avg_T /= iters;
    avg_Tp /= iters;
    avg_Sp /= iters;

    std::fstream output("res.csv", std::ios::app | std::ios::out);
    output << nthreads << "," << avg_T << "," << avg_Tp << "," << avg_Sp << "\n";
    output.close();

    return 0;
}