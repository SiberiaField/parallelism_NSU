TYPE ?= double

all: main test


main: main.o
	g++ -o $@ $^ 

main.o: main.cpp client.hpp server.hpp funcs.hpp
	g++ -c -DTYPE=$(TYPE) -o $@ main.cpp


test: test.o
	g++ -o $@ $^

test.o: test.cpp funcs.hpp
	g++ -c -DTYPE=$(TYPE) -o $@ test.cpp


clean:
	rm -f *.o
