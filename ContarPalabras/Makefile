CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude

#.exe
BIN := conteo_texto.exe
SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp,build/%.o,$(SRC))

.PHONY: all clean

all: $(BIN)

# Compilar cada .cpp a build/*.o
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

# Linkear objetos al binario
$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o "$@"

# Crear carpeta build si no existe (sintaxis de cmd/PowerShell)
build:
	if not exist build mkdir build

# Limpiar
clean:
	-if exist build rmdir /S /Q build
	-if exist $(BIN) del /Q $(BIN)
