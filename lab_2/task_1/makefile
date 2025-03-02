matrix_mul: main.o matrix.o
	g++ -fopenmp main.o matrix.o -o matrix_mul


main.o: main.cpp
	g++ -c -fopenmp main.cpp -o main.o

matrix.o: matrix.cpp matrix.hpp
	g++ -c -fopenmp matrix.cpp


clean:
	rm matrix_mul main.o matrix.o