CC = pgc++
DEVICE = gpu
CXXFLAGS = -acc=$(DEVICE) -fast

all: main_$(DEVICE)

main_$(DEVICE): build/main_$(DEVICE).o build/matrices_$(DEVICE).o build/solve_$(DEVICE).o
	$(CC) $(CXXFLAGS) -o $@ $^ -lboost_program_options 

build/main_$(DEVICE).o: main.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $^

build/matrices_$(DEVICE).o: matrices.cpp
	$(CC) -c $(CXXFLAGS) -Minfo=accel -o $@ $^

build/solve_$(DEVICE).o: solve.cpp
	$(CC) -c $(CXXFLAGS) -Minfo=accel -o $@ $^

clean:
	rm -f build/*.o
