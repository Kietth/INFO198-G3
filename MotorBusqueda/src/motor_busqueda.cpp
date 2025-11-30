#include "MotorBusqueda.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <unistd.h>  // Para getpid()

using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;

string MotorBusqueda::normalizar(const string& palabra) {
    string out;
    for (char c : palabra) {
        if (isalpha((unsigned char)c)) {
            out.push_back(tolower(c));
        }
    }
    return out;
}

void MotorBusqueda::cargarMapeoLibros(const string& carpetaLibros) {
    if (carpetaLibros.empty()) return;
    
    try {
        int count = 0;
        for (const auto& entry : fs::directory_iterator(carpetaLibros)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                string id = to_string(count++); // ID secuencial simple
                mapeoLibros[id] = filename;
            }
        }
    } catch (const exception& e) {
        cerr << "⚠ No se pudo cargar mapeo de libros: " << e.what() << endl;
    }
}

bool MotorBusqueda::cargarIndice(const string& archivoIdx) {
    ifstream file(archivoIdx);
    if (!file.is_open()) {
        cerr << "❌ No se pudo abrir el archivo de índice: " << archivoIdx << endl;
        return false;
    }
    
    string linea;
    int palabrasCargadas = 0;
    while (getline(file, linea)) {
        if (linea.empty()) continue;
        
        size_t posFirstSemicolon = linea.find(';');
        if (posFirstSemicolon == string::npos) continue;
        
        string palabra = linea.substr(0, posFirstSemicolon);
        string resto = linea.substr(posFirstSemicolon + 1);
        
        stringstream ss(resto);
        string token;
        
        while (getline(ss, token, ';')) {
            if (token.empty()) continue;
            
            token.erase(remove(token.begin(), token.end(), '('), token.end());
            token.erase(remove(token.begin(), token.end(), ')'), token.end());
            
            size_t coma = token.find(',');
            if (coma != string::npos) {
                string doc = token.substr(0, coma);
                try {
                    int count = stoi(token.substr(coma + 1));
                    indiceInvertido[palabra][doc] = count;
                } catch (const exception& e) {
                    cerr << "⚠ Error parseando conteo para '" << palabra << "': " << e.what() << endl;
                }
            }
        }
        palabrasCargadas++;
    }
    
    file.close();
    cout << "✅ Motor: Índice cargado (" << palabrasCargadas << " palabras)" << endl;
    return true;
}

string MotorBusqueda::obtenerNombreReal(const string& idLibro) {
    auto it = mapeoLibros.find(idLibro);
    if (it != mapeoLibros.end()) {
        return it->second;
    }
    return "Libro_" + idLibro;
}

MotorBusqueda::MotorBusqueda(const string& archivoIdx, const string& carpetaLibros) {
    cargarMapeoLibros(carpetaLibros);
    if (!cargarIndice(archivoIdx)) {
        throw runtime_error("No se pudo cargar el índice");
    }
}

json MotorBusqueda::buscar(const string& consulta, int topK) {
    string consultaNormalizada = normalizar(consulta);
    
    auto it = indiceInvertido.find(consultaNormalizada);
    if (it == indiceInvertido.end()) {
        return json::array();
    }
    
    vector<pair<string, int>> resultados;
    for (const auto& [doc, count] : it->second) {
        resultados.push_back({doc, count});
    }
    
    sort(resultados.begin(), resultados.end(), 
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.second > b.second;
         });
    
    json respuesta = json::array();
    int limite = min(topK, (int)resultados.size());
    
    for (int i = 0; i < limite; i++) {
        json item = json::object();
        string nombreReal = obtenerNombreReal(resultados[i].first);
        item["Libro"] = nombreReal;
        item["Score"] = resultados[i].second;
        item["ID"] = resultados[i].first;
        respuesta.push_back(item);
    }
    
    return respuesta;
}

int MotorBusqueda::getPID() const {
    return getpid();
}

// Función principal del motor
int main(int argc, char* argv[]) {
    cout << "=== MOTOR DE BÚSQUEDA ===" << endl;
    cout << "PID: " << getpid() << endl;
    
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <archivo.idx> <consulta> [topK]" << endl;
        return 1;
    }
    
    string archivoIdx = argv[1];
    string consulta = argv[2];
    int topK = 3;
    
    if (argc >= 4) {
        try {
            topK = stoi(argv[3]);
        } catch (const exception& e) {
            cerr << "❌ Error en topK: " << e.what() << ", usando 3 por defecto" << endl;
            topK = 3;
        }
    }
    
    // Validar archivo
    ifstream file(archivoIdx);
    if (!file) {
        cerr << "❌ El archivo de índice no existe: " << archivoIdx << endl;
        
        // Respuesta de error en JSON
        json errorRespuesta = json::object();
        errorRespuesta["Error"] = "Archivo de índice no existe";
        errorRespuesta["MotorPID"] = getpid();
        cout << errorRespuesta.dump(4) << endl;
        
        return 1;
    }
    file.close();
    
    try {
        MotorBusqueda motor(archivoIdx);
        json resultados = motor.buscar(consulta, topK);
        
        // Crear respuesta JSON
        json respuesta = json::object();
        respuesta["MotorPID"] = motor.getPID();
        respuesta["Query"] = consulta;
        respuesta["TopK"] = topK;
        respuesta["Resultados"] = resultados;
        
        cout << respuesta.dump(4) << endl;
    } catch (const exception& e) {
        cerr << "❌ Error en motor de búsqueda: " << e.what() << endl;
        
        json errorRespuesta = json::object();
        errorRespuesta["Error"] = e.what();
        errorRespuesta["MotorPID"] = getpid();
        cout << errorRespuesta.dump(4) << endl;
        
        return 1;
    }
    
    return 0;
}
