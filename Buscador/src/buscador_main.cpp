#include "BuscadorCache.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>  // Para getpid()

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    cout << "=== BUSCADOR DE ÍNDICE INVERTIDO ===" << endl;
    cout << "PID: " << getpid() << endl;
    
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <archivo.idx>" << endl;
        return 1;
    }
    
    string archivoIdx = argv[1];
    
    // Validar archivo
    if (archivoIdx.size() < 4 || archivoIdx.substr(archivoIdx.size() - 4) != ".idx") {
        cerr << "❌ El archivo debe terminar en .idx" << endl;
        return 1;
    }
    
    ifstream file(archivoIdx);
    if (!file) {
        cerr << "❌ El archivo no existe: " << archivoIdx << endl;
        return 1;
    }
    file.close();
    
    // Valor hardcodeado
    int topK = 3;
    
    cout << "⚙️  Configuración - TOPK: " << topK << endl;
    
    BuscadorCache buscador;
    
    cout << "\n¡Buscador listo! Ingrese sus consultas (escriba 'salir' para terminar)\n" << endl;
    
    string consulta;
    while (true) {
        cout << "Query> ";
        getline(cin, consulta);
        
        if (consulta == "salir" || consulta == "exit") {
            cout << "Cerrando buscador..." << endl;
            break;
        }
        
        if (consulta.empty()) continue;
        
        try {
            json respuesta = buscador.buscar(archivoIdx, consulta, topK);
            
            // Agregar información del buscador principal
            respuesta["BuscadorPID"] = buscador.getPID();
            respuesta["ArchivoIndice"] = archivoIdx;
            
            cout << "\n" << respuesta.dump(4) << "\n" << endl;
        } catch (const exception& e) {
            cerr << "❌ Error en la búsqueda: " << e.what() << endl;
            
            // Respuesta de error
            json respuestaError = json::object();
            respuestaError["Error"] = e.what();
            respuestaError["Query"] = consulta;
            cout << "\n" << respuestaError.dump(4) << "\n" << endl;
        }
    }
    
    return 0;
}
