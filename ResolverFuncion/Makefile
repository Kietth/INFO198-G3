CXX = g++
CXXFLAGS = -Wall -Iinclude

all: sistema

# Compila main.cpp y funcion.cpp en un ejecutable llamado sistema
sistema: src/main.cpp src/funcion.cpp
	$(CXX) $(CXXFLAGS) -o sistema src/main.cpp src/funcion.cpp

clean:
	rm -f sistema
