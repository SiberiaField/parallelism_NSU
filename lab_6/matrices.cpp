#include "matrices.hpp"


void matrices::fill(matrix &mat, const double value) noexcept{
    for(int i = 0; i < mat.lines; i++){
        for(int j = 0; j < mat.columns; j++){
            mat.data[i * mat.columns + j] = value;
        }
    }
}


void matrices::random_fill(matrix& mat, const double p, const double low, const double high) noexcept{
    static std::default_random_engine re;
    static std::uniform_int_distribution<std::size_t> random_idx_distribution(0, mat.size - 1);
    static std::uniform_real_distribution<double> random_value_distribution(low, high);
    
    std::size_t random_idx, random_line, random_column;
    const std::size_t num_of_insertions = (std::size_t)(p * (double)mat.size);
    for(int insertion = 0; insertion < num_of_insertions; insertion++){
        random_idx = random_idx_distribution(re);
        random_line = random_idx % mat.lines;
        random_column = random_idx - (random_line * mat.columns);
        mat.data[random_line * mat.columns + random_column] = random_value_distribution(re);
    }
}


void matrices::reshape(matrices::matrix& mat, std::size_t lines, std::size_t columns){
    if(mat.lines * mat.columns != mat.size){
        throw std::invalid_argument("Size must not be changed: (lines * columns == this->size)");
    }
    mat.lines = lines;
    mat.columns = columns;
}


std::ostream& operator<<(std::ostream& os, const matrices::matrix& mat){
    for(int i = 0; i < mat.lines; i++){
        for(int j = 0; j < mat.columns; j++){
            os << ' ' << mat.data[i * mat.columns + j];
        }
        os << '\n';
    }
    return os;
}
