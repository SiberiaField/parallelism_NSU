#include "solve.hpp"


void proceed_one_iteration(matrices::matrix& mat, const unsigned plate_shape, const int diag_idx, int zeros, const bool not_submatrix_end){
    // fill diagonal elements
    int shift = 1;
    mat.at(diag_idx, diag_idx) = -4;
    if(not_submatrix_end){
        mat.at(diag_idx + 1, diag_idx) = mat.at(diag_idx, diag_idx + 1) = 1;
        shift = 2;
    }

    // fill entire zeros and put ones after
    if(zeros != 0){
        if(not_submatrix_end == false){
            zeros++;
        }
        for(; zeros > 0; zeros--, shift++){
            mat.at(diag_idx + shift, diag_idx) = mat.at(diag_idx, diag_idx + shift) = 0;
        }
        mat.at(diag_idx + shift, diag_idx) = mat.at(diag_idx, diag_idx + shift) = 1;
        shift += 1;
    }

    // fill other zeros in line and column
    for(; diag_idx + shift < mat.get_lines(); shift++){
        mat.at(diag_idx + shift, diag_idx) = mat.at(diag_idx, diag_idx + shift) = 0;
    }
}


void init_solving_matrix(matrices::matrix& mat, const unsigned plate_shape){
    mat.reshape(plate_shape * plate_shape, plate_shape * plate_shape);

    int zeros = plate_shape - 2;
    int submatrix_end, diag_idx;
    for(int submatrix_idx = 0; submatrix_idx < plate_shape; submatrix_idx++){
        if(submatrix_idx == plate_shape - 1){
            zeros = 0;
        }
        submatrix_end = (submatrix_idx + 1) * plate_shape - 1;
        diag_idx = submatrix_idx * plate_shape;

        for(; diag_idx < submatrix_end; diag_idx++){
            proceed_one_iteration(mat, plate_shape, diag_idx, zeros, true);
        }
        proceed_one_iteration(mat, plate_shape, diag_idx, zeros, false);
    }
}


double linear_interpolation(const int x0, const double y0, const int x1, const double y1, const int x){
    return (y0 * (x1 - x) + y1 * (x - x0)) / (x1 - x0);
}


void fill_border_line(matrices::matrix& vec, const int line_idx, const double y0, const double y1, const unsigned plate_shape){
    vec.at(line_idx, 0) = y0;
    vec.at(line_idx, plate_shape - 1) = y1;
    for(int i = 1; i < plate_shape - 1; i++){
        vec.at(line_idx, i) = linear_interpolation(0, y0, plate_shape - 1, y1, i);
    }
}


void fill_border_column(matrices::matrix& vec, const int column_idx, const double y0, const double y1, const unsigned plate_shape){
    vec.at(0, column_idx) = y0;
    vec.at(plate_shape - 1, column_idx) = y1;
    for(int i = 1; i < plate_shape - 1; i++){
        vec.at(i, column_idx) = linear_interpolation(0, y0, plate_shape - 1, y1, i);
    }
}


void init_solving_vector(matrices::matrix& vec, const unsigned plate_shape){
    vec.reshape(plate_shape, plate_shape);
    vec.fill(0);
    fill_border_line(vec, 0, 10, 20, plate_shape);
    fill_border_column(vec, plate_shape - 1, 20, 30, plate_shape);
    fill_border_line(vec, plate_shape - 1, 30, 20, plate_shape);
    fill_border_column(vec, 0, 10, 20, plate_shape);
    vec.reshape(vec.get_size(), 1);
}


std::unordered_map<char, double> solve(const matrices::matrix& a, matrices::matrix& b, matrices::matrix& x, 
                                       const double step, const double eps, const unsigned iters){

    matrices::matrix solve_tmp(a.get_lines(), 1);
    double deviation_of_b = b.standart_deviation();
    
    double convergence;
    int proceeded_iters;
    
    
    #pragma acc enter data copyin(step)
    const auto start{std::chrono::steady_clock::now()};
    for(proceeded_iters = 0; proceeded_iters < iters; proceeded_iters++){
        solve_tmp.mul(a, x);
        solve_tmp -= b;
            
        convergence = solve_tmp.standart_deviation() / deviation_of_b;
        if(convergence < eps){
            break;
        }
    
        solve_tmp *= step;
        x -= solve_tmp;
    
        if(proceeded_iters % 1000 == 0){
            std::cout << "\nIters: " << proceeded_iters;
        }
    }
    const auto end{std::chrono::steady_clock::now()};
    #pragma acc exit data delete(step)
    const std::chrono::duration<double> elapsed_seconds{end - start};

    static std::unordered_map<char, double> results;
    results.insert({'t', elapsed_seconds.count()});
    results.insert({'i', proceeded_iters});
    results.insert({'e', convergence});
    return results;
}
