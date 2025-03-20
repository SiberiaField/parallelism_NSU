#include "solve_base.hpp"


void sub(matrix* a, matrix* b){
    #pragma omp parallel
    { 
        #pragma omp for schedule(static, 100)
        for(int i = 0; i < a->lines; i++){
            for(int j = 0; j < a->columns; j++){
                a->data[i * a->columns + j] -= b->data[i * b->columns + j];
            }
        }
    }
}


void matrix_mul(matrix* a, matrix* b, matrix* res){
    #pragma omp parallel
    { 
        #pragma omp for schedule(static, 100)
        for(int i = 0; i < a->lines; i++){
            for(int j = 0; j < b->columns; j++){
                res->data[i * res->columns + j] = 0;
                for(int k = 0; k < a->columns; k++){
                    res->data[i * res->columns + j] += a->data[i * a->columns + k] * b->data[k * b->columns + j];
                }
            }
        }
    }
}


void scalar_mul(matrix* a, double scalar){
    #pragma omp parallel
    { 
        #pragma omp for schedule(static, 100)
        for(int i = 0; i < a->lines; i++){
            for(int j = 0; j < a->columns; j++){
                a->data[i * a->columns + j] *= scalar;
            }
        }    
    }
}


double standart_deviation(matrix* a){
    double deviation = 0;

    #pragma omp parallel 
    {
        double value;
        #pragma omp for schedule(static, 100) reduction(+:deviation) 
        for(int i = 0; i < a->lines; i++){
            for(int j = 0; j < a->columns; j++){
                value = a->data[i * a->columns + j];
                deviation += value * value;
            }
        }
    }

    return sqrtf64(deviation);
}