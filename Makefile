CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
THREAD_FLAGS = -pthread

# --- Ejecutables ---
ADMIN_BIN = AdministradorUsuarios/administrador_usuarios
MATRICES_BIN = MultMatrices/multmat
PALINDROMO_BIN = Palindromo/palindromo
FUNCION_BIN = ResolverFuncion/resolver_funcion
CONTEO_BIN = ContarPalabras/conteo_texto
INDICE_BIN = CrearIndice/crear_indice
INDICE_PARALELO_BIN = CrearIndiceParalelo/crear_indice_paralelo
BUSCADOR_BIN = Buscador/buscador
MOTOR_BUSQUEDA_BIN = MotorBusqueda/motor_busqueda
CACHE_BIN = Cache/cache
ANALISIS_THREADS_BIN = AnalisisThreads/analisis_threads
MENU_BIN = menu_principal

# --- Targets que no son archivos ---
.PHONY: all clean re

# --- Target por defecto ---
all: $(ADMIN_BIN) $(MATRICES_BIN) $(PALINDROMO_BIN) $(FUNCION_BIN) $(CONTEO_BIN) $(INDICE_BIN) $(INDICE_PARALELO_BIN) $(BUSCADOR_BIN) $(MOTOR_BUSQUEDA_BIN) $(CACHE_BIN) $(ANALISIS_THREADS_BIN) $(MENU_BIN)
	@echo "--- ✅ Compilacion general completada ---"

# --- Administrador de usuarios ---
$(ADMIN_BIN):
	@echo "--- Compilando [Administrador de usuarios] ---"
	$(CXX) AdministradorUsuarios/src/*.cpp \
	-IAdministradorUsuarios/include \
	-o $(ADMIN_BIN) $(CXXFLAGS)

# --- Multiplicacion de matrices ---
$(MATRICES_BIN):
	@echo "--- Compilando [Multiplicador de Matrices] ---"
	$(CXX) MultMatrices/src/*.cpp \
	-IMultMatrices/include \
	-o $(MATRICES_BIN) $(CXXFLAGS)

# --- Palindromo ---
$(PALINDROMO_BIN):
	@echo "--- Compilando [Palindromo] ---"
	$(CXX) Palindromo/src/*.cpp \
	-IPalindromo/include \
	-o $(PALINDROMO_BIN) $(CXXFLAGS)

# --- Resolver funcion ---
$(FUNCION_BIN):
	@echo "--- Compilando [Resolver Funcion] ---"
	$(CXX) ResolverFuncion/src/*.cpp \
	-IResolverFuncion/include \
	-o $(FUNCION_BIN) $(CXXFLAGS)

# --- Conteo de texto ---
$(CONTEO_BIN):
	@echo "--- Compilando [Conteo de Texto] ---"
	$(CXX) ContarPalabras/src/*.cpp \
	-IContarPalabras/include \
	-o $(CONTEO_BIN) $(CXXFLAGS)

# --- Crear Indice ---
$(INDICE_BIN):
	@echo "--- Compilando [Crear Indice] ---"
	$(CXX) CrearIndice/src/*.cpp \
	-ICrearIndice/include \
	-o $(INDICE_BIN) $(CXXFLAGS) -lstdc++fs

# --- Crear Indice Paralelo ---
$(INDICE_PARALELO_BIN):
	@echo "--- Compilando [Crear Indice PARALELO] ---"
	$(CXX) CrearIndiceParalelo/src/crear_indice_paralelo.cpp \
	-o $(INDICE_PARALELO_BIN) $(CXXFLAGS) $(THREAD_FLAGS) -lstdc++fs

# --- Buscador (Cliente Principal) ---
$(BUSCADOR_BIN):
	@echo "--- Compilando [Buscador Principal] ---"
	$(CXX) Buscador/src/buscador_main.cpp Buscador/src/BuscadorCache.cpp \
	-IBuscador/include -IMenuPrincipal/include \
	-o $(BUSCADOR_BIN) $(CXXFLAGS) -lstdc++fs

# --- Motor de Busqueda ---
$(MOTOR_BUSQUEDA_BIN):
	@echo "--- Compilando [Motor de Busqueda] ---"
	$(CXX) MotorBusqueda/src/motor_busqueda.cpp \
	-IMotorBusqueda/include -IBuscador/include -IMenuPrincipal/include \
	-o $(MOTOR_BUSQUEDA_BIN) $(CXXFLAGS) -lstdc++fs

# --- Cache ---
$(CACHE_BIN):
	@echo "--- Compilando [Cache] ---"
	$(CXX) Cache/src/cache.cpp \
	-ICache/include -IBuscador/include -IMenuPrincipal/include \
	-o $(CACHE_BIN) $(CXXFLAGS) -lstdc++fs

# --- Analisis de rendimiento con threads ---
$(ANALISIS_THREADS_BIN):
	@echo "--- Compilando [Analisis de rendimiento con threads] ---"
	$(CXX) AnalisisThreads/src/analisis_threads.cpp \
	MenuPrincipal/src/env_utils.cpp \
	-IMenuPrincipal/include \
	-o $(ANALISIS_THREADS_BIN) $(CXXFLAGS)

# --- Menu principal ---
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
	@echo "--- Limpiando todos los ejecutables ---"
	rm -f $(ADMIN_BIN) $(MATRICES_BIN) $(PALINDROMO_BIN) $(FUNCION_BIN) $(MENU_BIN) $(CONTEO_BIN) $(INDICE_BIN) $(INDICE_PARALELO_BIN) $(BUSCADOR_BIN) $(MOTOR_BUSQUEDA_BIN) $(CACHE_BIN) $(ANALISIS_THREADS_BIN)
	@echo "--- Limpieza completada ---"

# --- Reconstruir (re) ---
re: clean all
	@echo "--- Reconstruccion general completada ---"
