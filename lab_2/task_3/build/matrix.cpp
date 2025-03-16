#include "solveLib.hpp"


void init_matrix(matrix* a, unsigned int lines, unsigned int columns){
    a->lines = lines;
    a->columns = columns;
    a->data = new double[lines * columns];
}


void fill_matrix(matrix* a){
    #pragma omp parallel for
    for(int i = 0; i < a->lines; i++){
        for(int j = 0; j < a->columns; j++){
            a->data[i * a->columns + j] = (i == j) ? 2.0 : 1.0;
        }
    }
}


void fill_vector(matrix* vec, int value){
    #pragma omp parallel for
    for(int i = 0; i < vec->lines; i++){
        vec->data[i] = value;
    }
}