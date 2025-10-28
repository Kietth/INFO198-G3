CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Ejecutables
ADMIN_BIN = AdministradorUsuarios/administrador_usuarios
MATRICES_BIN = MultMatrices/multmat
PALINDROMO_BIN = Palindromo/palindromo
FUNCION_BIN = ResolverFuncion/resolver_funcion
MENU_BIN = menu_principal
CONTEO_BIN = ContarPalabras/conteo_texto
# Nuevo ejecutable
II_BIN = IndiceInvertido/ii

all: admin matrices palindromo funcion menu conteo ii

# --- Administrador de usuarios ---
admin:
	$(CXX) AdministradorUsuarios/src/*.cpp \
	-IAdministradorUsuarios/include \
	-o $(ADMIN_BIN) $(CXXFLAGS)

# --- Multiplicación de matrices ---
matrices:
	$(CXX) MultMatrices/src/*.cpp \
	-IMultMatrices/include \
	-o $(MATRICES_BIN) $(CXXFLAGS)

# --- Palíndromo ---
palindromo:
	$(CXX) Palindromo/src/*.cpp \
	-IPalindromo/include \
	-o $(PALINDROMO_BIN) $(CXXFLAGS)

# --- Resolver función ---
funcion:
	$(CXX) ResolverFuncion/src/*.cpp \
	-IResolverFuncion/include \
	-o $(FUNCION_BIN) $(CXXFLAGS)

# --- Conteo de texto ---
conteo:
	$(CXX) ContarPalabras/src/*.cpp \
	-IContarPalabras/include \
	-o $(CONTEO_BIN) $(CXXFLAGS)

# --- Menú principal ---
MENU_SRCS = MenuPrincipal/src/main.cpp \
	MenuPrincipal/src/Auth.cpp \
	MenuPrincipal/src/ConfigManager.cpp \
	MenuPrincipal/src/MatrixInterface.cpp \
	AdministradorUsuarios/src/ArchivoManager.cpp \
	AdministradorUsuarios/src/PerfilManager.cpp \
	AdministradorUsuarios/src/sistema.cpp \
	AdministradorUsuarios/src/usuario.cpp

menu:
	$(CXX) $(MENU_SRCS) \
	-IAdministradorUsuarios/include \
	-IMenuPrincipal/include \
	-o $(MENU_BIN) $(CXXFLAGS)

# --- Indice Invertido ---
ii:
	$(CXX) IndiceInvertido/src/main.cpp IndiceInvertido/src/ii.cpp \
	-IIndiceInvertido/include \
	-o $(II_BIN) $(CXXFLAGS)

# --- Limpiar ---
clean:
	rm -f $(ADMIN_BIN) $(MATRICES_BIN) $(PALINDROMO_BIN) $(FUNCION_BIN) $(MENU_BIN) $(CONTEO_BIN) $(II_BIN)
