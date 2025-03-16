$(LIB_DIR)/solveLib.a: build/matrix.o $(LIB_DIR)/solve_base.o $(LIB_DIR)/solve.o
	ar -rcs $@ $^

build/matrix.o: build/matrix.cpp
	g++ -c -fopenmp -o $@ $^

$(LIB_DIR)/solve_base.o: $(LIB_DIR)/solve_base.cpp
	g++ -c -fopenmp -o $@ $^

$(LIB_DIR)/solve.o: $(LIB_DIR)/solve.cpp
	g++ -c -fopenmp -o $@ $^