# Makefile sin archivos .o visibles
CXX = g++
CXXFLAGS = -Wall -g -I./include
LDFLAGS =

SRC_DIR = src
OBJ_DIR = build

TARGET = administrador_usuarios

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/usuario.cpp $(SRC_DIR)/ArchivoManager.cpp \
		$(SRC_DIR)/sistema.cpp $(SRC_DIR)/PerfilManager.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Enlazando $(TARGET)..."
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS)
	@echo "$(TARGET) creado en $(CURDIR)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compilando $< -> $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Limpiando $(TARGET) y $(OBJ_DIR) en $(CURDIR)..."
	rm -f $(TARGET)
	rm -f $(OBJ_DIR)/*.o