#include "integrate.hpp"


double func(double x){
    return exp(-x * x);
}


double integ::integrate_omp(){
    static double a = -4, b = 4; 
    static int num_of_steps = 40000000;
    static double h = (b - a) / num_of_steps;
    double res_sum = 0;

    const auto start{std::chrono::steady_clock::now()};
    #pragma omp parallel
    {
        int num_of_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int steps_per_thread = num_of_steps / num_of_threads;
        int low = steps_per_thread * tid;
        int high = (tid == num_of_threads - 1) ? num_of_steps - 1 : low + steps_per_thread - 1;
        
        double sum = 0;
        for(int i = low; i <= high; i++){
            sum += func(a + h * (i + 0.5));
        }
        sum *= h;

        #pragma omp atomic
        res_sum += sum;
    }
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    return elapsed_seconds.count();
}