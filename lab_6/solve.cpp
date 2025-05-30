#include "solve.hpp"


double linear_interpolation(const int x0, const double y0, const int x1, const double y1, const int x){
    return (y0 * (x1 - x) + y1 * (x - x0)) / (x1 - x0);
}


void fill_border_line(matrices::matrix& vec, const int line_idx, const double y0, const double y1, const unsigned plate_shape){
    vec.data[line_idx * vec.columns] = y0;
    vec.data[line_idx * vec.columns + (plate_shape - 1)] = y1;
    for(int i = 1; i < plate_shape - 1; i++){
        vec.data[line_idx * vec.columns + i] = linear_interpolation(0, y0, plate_shape - 1, y1, i);
    }
}


void fill_border_column(matrices::matrix& vec, const int column_idx, const double y0, const double y1, const unsigned plate_shape){
    vec.data[column_idx] = y0;
    vec.data[(plate_shape - 1) * vec.columns + column_idx] = y1;
    for(int i = 1; i < plate_shape - 1; i++){
        vec.data[i * vec.columns + column_idx] = linear_interpolation(0, y0, plate_shape - 1, y1, i);
    }
}


void init_solving_vector(matrices::matrix& vec, const unsigned plate_shape){
    matrices::reshape(vec, plate_shape, plate_shape);
    matrices::fill(vec, 0);
    fill_border_line(vec, 0, 10, 20, plate_shape);
    fill_border_column(vec, plate_shape - 1, 20, 30, plate_shape);
    fill_border_line(vec, plate_shape - 1, 30, 20, plate_shape);
    fill_border_column(vec, 0, 10, 20, plate_shape);
    matrices::reshape(vec, vec.size, 1);
}
