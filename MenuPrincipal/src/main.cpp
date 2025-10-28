// === MenuPrincipal/src/main.cpp (VERSIÓN CORREGIDA) ===
#include "../include/Menu.h"
#include "../include/Auth.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
using namespace std;

// Función para ejecutar comandos mostrando el PID
int ejecutarConPID(const string& comando, const string& nombreProceso) {
    cout << "\n----------------------------------------" << endl;
    cout << "INICIANDO PROCESO EXTERNO" << endl;
    cout << "Proceso: " << nombreProceso << endl;
    cout << "PID actual: " << getpid() << endl;
    cout << "Comando: " << comando << endl;
    cout << "----------------------------------------" << endl;
    
    int resultado = system(comando.c_str());
    
    cout << "----------------------------------------" << endl;
    cout << "PROCESO FINALIZADO" << endl;
    cout << "Proceso: " << nombreProceso << endl;
    cout << "PID: " << getpid() << endl;
    
    if (resultado != 0) {
        cout << "Error al ejecutar " << nombreProceso << " (código: " << resultado << ")." << endl;
    } else {
        cout << "Ejecución exitosa" << endl;
    }
    
    cout << "----------------------------------------" << endl;
    
    return resultado;
}

// Función para cargar configuración desde .env
void cargarConfiguracion(const string& archivo = ".env") {
    ifstream file(archivo);
    
    if (!file.is_open()) {
        cerr << "ADVERTENCIA: No se pudo abrir el archivo " << archivo << endl;
        cerr << "Usando valores por defecto." << endl;
        return;
    }
    
    string linea;
    while (getline(file, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        
        size_t pos = linea.find('=');
        if (pos != string::npos) {
            string clave = linea.substr(0, pos);
            string valor = linea.substr(pos + 1);
            
            // Limpiar espacios
            clave.erase(0, clave.find_first_not_of(" \t"));
            clave.erase(clave.find_last_not_of(" \t") + 1);
            
            valor.erase(0, valor.find_first_not_of(" \t"));
            valor.erase(valor.find_last_not_of(" \t") + 1);
            
            setenv(clave.c_str(), valor.c_str(), 1);
        }
    }
    file.close();
}

// Función para validar archivo
bool validarArchivoExiste(const string& ruta) {
    ifstream archivo(ruta);
    return archivo.good();
}

// Función para interfaz de matrices
void interfazMultiplicadorMatrices() {
    int opcionMatrices;
    
    do {
        cout << "\n=== MULTIPLICADOR DE MATRICES ===" << endl;
        cout << "PID del proceso: " << getpid() << endl; // ✅ MOSTRAR PID
        cout << "1. Ingresar datos manualmente" << endl;
        cout << "2. Usar matrices de ejemplo 3x3" << endl;
        cout << "3. Usar matrices de ejemplo 4x4" << endl;
        cout << "0. Volver al menú principal" << endl;
        cout << "Seleccione: ";
        cin >> opcionMatrices;

        if (opcionMatrices == 0) {
            cout << "Volviendo al menú principal..." << endl;
            return;
        }

        string archivoA, archivoB;
        int n = 0;

        switch(opcionMatrices) {
            case 1: {
                cout << "\n--- Ingreso Manual ---" << endl;
                cout << "PID del proceso: " << getpid() << endl; // ✅ MOSTRAR PID
                cout << "Tamaño de la matriz (N): ";
                cin >> n;
                
                if (n <= 0) {
                    cout << "Error: El tamaño de la matriz debe ser mayor que 0." << endl;
                    continue;
                }
                
                cout << "Ruta completa del archivo de la matriz A: ";
                cin.ignore();
                getline(cin, archivoA);
                
                cout << "Ruta completa del archivo de la matriz B: ";
                getline(cin, archivoB);
                break;
            }
            
            case 2: {
                cout << "\n--- Matrices de Ejemplo 3x3 ---" << endl;
                cout << "PID del proceso: " << getpid() << endl; // ✅ MOSTRAR PID
                n = 3;
                const char* matricesDir = getenv("MATRICES_EJEMPLO");
                string directorio = matricesDir ? matricesDir : "./MultMatrices/data";
                
                archivoA = directorio + "/A.txt";
                archivoB = directorio + "/B.txt";
                
                cout << "Matriz A: " << archivoA << " (3x3)" << endl;
                cout << "Matriz B: " << archivoB << " (3x3)" << endl;
                break;
            }
            
            case 3: {
                cout << "\n--- Matrices de Ejemplo 4x4 ---" << endl;
                cout << "PID del proceso: " << getpid() << endl; // ✅ MOSTRAR PID
                n = 4;
                const char* matricesDir = getenv("MATRICES_EJEMPLO");
                string directorio = matricesDir ? matricesDir : "./MultMatrices/data";
                
                archivoA = directorio + "/A_4x4.txt";
                archivoB = directorio + "/B_4x4.txt";
                
                cout << "Matriz A: " << archivoA << " (4x4)" << endl;
                cout << "Matriz B: " << archivoB << " (4x4)" << endl;
                break;
            }
            
            default:
                cout << "Opción inválida. Intente de nuevo." << endl;
                continue;
        }

        // Validar existencia de archivos
        if (!validarArchivoExiste(archivoA)) {
            cout << "Error: El archivo " << archivoA << " no existe." << endl;
            cout << "Verifique la ruta o use matrices de ejemplo." << endl;
            continue;
        }
        
        if (!validarArchivoExiste(archivoB)) {
            cout << "Error: El archivo " << archivoB << " no existe." << endl;
            cout << "Verifique la ruta o use matrices de ejemplo." << endl;
            continue;
        }
        
        cout << "✓ Archivos validados correctamente." << endl;
        cout << "✓ Tamaño de matriz: " << n << "x" << n << endl;
        
        // Confirmar antes de lanzar ejecución
        char confirmar;
        cout << "\n¿Desea ejecutar la multiplicación? (s/n): ";
        cin >> confirmar;
        
        if (confirmar == 's' || confirmar == 'S') {
            const char* multiMPath = getenv("MULTI_M");
            string comando = multiMPath ? multiMPath : "./MultMatrices/multmat";
            comando += " \"" + archivoA + "\" \"" + archivoB + "\"";
            
            // ✅ MOSTRAR PID en la ejecución
            ejecutarConPID(comando, "Multiplicador de Matrices");
        } else {
            cout << "Multiplicación cancelada." << endl;
        }
        
        // Preguntar si quiere hacer otra operación
        char otra;
        cout << "\n¿Desea realizar otra operación de matrices? (s/n): ";
        cin >> otra;
        
        if (otra != 's' && otra != 'S') {
            cout << "Volviendo al menú principal..." << endl;
            break;
        }
        
    } while(true);
}

void esperar() {
    cout << "\nPresione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void Menu::mostrar(const string& user, const string& pass) {
    // Cargar configuración desde .env
    cargarConfiguracion();
    cout << "=== CONFIGURACIÓN CARGADA ===" << endl;
    
    Auth auth("./AdministradorUsuarios/USUARIOS.TXT",
              "./AdministradorUsuarios/PERFILES.TXT");

    if (!auth.login(user, pass)) {
        cout << "Usuario o contraseña inválidos\n";
        return;
    }

    cout << "Bienvenido, " << auth.getNombre()
         << " (perfil: " << auth.getPerfil() << ")\n";
    
    // Mostrar PID inicial
    cout << "PID del proceso principal: " << getpid() << endl;
    cout << "=============================" << endl;

    int opcion;
    do {
        cout << "\n=== MENU PRINCIPAL ===" << endl;
        cout << "0. Salir" << endl;

        if (auth.tienePermiso(1)) cout << "1. Administrador de usuarios y perfiles" << endl;
        if (auth.tienePermiso(2)) cout << "2. Multiplica matrices NxN" << endl;
        if (auth.tienePermiso(3)) cout << "3. Juego" << endl;
        if (auth.tienePermiso(4)) cout << "4. ¿Es palíndromo?" << endl;
        if (auth.tienePermiso(5)) cout << "5. Calcular f(x)=x^2+2x+8" << endl;
        if (auth.tienePermiso(6)) cout << "6. Conteo sobre texto" << endl;
        if (auth.tienePermiso(7)) cout << "7. Crear indice invertido" << endl;
        

        cout << "Seleccione: ";
        cin >> opcion;

        if (opcion == 0) {
            cout << "Saliendo...\n";
            break;
        }

        if (!auth.tienePermiso(opcion)) {
            cout << "No tiene permiso para esta opción.\n";
            esperar();
            continue;
        }

        switch(opcion) {
            case 1:
                if (auth.getPerfil() == "ADMIN") {
                    const char* adminPath = getenv("ADMIN_SYS");
                    string comando = adminPath ? adminPath : "./AdministradorUsuarios/administrador_usuarios";
                    
                    // Pasar rutas correctas de archivos
                    comando += " ./AdministradorUsuarios/USUARIOS.TXT ./AdministradorUsuarios/PERFILES.TXT";
                    
                    // Ejecutar mostrando el PID
                    ejecutarConPID(comando, "Administrador de Usuarios");
                } else {
                    cout << "Error: Solo usuarios ADMIN pueden acceder a esta función." << endl;
                }
                esperar();
                break;
                
            case 2:
                interfazMultiplicadorMatrices();
                esperar();
                break;
                
            case 3:
                cout << "Juego (en construcción)\n";
                esperar();
                break;
                
            case 4: {
                const char* palindromoPath = getenv("PALINDROMO");
                string comando = palindromoPath ? palindromoPath : "./Palindromo/palindromo";
                ejecutarConPID(comando, "Validador de Palíndromos");
                esperar();
                break;
            }
                
            case 5: {
                const char* funcionPath = getenv("RESOLVER_FUNCION");
                string comando = funcionPath ? funcionPath : "./ResolverFuncion/resolver_funcion";
                ejecutarConPID(comando, "Resolvedor de Funciones");
                esperar();
                break;
            }
                
            case 6: {
                const char* conteoPath = getenv("CONTEO_TEXTO");
                string comando = conteoPath ? conteoPath : "./ContarPalabras/conteo_texto";
                ejecutarConPID(comando, "Contador de Texto");
                esperar();
                break;
            }
             case 7: {
                const char* iiPath = getenv("CREATE_INDEX");
                string comando = iiPath ? iiPath : "./IndiceInvertido/ii";
                ejecutarConPID(comando, "Crear Indice Invertido");
                esperar();
                break;
            }
                
            default:
                cout << "Opción inválida\n";
                esperar();
        }
    } while(true);
}

int main(int argc, char* argv[]) {
    string user, pass;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-u" && i + 1 < argc) {
            user = argv[++i];
        } else if (arg == "-p" && i + 1 < argc) {
            pass = argv[++i];
        }
    }

    if (user.empty() || pass.empty()) {
        cerr << "Uso: " << argv[0] << " -u <usuario> -p <password>\n";
        cerr << "Ejemplo: " << argv[0] << " -u seba -p 123\n";
        return 1;
    }

    Menu menu;
    menu.mostrar(user, pass);

    return 0;
}
