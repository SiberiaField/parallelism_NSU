ifndef USE_FLOAT
	USE_FLOAT = OFF
endif

ifeq ($(USE_FLOAT), ON)
	Define_Float = -DUSE_FLOAT
else
	Define_Float =
endif


main: main.o sin_vec.o
	g++ main.o sin_vec.o -o main


main.o: main.cpp
	g++ -c main.cpp -o main.o $(Define_Float)

sin_vec.o: sin_vec.cpp sin_vec.hpp
	g++ -c sin_vec.cpp -o sin_vec.o $(Define_Float)


clean:
	rm main.o sin_vec.o main