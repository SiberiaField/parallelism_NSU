#include "sin_vec.hpp"


void Sin_Vec::fill(){
    double curr_x = 0;
    for(int i = 0; i < data.size(); i++){
        data[i] = sin(curr_x);
        curr_x += step;
    }
}


Sin_Vec::Sin_Vec(){
    data.resize(100000000);
    step = 6.28e-8;
    fill();
}


Sin_Vec::~Sin_Vec(){
    data.clear();
}


TYPE Sin_Vec::sum(float* seconds){
    clock_t start = clock();
    TYPE sum = 0;

    for(auto val : data){
        sum += val;
    }
    clock_t end = clock();

    *seconds = (float)(end - start) / CLOCKS_PER_SEC;

    return sum;
}