integrate: main.o integrate.o
	g++ -fopenmp main.o integrate.o -o integrate


main.o: main.cpp
	g++ -c -fopenmp main.cpp -o main.o

integrate.o: integrate.cpp integrate.hpp
	g++ -c -fopenmp integrate.cpp


clean:
	rm integrate main.o integrate.o