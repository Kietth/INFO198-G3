# Compilador y opciones
CC = g++
CFLAGS = -O3 -Wall -Iinclude 

# Librerías
SERVER_LIBS = -pthread
CLIENT_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -pthread

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
# ROOT_DIR = . (Eliminado para evitar ambigüedad)

# --- Archivos Servidor ---
SERVER_SRC = $(SRC_DIR)/servidor.cpp \
             $(SRC_DIR)/logica.cpp \
             $(SRC_DIR)/red_servidor.cpp
             
SERVER_HEADERS = $(INCLUDE_DIR)/servidor.h \
                 $(INCLUDE_DIR)/logica.h \
                 $(INCLUDE_DIR)/red_servidor.h \
                 $(INCLUDE_DIR)/estado_servidor.h \
                 $(INCLUDE_DIR)/common.h

# --- Archivos Cliente ---
CLIENT_SRC = $(SRC_DIR)/cliente.cpp \
             $(SRC_DIR)/graficos.cpp \
             $(SRC_DIR)/red_cliente.cpp
             
CLIENT_HEADERS = $(INCLUDE_DIR)/cliente.h \
                 $(INCLUDE_DIR)/graficos.h \
                 $(INCLUDE_DIR)/red_cliente.h \
                 $(INCLUDE_DIR)/estado_cliente.h \
                 $(INCLUDE_DIR)/common.h

# --- CAMBIO AQUÍ: Nombres de ejecutables simplificados ---
# Se generarán en el directorio actual (la raíz)
SERVER_EXE = server
CLIENT_EXE = client
# --- FIN CAMBIO ---

# Regla principal
all: $(SERVER_EXE) $(CLIENT_EXE)

# Regla para compilar el servidor (salida a la raíz)
$(SERVER_EXE): $(SERVER_SRC) $(SERVER_HEADERS)
	$(CC) $(CFLAGS) -o $(SERVER_EXE) $(SERVER_SRC) $(SERVER_LIBS)

# Regla para compilar el cliente (salida a la raíz)
$(CLIENT_EXE): $(CLIENT_SRC) $(CLIENT_HEADERS) $(INCLUDE_DIR)/common.h
	$(CC) $(CFLAGS) -o $(CLIENT_EXE) $(CLIENT_SRC) $(CLIENT_LIBS)

# Regla Clean modificada
clean:
	rm -f $(SERVER_EXE) $(CLIENT_EXE)

# Regla phony para asegurar que 'all' y 'clean' siempre se consideren
.PHONY: all clean

