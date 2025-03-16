FOR_OMP ?= ON

ifeq ($(FOR_OMP), ON)
	LIB_TYPE = for
else
	LIB_TYPE = parallel
endif

MAIN = main_$(LIB_TYPE)

LIB_DIR = build/solveLib_omp_$(LIB_TYPE)

$(MAIN): main.o $(LIB_DIR)/solveLib.a
	g++ -fopenmp -o $@ $^ 

main.o: main.cpp
	g++ -c -fopenmp -o $@ $^

include makelib.mk

clean:
	rm -f build/solveLib_omp_for/*.o  build/solveLib_omp_for/solveLib.a
	rm -f build/solveLib_omp_parallel/*.o  build/solveLib_omp_parallel/solveLib.a
	rm -f build/matrix.o
	rm -f main.o