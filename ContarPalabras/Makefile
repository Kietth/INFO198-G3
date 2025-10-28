CXX = g++
CXXFLAGS = -Wall -Iinclude

all: conteo_texto

# Compila main.cpp y conteo.cpp en un ejecutable llamado conteo_texto
conteo_texto: src/main.cpp src/conteo.cpp
	$(CXX) $(CXXFLAGS) -o conteo_texto src/main.cpp src/conteo.cpp

clean:
	rm -f conteo_texto