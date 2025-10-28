#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

using namespace std;

namespace fs = filesystem;

void crearArchivoIDX(const string& nombreArchivoIDX, const string& rutaCarpeta) {
    // Map 
    map<string, map<string, int>> indice;

    // Archivo de salida
    ofstream archivoSalida(nombreArchivoIDX);
    if (!archivoSalida.is_open()) {
        cerr << "Error: Fallo al crear archivo" << nombreArchivoIDX << endl;
        return;
    }

    try {
        // Recorremos los archivos
        for (const auto& entrada : fs::directory_iterator(rutaCarpeta)) {
            if (entrada.is_regular_file() && entrada.path().extension() == ".txt") {
                string nombreDocumento = entrada.path().filename().string();
                ifstream archivoEntrada(entrada.path());

                if (archivoEntrada.is_open()) {
                    string palabra;
                    // Leemos cada palabra del archivo
                    while (archivoEntrada >> palabra) {
                        // Se limpia la palabra de signos
                        string palabraLimpia;
                        for (char c : palabra) {
                            if (isalpha(c) || isdigit(c)) {
                                palabraLimpia += tolower(c);
                            }
                        }

                        if (!palabraLimpia.empty()) {
                            // Incrementamos el contador para la palabra y el documento
                            indice[palabraLimpia][nombreDocumento]++;
                        }
                    }
                    archivoEntrada.close();
                } else {
                    cerr << "Advertencia: No se pudo leer el archivo " << nombreDocumento << endl;
                }
            }
        }

        // Escribimos el índice en el archivo de salida
        for (const auto& parPalabra : indice) {
            archivoSalida << parPalabra.first << ";";
            bool esPrimerDoc = true;
            for (const auto& parDoc : parPalabra.second) {
                if (!esPrimerDoc) {
                    archivoSalida << ";";
                }
                archivoSalida << "(" << parDoc.first << "," << parDoc.second << ")";
                esPrimerDoc = false;
            }
            archivoSalida << endl;
        }

        cout << "¡Archivo " << nombreArchivoIDX << " creado exitosamente!" << endl;

    } catch (const fs::filesystem_error& ex) {
        cerr << "Error del sistema de archivos: " << ex.what() << endl; //manejo de errores
    }

    archivoSalida.close();
}

void interfazIi() {

    string nombre = "default";
    string librosPath = "./LIBROS";
    
    while (true) {
        cout << "\n==== (6) Indice Invertido ====\n";
        cout << "1) Cambiar nombre del archivo a crear (Nombre actual: " << nombre << ") \n";
        cout << "2) Cambiar ruta de la carpeta de libros a buscar (Ruta actual: " << librosPath << ") \n";
        cout << "3) Ejecutar\n";
        cout << "4) VOLVER\n";
        cout << "Seleccione una opcion: ";

        int op;
        cin >> op;

        switch(op) {
            case 1:
                cout << "Ingrese un nuevo nombre para el archivo: ";
                cin >> nombre; 
                break;
            case 2:
                cout << "Ingrese un nueva ruta: ";
                cin >> librosPath;
                break;
            case 3:
                cout << "Ejecutando... \n";
                crearArchivoIDX(nombre, librosPath); 
                break;
            case 4:
                return;
            default:
                cout << "Opción inválida\n";
        }
    }
}
