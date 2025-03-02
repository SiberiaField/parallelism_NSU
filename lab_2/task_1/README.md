
## Description

This program calculates multiplication of a matrix by a vector using OpenMP.
Values of matrix and vector are filled randomly.
Times of calculaiton are saved to res.csv.

## Usage

### Building

Use ```make``` to build the program.

### Running

``` C++
./matrix_mul <Num_of_threads> <Matrix_shape> <Num_of_iterations>
```

* <Num_of_threads> - Number of threads used to calculate multiplication.
* <Matrix_shape> - Shape of square matrix and vector.
* <Num_of_iterations> - Number of multiplications.

### Clearing

You can use ```make clean``` to delete object files and the program.
