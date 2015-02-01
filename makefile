CXX = clang++
CXXFLAGS = -std=c++11 -lsfml-graphics -lsfml-system -lsfml-window -O2

all: molasses-test

clean:
	rm -rf *.o molasses-test
