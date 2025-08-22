CC=g++
CFLAGS=-Wall -O -std=c++17
BINS=prog

# Lista de todos los archivos fuente (.cpp)
SRCS=main.cpp guardarUsuario.cpp eliminarUsuario.cpp listarUsuarios.cpp

# Obtener los nombres de los archivos objeto (.o) a partir de los fuentes
OBJS=$(SRCS:.cpp=.o)

all: clean $(BINS)

# Regla principal para crear el ejecutable
$(BINS): $(OBJS)
	$(CC) $(CFLAGS) -o $(BINS) $(OBJS)

%.o: %.cpp usuario.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo " [CLN] Eliminando archivos binarios y de objeto"
	@rm -f $(BINS) $(OBJS) temp.bin 