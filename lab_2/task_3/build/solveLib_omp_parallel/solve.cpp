#include "solve_base.hpp"


static matrix solve_tmp = {0};


double solve(matrix* a, matrix* b, matrix* x, double t, double eps){
    if(solve_tmp.data == nullptr){
        init_matrix(&solve_tmp, a->lines, 1);
    }

    static double deviation_of_b = sqrtf64(standart_deviation(b, 0, b->lines - 1));

    double convergence = 0;

    const auto start{std::chrono::steady_clock::now()};
    #pragma omp parallel
    {
        int num_of_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int lines_per_thread = a->lines / num_of_threads;
        int low = lines_per_thread * tid;
        int high = (tid == num_of_threads - 1) ? a->lines - 1 : low + lines_per_thread - 1;

        double thread_convergence;

        while(1){
            matrix_mul(a, x, &solve_tmp, low, high);
            sub(&solve_tmp, b, low, high);

            thread_convergence = standart_deviation(&solve_tmp, low, high);

            #pragma omp atomic
            convergence += thread_convergence;
            #pragma omp barrier

            #pragma omp single
            {
                convergence = sqrtf64(convergence);
                convergence /= deviation_of_b;
            }
            
            if(convergence < eps){
                break;
            }
            #pragma omp barrier

            #pragma omp single nowait
            {
                convergence = 0;
            }

            scalar_mul(&solve_tmp, t, low, high);
            sub(x, &solve_tmp, low, high);
            #pragma omp barrier
        }
    }
    const auto end{std::chrono::steady_clock::now()};

    const std::chrono::duration<double> elapsed_seconds{end - start};
    return elapsed_seconds.count();
}


void free_tmp(){
    delete[] solve_tmp.data;
}