#include "solve_base.hpp"


static matrix solve_tmp = {0};


double solve(matrix* a, matrix* b, matrix* x, double t, double eps){
    if(solve_tmp.data == nullptr){
        init_matrix(&solve_tmp, a->lines, 1);
    }

    static double deviation_of_b = standart_deviation(b);
    double convergence;

    const auto start{std::chrono::steady_clock::now()};
    while(1){
        matrix_mul(a, x, &solve_tmp);
        sub(&solve_tmp, b);

        convergence = standart_deviation(&solve_tmp) / deviation_of_b;
        if(convergence < eps){
            break;
        }

        scalar_mul(&solve_tmp, t);
        sub(x, &solve_tmp);
    }
    const auto end{std::chrono::steady_clock::now()};

    const std::chrono::duration<double> elapsed_seconds{end - start};
    return elapsed_seconds.count();
}


void free_tmp(){
    delete[] solve_tmp.data;
}