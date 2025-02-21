## Description
This program calculates the sum of 10 million sinusoidal values ranging from 0 to 2*Pi.

You can also select the value type: float or double.
## Usage
1. Run the command bellow to build the programm
```
make USE_FLOAT=_
```
Replace `_` with `ON` or `OFF` to choose float or double respectively.

You can also run `make` without arguments. Is's the same as  `make USE_FLOAT=OFF` and `make USE_FLOAT=`.

2. Use `./main` to launch the prog

After the program is completed you should get a similiar results

| For float  | For double |
|:-------------:|:-------------:|
| SUM 807.559 | SUM 807.754 |
| TIME 0.61224 sec | TIME 0.632357 sec |
| TYPE float | TYPE double |

3. Run `make clean` if you want to delete programm and its objects files.