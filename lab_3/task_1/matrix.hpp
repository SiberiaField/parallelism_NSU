#ifndef __MATRIX__
#define __MATRIX__


#include <memory>
#include <random>
#include <iostream>

class matrix{
private:
    std::unique_ptr<double[]> data;
    int lines;
    int columns;
public:
    matrix(int lines, int columns) noexcept;
    ~matrix() noexcept {};

    void fill(int low, int high) noexcept;
    static void mul(const matrix &a, const matrix &b, matrix &c, int low, int high);

    int get_lines() noexcept;
    int get_columns() noexcept;
};


#endif
