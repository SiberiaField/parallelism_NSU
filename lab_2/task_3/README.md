## Description

This program solves System of Linear Equations via iteration method and using OpenMP library.
Times of calculaiton are saved to res.csv.

## Building

There are two variations of program, you can choose which one you want to build using this command:

```shell
make FOR_OMP=<ON, OFF>
```

Note: ```make``` is equivalent to ```make FOR_OMP=ON```

### FOR_OMP=ON

In this case you build a version of program where iteration method is splitted by many parallel sections.
Every function from ```solve_base.cpp``` is a parallel sections.

Name of the program is ```main_for```.

### FOR_OMP=OFF

In this case you build a version of program where iteration method contains only one parallel section.
Synchronization of threads is implemented with some ```#pragma omp``` instructions, e.g. ```#pragma omp barrier```.

Name of the program is ```main_parallel```.

## Running

```shell
./main_* <Num_of_threads> <Matrix_shape> <Num_of_iterations>
```

* <Num_of_threads> - Number of threads used to calculate multiplication.
* <Matrix_shape> - Shape of matrix whose solving program caclulates.
* <Num_of_iterations> - Number of multiplications.

## Clearing

You can use ```make clean``` to delete object files and libraries.
