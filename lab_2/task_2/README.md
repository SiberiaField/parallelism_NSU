
## Description

This program calculates integral of some function using OpenMP.
Times of calculation are saved to res.csv.

## Usage

### Building

Use ```make``` to build the program.

### Running

``` C++
./integrate <Num_of_threads> <Num_of_iterations>
```

* <Num_of_threads> - Number of threads used to calculate integral.
* <Num_of_iterations> - Number of calculations.

### Clearing

You can use ```make clean``` to delete object files and the program.
