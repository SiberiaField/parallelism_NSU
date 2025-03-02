#include "matrix.hpp"


void mat::init_matrix(matrix* a, unsigned int lines, unsigned int columns){
    a->lines = lines;
    a->columns = columns;
    a->data = new double[lines * columns];
}


void mat::fill_matrix(matrix* a){
    static std::uniform_real_distribution<double> unif(-1000, 1000);
    static std::default_random_engine re;

    #pragma omp parallel
    {
        int num_of_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int lines_per_thread = a->lines / num_of_threads;
        int low = lines_per_thread * tid;
        int high = (tid == num_of_threads - 1) ? a->lines - 1 : low + lines_per_thread - 1;

        for(int i = low; i <= high; i++){
            for(int j = 0; j < a->columns; j++){
                a->data[i * a->columns + j] = unif(re);
            }
        }
    }
}


double mat::mul_parallel(matrix* a, matrix* b, matrix* c){
    const auto start{std::chrono::steady_clock::now()};
    #pragma omp parallel
    {
        int num_of_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int lines_per_thread = a->lines / num_of_threads;
        int low = lines_per_thread * tid;
        int high = (tid == num_of_threads - 1) ? a->lines - 1 : low + lines_per_thread - 1;

        for(int i = low; i <= high; i++){
            for(int j = 0; j < b->columns; j++){
                c->data[i * c->columns + j] = 0;
                for(int k = 0; k < a->columns; k++){
                    c->data[i * c->columns + j] += a->data[i * a->columns + k] * b->data[k * b->columns + j];
                }
            }
        }
    }
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    return elapsed_seconds.count();
}