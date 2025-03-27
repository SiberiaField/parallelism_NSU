#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include <vector>

#include "matrix.hpp"


int nthreads;
int matrix_shape;
int iters;
std::mutex mut;


double parallel_mul(int num_of_threads){
    matrix a(matrix_shape, matrix_shape);
    matrix b(matrix_shape, 1);
    matrix c(matrix_shape, 1);

    int low, high;
    int lines_per_thread = a.get_lines() / num_of_threads;
    int barrier_counter = num_of_threads;
    double memory_alloc_time = 0;
    std::vector<std::thread> threads;

    const auto start{std::chrono::steady_clock::now()};
    for(int i = 0; i < num_of_threads; i++){
        low = i * lines_per_thread;
        high = (i == num_of_threads - 1) ? a.get_lines() - 1 : low + lines_per_thread - 1;
        threads.push_back(std::thread(
            [low, high, &a, &b, &c, &barrier_counter, &memory_alloc_time]{
                const auto start{std::chrono::steady_clock::now()};
                a.fill(low, high);
                b.fill(low, high);
                const auto end{std::chrono::steady_clock::now()};
                const std::chrono::duration<double> elapsed_seconds{end - start};
                {
                    std::lock_guard<std::mutex> lock{mut};
                    memory_alloc_time += elapsed_seconds.count();
                    barrier_counter--;
                }
                while(barrier_counter != 0);

                matrix::mul(a, b, c, low, high);
            }
        ));
    }

    for(auto t_iter = threads.begin(); t_iter != threads.end(); ++t_iter){
        t_iter->join();
    }
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    threads.clear();
    return elapsed_seconds.count() - (memory_alloc_time / num_of_threads);
}


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
    if(parse_args(argc, argv)){
        std::cerr << "Wrong number of arguments!\n";
        exit(1);
    }

    std::cout << "Start calculations\n";
    double T, Tp;
    double avg_Tp = 0, avg_Sp = 0;
    T = parallel_mul(1);
    for(int i = 0; i < iters; i++){
        Tp = parallel_mul(nthreads);
        avg_Tp += Tp;
        avg_Sp += T / Tp;
        std::cout << "  Iter " << i + 1 << "\n";
    }
    std::cout << "End calculations\n";
    avg_Tp /= iters;
    avg_Sp /= iters;

    std::cout << "Save results\n";
    std::fstream output("res.csv", std::ios::app | std::ios::out);
    output << matrix_shape << "," << nthreads << ",";
    output << T << "," << avg_Tp << "," << avg_Sp << "\n";
    output.close();

    return 0;
}
