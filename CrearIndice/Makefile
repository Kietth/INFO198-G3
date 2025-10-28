CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

all: clean indice

indice: src/crear_indice.cpp
	$(CXX) $(CXXFLAGS) -o crear_indice src/crear_indice.cpp

clean:
	rm -f crear_indice
