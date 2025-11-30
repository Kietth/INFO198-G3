#include "Menu.h"
#include "Auth.h"
#include "env_utils.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <unistd.h> // Para getpid()
inline int obtenerPID() {return getpid();}


using namespace std;


string getRutaEjecutable(string ruta) {
    if (ruta.rfind("./", 0) != 0 && ruta.rfind("/", 0) != 0) {
        ruta = "./" + ruta;
    }
    return ruta;
}

void esperar() {
    cout << "\nPresione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void Menu::mostrar(const string& user, const string& pass, const string& filePath) {
    Auth auth("AdministradorUsuarios/USUARIOS.TXT",
              "AdministradorUsuarios/PERFILES.TXT");

    if (!auth.login(user, pass)) {
        cout << "Usuario o contraseña inválidos\n";
        return;
    }

    cout << "Bienvenido, " << auth.getNombre()
         << " (perfil: " << auth.getPerfil() << ")\n";

    int opcion;
    do {
        cout << "\n=== MENU PRINCIPAL ===  PID: " << obtenerPID() << endl;
        cout << "0. Salir\n";

        if (auth.tienePermiso(1)) cout << "1. Administrador de usuarios y perfiles\n";
        if (auth.tienePermiso(2)) cout << "2. Multiplica matrices NxN\n";
        if (auth.tienePermiso(3)) cout << "3. Juego\n";
        if (auth.tienePermiso(4)) cout << "4. ¿Es palíndromo?\n";
        if (auth.tienePermiso(5)) cout << "5. Calcular f(x)=x^2+2x+8\n";
        if (auth.tienePermiso(6)) cout << "6. Conteo sobre texto\n";
        if (auth.tienePermiso(7)) cout << "7. Crea índice invertido\n";
        if (auth.tienePermiso(8)) cout << "8. Crea índice invertido PARALELO\n";
        if (auth.tienePermiso(9)) cout << "9. Buscador de índice invertido\n";
        if (auth.tienePermiso(10)) cout << "10. Análisis de rendimiento (índice paralelo)\n";

        cout << "Seleccione: ";
        cin >> opcion;

        if (opcion == 0) {
            cout << "Saliendo...\n";
            break;
        }

        if (!auth.tienePermiso(opcion)) {
            cout << "❌ No tiene permiso para esta opción.\n";
            esperar();
            continue;
        }

        switch(opcion) {
            // ----- CASE 1: AdministradorUsuarios -----
            case 1: {
                auto env = cargarEnv(".env");
                string adminPath = env["ADMIN_SYS"]; 
                
                adminPath = getRutaEjecutable(adminPath); 

                
                size_t lastSlash = adminPath.find_last_of('/');
                string directorio = ".";
                string ejecutable = adminPath;

                if (lastSlash != string::npos) {
                    directorio = adminPath.substr(0, lastSlash); 
                    ejecutable = adminPath.substr(lastSlash + 1); 
                }
                
                
                string comando = "cd " + directorio + " && ./" + ejecutable + " && cd -";
                

                cout << "Ejecutando: " << comando << endl;
                int ret = system(comando.c_str());
                if (ret != 0) {
                    cout << "❌ No se pudo ejecutar el administrador de usuarios (código: " << ret << ").\n";
                }
                
                esperar();
                break;
            }
            // ----- CASE 2: Multiplica matrices -----
            case 2:{
                auto env = cargarEnv(".env");
              
                string multiPath = getRutaEjecutable(env["MULTI_M"]);
                
                int N;
                string pathA, pathB;
                
                cout << "\n--- Multiplicación de Matrices ---" << endl;
                cout << "Ingrese el tamaño de las matrices (N): ";
                cin >> N;
                
                if (N <= 0) {
                    cout << "❌ El tamaño N debe ser mayor que 0." << endl;
                    esperar();
                    break;
                }
                
                cin.ignore(); 
                
                cout << "Ingrese el path del archivo de la primera matriz: ";
                getline(cin, pathA);
                
                cout << "Ingrese el path del archivo de la segunda matriz: ";
                getline(cin, pathB);
                
                ifstream fileA(pathA), fileB(pathB);
                if (!fileA) {
                    cout << "❌ El archivo de la matriz 1 no existe." << endl;
                    esperar();
                    break;
                }
                if (!fileB) {
                    cout << "❌ El archivo de la matriz 2 no existe." << endl;
                    esperar();
                    break;
                }
                
                if (fileA.peek() == ifstream::traits_type::eof()) {
                    cout << "❌ El archivo de la matriz 1 está vacío." << endl;
                    esperar();
                    break;
                }
                if (fileB.peek() == ifstream::traits_type::eof()) {
                    cout << "❌ El archivo de la matriz 2 está vacío." << endl;
                    esperar();
                    break;
                }
                
                string comando = multiPath + " \"" + pathA + "\" \"" + pathB + "\" " + to_string(N);
                cout << "Ejecutando: " << comando << endl;
                
                int resultado = system(comando.c_str());
                
                if (resultado != 0) {
                    cout << "❌ Error al ejecutar el multiplicador de matrices (código: " << resultado << ")." << endl;
                }
                
                esperar();
                break;
            }
            // ----- CASE 3: Juego -----
            case 3:
                cout << "Juego (en construcción)\n";
                esperar();
                break;
            // ----- CASE 4: Palíndromo (Hardcodeado) -----
            case 4:
                
                system("./Palindromo/palindromo");
                esperar();
                break;
            // ----- CASE 5: Resolver Función (Hardcodeado) -----
            case 5:
                
                system("./ResolverFuncion/resolver_funcion");
                esperar();
                break;
            // ----- CASE 6: Conteo sobre texto -----
            case 6: { 
                auto env = cargarEnv(".env");
                
                string prog = getRutaEjecutable(env["CONTEO_TEXTO"]); 

                if (prog.empty()) {
                    cout << "❌ La variable de entorno CONTEO_TEXTO no está definida en .env" << endl;
                    esperar();
                    break;
                }
                
                ifstream file(filePath); 
                if (!file) {
                    cout << "❌ El archivo de libro (" << filePath << ") no existe." << endl;
                    esperar();
                    break;
                }
                
                if (file.peek() == ifstream::traits_type::eof()) {
                    cout << "❌ El archivo de libro (" << filePath << ") está vacío." << endl;
                    esperar();
                    break;
                }
                file.close();

                string comando = prog + " \"" + filePath + "\"";
                cout << "Ejecutando: " << comando << endl;

                int resultado = system(comando.c_str());
                
                if (resultado != 0) {
                    cout << "❌ Error al ejecutar el conteo de palabras (código: " << resultado << ")." << endl;
                }
                
                esperar();
                break;
            } 
            // ----- CASE 7: Crear Índice -----
            case 7: {
                string nombreIdx, carpeta;
                cout << "Ingrese el nombre del archivo índice (.idx): ";
                cin >> nombreIdx;

                if (nombreIdx.size() < 4 || nombreIdx.substr(nombreIdx.size()-4) != ".idx") {
                    cout << "❌ El archivo debe terminar en .idx\n";
                    esperar();
                    break;
                }

                cout << "Ingrese el path de la carpeta con los libros: ";
                cin >> carpeta;

                auto env = cargarEnv(".env");
                
                string prog = getRutaEjecutable(env["CREATE_INDEX"]);

                string comando = prog + " " + nombreIdx + " " + carpeta;
                cout << "Ejecutando: " << comando << endl;

                system(comando.c_str());
                esperar();
                break;
            }
            // ----- CASE 8: Crear Índice Invertido PARALELO -----
            case 8: {
                string nombreIdx, carpeta;
                int nThreads, nLote;
                
                cout << "\n--- Crear Índice Invertido PARALELO ---" << endl;
                
                cout << "Ingrese el nombre del archivo índice (.idx): ";
                cin >> nombreIdx;

                if (nombreIdx.size() < 4 || nombreIdx.substr(nombreIdx.size()-4) != ".idx") {
                    cout << "❌ El archivo debe terminar en .idx\n";
                    esperar();
                    break;
                }

                cout << "Ingrese el path de la carpeta con los libros: ";
                cin >> carpeta;
                
                cout << "Ingrese el número de threads (N-THREADS): ";
                cin >> nThreads;
                
                if (nThreads <= 0) {
                    cout << "❌ N-THREADS debe ser mayor que 0" << endl;
                    esperar();
                    break;
                }
                
                cout << "Ingrese el tamaño del lote (N-LOTE): ";
                cin >> nLote;
                
                if (nLote <= 0) {
                    cout << "❌ N-LOTE debe ser mayor que 0" << endl;
                    esperar();
                    break;
                }

                auto env = cargarEnv(".env");
                string prog = getRutaEjecutable(env["INDICE_INVET_PARALELO"]);

                if (prog.empty()) {
                    cout << "❌ La variable INDICE_INVET_PARALELO no está definida en .env" << endl;
                    esperar();
                    break;
                }

                string comando = prog + " " + nombreIdx + " " + carpeta + " " 
                               + to_string(nThreads) + " " + to_string(nLote);
                cout << "Ejecutando: " << comando << endl;

                int resultado = system(comando.c_str());
                
                if (resultado != 0) {
                    cout << "❌ Error al ejecutar el índice paralelo (código: " << resultado << ")." << endl;
                }
                
                esperar();
                break;
            }
            // ----- CASE 9: Buscador de Índice Invertido -----
            case 9: {
                string nombreIdx;
                
                cout << "\n--- Buscador de Índice Invertido ---" << endl;
                
                cout << "Ingrese el nombre del archivo índice (.idx): ";
                cin >> nombreIdx;

                if (nombreIdx.size() < 4 || nombreIdx.substr(nombreIdx.size()-4) != ".idx") {
                    cout << "❌ El archivo debe terminar en .idx\n";
                    esperar();
                    break;
                }

                // Verificar que el archivo existe
                ifstream fileIdx(nombreIdx);
                if (!fileIdx) {
                    cout << "❌ El archivo de índice no existe: " << nombreIdx << endl;
                    esperar();
                    break;
                }
                fileIdx.close();

                auto env = cargarEnv(".env");
                string prog = getRutaEjecutable(env["BUSCADOR"]);

                if (prog.empty()) {
                    cout << "❌ La variable BUSCADOR no está definida en .env" << endl;
                    esperar();
                    break;
                }

                string comando = prog + " " + nombreIdx;
                cout << "Ejecutando: " << comando << endl;

                int resultado = system(comando.c_str());
                
                if (resultado != 0) {
                    cout << "❌ Error al ejecutar el buscador (código: " << resultado << ")." << endl;
                }
                
                esperar();
                break;
            }
            // ----- CASE 10: Análisis de rendimiento con threads -----
            case 10: {
                cout << "\n--- Análisis de rendimiento del índice invertido paralelo ---" << endl;

                auto env = cargarEnv(".env");
                string prog = getRutaEjecutable(env["ANALISIS_THREADS"]);

                if (prog.empty()) {
                    cout << "❌ La variable ANALISIS_THREADS no está definida o está vacía en .env" << endl;
                    esperar();
                    break;
                }

                string comando = prog; // No necesita argumentos extra, el propio programa pregunta todo
                cout << "Ejecutando: " << comando << endl;

                int resultado = system(comando.c_str());
                if (resultado != 0) {
                    cout << "❌ Error al ejecutar el análisis de rendimiento (código: "
                         << resultado << ")." << endl;
                }

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
    string user, pass, filePath; 

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-u" && i + 1 < argc) {
            user = argv[++i];
        } else if (arg == "-p" && i + 1 < argc) {
            pass = argv[++i];
        } else if (arg == "-f" && i + 1 < argc) { 
            filePath = argv[++i];
        }
    }

    if (user.empty() || pass.empty() || filePath.empty()) {
        cerr << "Uso: " << argv[0] << " -u <usuario> -p <password> -f <ruta_archivo_libro>\n";
        return 1;
    }

    Menu menu;
    menu.mostrar(user, pass, filePath);

    return 0;
}
