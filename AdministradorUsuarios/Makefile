# Makefile sin archivos .o visibles
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I./include
TARGET = administrador_usuarios
SRC_DIR = src

# Lista de archivos fuente
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/usuario.cpp $(SRC_DIR)/ArchivoManager.cpp \
          $(SRC_DIR)/sistema.cpp $(SRC_DIR)/PerfilManager.cpp


# Compilar directamente todos los archivos
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

.PHONY: all clean run rebuild
