matrix_mul: main.o matrix.o
	g++ -o $@ $^

main.o: main.cpp
	g++ -c -o $@ $^

matrix.o: matrix.cpp
	g++ -c -o $@ $^

clean:
	rm -f *.o
