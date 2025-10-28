CXX = g++
CXXFLAGS = -std=c++11 -Iinclude

all: clean multmat

multmat: src/main.cpp src/func_matrices.cpp src/inoutarchivos.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Ejecutable 'multmat' creado."

clean:
	@rm -f multmat
	@echo "Limpieza completada."