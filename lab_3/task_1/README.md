# Matrix Mul using std::thread

## Description

There are files to build program which calculate matrix multiplication using std::thread for speed up.

Times of calculations are saved to __res.csv__.

## Usage

### Build

To build program use ```make```. Output is program with name __matrix_mul__.

### Launch

To launch program use:

```shell
./matrix_mul <Num-of-threads> <Matrix-shape> <Num-of-iterations> 
```

### Note

You can use ```make clean``` to delete object files.
