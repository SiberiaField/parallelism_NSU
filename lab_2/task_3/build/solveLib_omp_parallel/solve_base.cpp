#include "solve_base.hpp"


void sub(matrix* a, matrix* b, int low, int high){
    for(int i = low; i <= high; i++){
        for(int j = 0; j < a->columns; j++){
            a->data[i * a->columns + j] -= b->data[i * b->columns + j];
        }
    }
}


void matrix_mul(matrix* a, matrix* b, matrix* res, int low, int high){
    for(int i = low; i <= high; i++){
        for(int j = 0; j < b->columns; j++){
            res->data[i * res->columns + j] = 0;
            for(int k = 0; k < a->columns; k++){
                res->data[i * res->columns + j] += a->data[i * a->columns + k] * b->data[k * b->columns + j];
            }
        }
    }
}


void scalar_mul(matrix* a, double scalar, int low, int high){
    for(int i = low; i <= high; i++){
        for(int j = 0; j < a->columns; j++){
            a->data[i * a->columns + j] *= scalar;
        }
    }
}


double standart_deviation(matrix* a, int low, int high){
    double deviation = 0;
    double value;

    for(int i = low; i <= high; i++){
        for(int j = 0; j < a->columns; j++){
            value = a->data[i * a->columns + j];
            deviation += value * value;
        }
    }
    
    return deviation;
}