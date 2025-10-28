#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <cctype>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif

using namespace std;
namespace fs = filesystem;

string normalizar(const string& palabra) {
    string out;
    for (char c : palabra) {
        if (isalpha((unsigned char)c)) {  // solo letras
            out.push_back(tolower(c));
        }
    }
    return out;
}


int main(int argc, char* argv[]) {
    cout << "=== Crear Indice Invertido === PID: "<< obtenerPID() << endl;

    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <archivo.idx> <carpeta_libros>" << endl;
        return 1;
    }

    string archivoIdx = argv[1];
    string carpeta = argv[2];

    // Validar extension .idx
    if (archivoIdx.size() < 4 || archivoIdx.substr(archivoIdx.size() - 4) != ".idx") {
        cerr << "❌ El archivo debe terminar en .idx" << endl;
        return 1;
    }

    // Validar carpeta
    if (!fs::exists(carpeta) || !fs::is_directory(carpeta)) {
        cerr << "❌ La carpeta no existe o no es válida: " << carpeta << endl;
        return 1;
    }

    // Estructura: palabra -> (archivo -> cantidad)
    map<string, map<string, int>> indice;

    // Recorrer todos los archivos de la carpeta
    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (entry.is_regular_file()) {
            string nombreArchivo = entry.path().filename().string();
            ifstream in(entry.path());
            if (!in.is_open()) {
                cerr << "⚠ No se pudo abrir: " << nombreArchivo << endl;
                continue;
            }

            string palabra;
            while (in >> palabra) {
                string limpia = normalizar(palabra);
                if (!limpia.empty()) {
                    indice[limpia][nombreArchivo]++;
                }
            }
        }
    }

    // Guardar en el archivo de salida
    ofstream out(archivoIdx);
    if (!out.is_open()) {
        cerr << "❌ No se pudo crear el archivo de índice: " << archivoIdx << endl;
        return 1;
    }

    for (auto& [palabra, docs] : indice) {
        out << palabra;
        for (auto& [doc, cant] : docs) {
            out << ";(" << doc << "," << cant << ")";
        }
        out << "\n";
    }

    cout << "✅ Índice invertido creado en: " << archivoIdx << endl;
    return 0;
}
