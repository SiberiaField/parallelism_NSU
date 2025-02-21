## Description
This program calculates the sum of 10 million sinusoidal values ranging from 0 to 2*Pi.

You can also select the value type: float or double.
## Usage
1. Run the command bellow to build a CMake project
```
cmake -B build -DUSE_FLOAT=_
```
Replace `_` with `ON` or `OFF` to choose float or double respectively.

2. Then run this command to build the program itself
```
cmake --build build
```
3. Use `./build/main` to launch the program

After the program is completed you should get a similiar results

| For float  | For double |
|:-------------:|:-------------:|
| SUM 807.559 | SUM 807.754 |
| TIME 0.606952 sec | TIME 0.601801 sec |
| TYPE float | TYPE double |
