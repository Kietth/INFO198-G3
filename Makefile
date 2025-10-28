CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

# --- Ejecutables ---
ADMIN_BIN = AdministradorUsuarios/administrador_usuarios
MATRICES_BIN = MultMatrices/multmat
PALINDROMO_BIN = Palindromo/palindromo
FUNCION_BIN = ResolverFuncion/resolver_funcion
CONTEO_BIN = ContarPalabras/conteo_texto
INDICE_BIN = CrearIndice/crear_indice
MENU_BIN = menu_principal

# --- Targets que no son archivos ---
.PHONY: all clean re

# --- Target por defecto ---
all: $(ADMIN_BIN) $(MATRICES_BIN) $(PALINDROMO_BIN) $(FUNCION_BIN) $(CONTEO_BIN) $(INDICE_BIN) $(MENU_BIN)
	@echo "--- ✅ Compilacion general completada ---"

# --- Administrador de usuarios ---
$(ADMIN_BIN):
	@echo "--- Compilando [Administrador de usuarios] ---"
	$(CXX) AdministradorUsuarios/src/*.cpp \
	-IAdministradorUsuarios/include \
	-o $(ADMIN_BIN) $(CXXFLAGS)

# --- Multiplicación de matrices ---
$(MATRICES_BIN):
	@echo "--- Compilando [Multiplicador de Matrices] ---"
	$(CXX) MultMatrices/src/*.cpp \
	-IMultMatrices/include \
	-o $(MATRICES_BIN) $(CXXFLAGS)

# --- Palíndromo ---
$(PALINDROMO_BIN):
	@echo "--- Compilando [Palindromo] ---"
	$(CXX) Palindromo/src/*.cpp \
	-IPalindromo/include \
	-o $(PALINDROMO_BIN) $(CXXFLAGS)

# --- Resolver función ---
$(FUNCION_BIN):
	@echo "--- Compilando [Resolver Funcion] ---"
	$(CXX) ResolverFuncion/src/*.cpp \
	-IResolverFuncion/include \
	-o $(FUNCION_BIN) $(CXXFLAGS)

# --- Conteo de texto ---
$(CONTEO_BIN):
	@echo "--- Compilando [Conteo de Texto] ---"
	$(CXX) ContarPalabras/src/*.cpp  \
	-IContarPalabras/include \
	-o $(CONTEO_BIN) $(CXXFLAGS)

# --- Crear Indice ---
$(INDICE_BIN):
	@echo "--- Compilando [Crear Indice] ---"
	$(CXX) CrearIndice/src/*.cpp  \
	-ICrearIndice/include \
	-o $(INDICE_BIN) $(CXXFLAGS) -lstdc++fs

# --- Menú principal ---
$(MENU_BIN): $(ADMIN_BIN)
	@echo "--- Compilando [Menu Principal] ---"
	$(CXX) MenuPrincipal/src/*.cpp \
	AdministradorUsuarios/src/ArchivoManager.cpp \
	AdministradorUsuarios/src/PerfilManager.cpp \
	AdministradorUsuarios/src/sistema.cpp \
	AdministradorUsuarios/src/usuario.cpp \
	-IAdministradorUsuarios/include -IMenuPrincipal/include \
	-o $(MENU_BIN) $(CXXFLAGS)

# --- Limpiar ---
clean:
	@echo "--- 🧹 Limpiando todos los ejecutables ---"
	rm -f $(ADMIN_BIN) $(MATRICES_BIN) $(PALINDROMO_BIN) $(FUNCION_BIN) $(MENU_BIN) $(CONTEO_BIN) $(INDICE_BIN)
	@echo "--- Limpieza completada ---"

# --- Reconstruir (re) ---
re: clean all
	@echo "--- ✨ Reconstruccion general completada ---"