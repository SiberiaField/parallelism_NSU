#include "matrices.hpp"


matrices::matrix::matrix() noexcept{
    this->lines = this->columns = this->size = 0;
    this->data = nullptr;
    #pragma acc enter data copyin(this)
}


matrices::matrix::matrix(std::size_t lines, std::size_t columns){
    this->lines = lines;
    this->columns = columns;
    size = lines * columns;
    data = new double[size];
    #pragma acc enter data copyin(this)
    #pragma acc enter data create(data[0:size])
}


matrices::matrix::~matrix() noexcept{
    #pragma acc exit data delete(data)
    #pragma acc exit data delete(this)
    delete data;
    data = nullptr;
    lines = columns = size = 0;
}


#pragma acc routine seq
double& matrices::matrix::at(std::size_t i, std::size_t j){
    if(i >= lines || j >= columns){
        throw std::out_of_range("Indices out of ranges");
    }

    return data[i * columns + j];
}


#pragma acc routine seq
const double& matrices::matrix::at(std::size_t i, std::size_t j) const{
    if(i >= lines || j >= columns){
        throw std::out_of_range("Indices out of ranges");
    }

    return data[i * columns + j];
}


void matrices::matrix::init(std::size_t lines, std::size_t columns){
    this->lines = lines;
    this->columns = columns;
    size = lines * columns;
    data = new double[size];
    #pragma acc update device(this)
    #pragma acc enter data create(data[0:size])
}


void matrices::matrix::fill(const double value) noexcept{
    #pragma acc parallel loop
    for(int i = 0; i < lines; i++){
        #pragma acc loop
        for(int j = 0; j < columns; j++){
            data[i * columns + j] = value;
        }
    }
}


void matrices::matrix::random_fill(const double p, const double low, const double high) noexcept{
    static std::default_random_engine re;
    static std::uniform_int_distribution<std::size_t> random_idx_distribution(0, size - 1);
    static std::uniform_real_distribution<double> random_value_distribution(low, high);
    
    std::size_t random_idx, random_line, random_column;
    const std::size_t num_of_insertions = (std::size_t)(p * (double)size);
    for(int insertion = 0; insertion < num_of_insertions; insertion++){
        random_idx = random_idx_distribution(re);
        random_line = random_idx % lines;
        random_column = random_idx - (random_line * columns);
        this->at(random_line, random_column) = random_value_distribution(re);
    }
}


void matrices::matrix::reshape(std::size_t lines, std::size_t columns){
    if(lines * columns != size){
        throw std::invalid_argument("Size must not be changed: (lines * columns == this->size)");
    }
    this->lines = lines;
    this->columns = columns;
}


void matrices::matrix::mul(const matrices::matrix &a, const matrices::matrix &b){
    if(a.columns != b.lines){
        throw std::invalid_argument("Matrices have a wrong shapes!");
    }

    double tmp;
    #pragma acc enter data create(tmp)
    #pragma acc parallel loop
    for(int i = 0; i < a.lines; i++){
        #pragma acc loop
        for(int j = 0; j < b.columns; j++){
            tmp = 0;
            #pragma acc loop reduction(+:tmp)
            for(int k = 0; k < a.columns; k++){
                tmp += a.data[i * a.columns + k] * b.data[k * b.columns + j];
            }
            data[i * columns + j] = tmp;
        }
    }
    #pragma acc exit data delete(tmp)
}


double matrices::matrix::standart_deviation(){
    double deviation = 0;

    #pragma acc parallel loop reduction(+:deviation)
    for(int i = 0; i < this->get_lines(); i++){
        #pragma acc loop reduction(+:deviation)
        for(int j = 0; j < this->get_columns(); j++){
            deviation += this->at(i, j) * this->at(i, j);
        }
    }

    return sqrtf64(deviation);
}


matrices::matrix& matrices::matrix::operator-=(const matrices::matrix& b){
    #pragma acc parallel loop
    for(int i = 0; i < lines; i++){
        #pragma acc loop
        for(int j = 0; j < columns; j++){
            data[i *columns + j] -= b.data[i * b.columns + j];
        }
    }
    return *this;
}


matrices::matrix& matrices::matrix::operator*=(const double scalar){
    #pragma acc parallel loop
    for(int i = 0; i < lines; i++){
        #pragma acc loop
        for(int j = 0; j < columns; j++){
            data[i * columns + j] *= scalar;
        }
    }
    return *this;
}


#pragma acc routine seq
std::size_t matrices::matrix::get_lines() const noexcept{
    return lines;
}


#pragma acc routine seq
std::size_t matrices::matrix::get_columns() const noexcept{
    return columns;
}


#pragma acc routine seq
std::size_t matrices::matrix::get_size() const noexcept{
    return size;
}


void matrices::matrix::update_host(){   
    #pragma acc update self(this)
    #pragma acc update self(data[0:size])
}


void matrices::matrix::update_device(){
    #pragma acc update device(this)
    #pragma acc update device(data[0:size])
}


std::ostream& operator<<(std::ostream& os, const matrices::matrix& mat){
    for(int i = 0; i < mat.get_lines(); i++){
        for(int j = 0; j < mat.get_columns(); j++){
            os << ' ' << mat.at(i, j);
        }
        os << '\n';
    }
    return os;
}
