#ifndef SIN_VEC
#define SIN_VEC


#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>


#ifdef USE_FLOAT
    #define TYPE float
    #define TYPE_NAME "float"
#else 
    #define TYPE double
    #define TYPE_NAME "double"
#endif


class Sin_Vec{
private:
    std::vector<TYPE> data;
    double step;

    void fill();

public:
    Sin_Vec();
    ~Sin_Vec();

    TYPE sum(float* seconds);
};


#endif