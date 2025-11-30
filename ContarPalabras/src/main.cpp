#include "conteo.h"
#include <iostream>
#include <string>
#include <fstream>   
#include <sstream>   
#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Error [conteo_texto]: No se proporcionó la ruta del archivo." << endl;
        cerr << "Este programa se debe llamar: " << argv[0] << " <ruta_del_archivo.txt>" << endl;
        return 1; 
    }

    string nombreArchivo = argv[1];

    
    ifstream file(nombreArchivo);
    if (!file) {
        cerr << "Error [conteo_texto]: No se pudo abrir el archivo '" << nombreArchivo << "'." << endl;
        return 1; 
    }

   
    stringstream buffer;
    buffer << file.rdbuf();
    string textoDelArchivo = buffer.str();
    file.close();

    if (textoDelArchivo.empty()) {
        cout << "El archivo '" << nombreArchivo << "' está vacío. No hay nada que contar." << endl;
        return 0;
    }

    cout << "\n==== CONTEO SOBRE TEXTO ==== PID: " << obtenerPID() << endl;
    cout << "Procesando archivo: " << nombreArchivo << "..." << endl;

    
    ResumenConteo r = contar(textoDelArchivo);

    cout << "\n--- RESUMEN DE CONTEO ---" << endl;
    cout << "Vocales:               " << r.vocales      << endl;
    cout << "Consonantes:           " << r.consonantes  << endl;
    cout << "Especiales (simbolos): " << r.especiales   << endl;
    cout << "Palabras:              " << r.palabras     << endl;

    return 0; // Éxito
}