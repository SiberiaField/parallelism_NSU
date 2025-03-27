#include "matrix.hpp"


matrix::matrix(int lines, int columns) noexcept{
    this->lines = lines;
    this->columns = columns;
    data = std::make_unique<double[]>(lines * columns);
}


void matrix::fill(int low, int high) noexcept{
    static std::uniform_real_distribution<double> unif(-1000, 1000);
    static std::default_random_engine re;

    for(int i = low; i <= high; i++){
        for(int j = 0; j < columns; j++){
            data[i * columns + j] = unif(re);
        }
    }
}


void matrix::mul(const matrix &a, const matrix &b, matrix &res, int low, int high){
    if(a.columns != b.lines){
        throw std::invalid_argument("Matrices have a wrong shapes!");
    }

    for(int i = low; i <= high; i++){
        for(int j = 0; j < b.columns; j++){
            res.data[i * res.columns + j] = 0;
            for(int k = 0; k < a.columns; k++){
                res.data[i * res.columns + j] += a.data[i * a.columns + k] * b.data[k * b.columns + j];
            }
        }
    }
}


int matrix::get_lines() noexcept{
    return lines;
}


int matrix::get_columns() noexcept{
    return columns;
}
