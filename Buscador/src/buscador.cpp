#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include "json.hpp"

#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif

using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;

// Estructura para el caché
struct CacheEntry {
    string consulta;
    json respuesta;
};

class Buscador {
private:
    map<string, map<string, int>> indiceInvertido;
    vector<CacheEntry> cache;
    size_t cacheSize;
    
    // Normalizar palabra (quitar caracteres especiales y convertir a minúsculas)
    string normalizar(const string& palabra) {
        string out;
        for (char c : palabra) {
            if (isalpha((unsigned char)c)) {
                out.push_back(tolower(c));
            }
        }
        return out;
    }
    
    // Cargar índice invertido desde archivo
    bool cargarIndice(const string& archivoIdx) {
        ifstream file(archivoIdx);
        if (!file.is_open()) {
            cerr << "❌ No se pudo abrir el archivo de índice: " << archivoIdx << endl;
            return false;
        }
        
        string linea;
        while (getline(file, linea)) {
            if (linea.empty()) continue;
            
            size_t posFirstSemicolon = linea.find(';');
            if (posFirstSemicolon == string::npos) continue;
            
            string palabra = linea.substr(0, posFirstSemicolon);
            string resto = linea.substr(posFirstSemicolon + 1);
            
            // Parsear los documentos y conteos: (doc1,count1);(doc2,count2)...
            stringstream ss(resto);
            string token;
            
            while (getline(ss, token, ';')) {
                if (token.empty()) continue;
                
                // Remover paréntesis
                token.erase(remove(token.begin(), token.end(), '('), token.end());
                token.erase(remove(token.begin(), token.end(), ')'), token.end());
                
                size_t coma = token.find(',');
                if (coma != string::npos) {
                    string doc = token.substr(0, coma);
                    int count = stoi(token.substr(coma + 1));
                    indiceInvertido[palabra][doc] = count;
                }
            }
        }
        
        file.close();
        cout << "✅ Índice invertido cargado correctamente" << endl;
        return true;
    }
    
    // Buscar en caché
    json* buscarEnCache(const string& consulta) {
        for (auto& entry : cache) {
            if (entry.consulta == consulta) {
                return &entry.respuesta;
            }
        }
        return nullptr;
    }
    
    // Agregar a caché
    void agregarACache(const string& consulta, const json& respuesta) {
        if (cache.size() >= cacheSize) {
            cache.erase(cache.begin()); // Eliminar el más antiguo
        }
        cache.push_back({consulta, respuesta});
    }
    
public:
    Buscador(size_t tamCache = 4) : cacheSize(tamCache) {}
    
    bool inicializar(const string& archivoIdx) {
        return cargarIndice(archivoIdx);
    }
    
    json buscar(const string& consulta, int topK) {
        // Normalizar la consulta
        string consultaNormalizada = normalizar(consulta);
        
        // Buscar en caché
        json* enCache = buscarEnCache(consultaNormalizada);
        if (enCache != nullptr) {
            cout << "✅ Resultado encontrado en CACHÉ" << endl;
            json resultado = *enCache;
            
            // Limitar a TOPK
            if (resultado.size() > (size_t)topK) {
                json resultadoLimitado = json::array();
                size_t limite = min((size_t)topK, resultado.size());
                for (size_t i = 0; i < limite; i++) {
                    resultadoLimitado.push_back(resultado.begin()[i]);
                }
                return resultadoLimitado;
            }
            
            return resultado;
        }
        
        // No está en caché, buscar en el índice
        auto it = indiceInvertido.find(consultaNormalizada);
        
        if (it == indiceInvertido.end()) {
            // Palabra no encontrada
            return json::array();
        }
        
        // Crear lista de resultados
        vector<pair<string, int>> resultados;
        for (const auto& [doc, count] : it->second) {
            resultados.push_back({doc, count});
        }
        
        // Ordenar por score (conteo) descendente
        sort(resultados.begin(), resultados.end(), 
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });
        
        // Crear JSON de respuesta
        json respuesta = json::array();
        for (const auto& [doc, score] : resultados) {
            json item = json::object();
            item["Libro"] = doc;
            item["Score"] = score;
            respuesta.push_back(item);
        }
        
        // Agregar a caché
        agregarACache(consultaNormalizada, respuesta);
        
        // Retornar solo los TOPK elementos
        if (respuesta.size() > (size_t)topK) {
            json resultadoLimitado = json::array();
            size_t limite = min((size_t)topK, respuesta.size());
            for (size_t i = 0; i < limite; i++) {
                resultadoLimitado.push_back(respuesta.begin()[i]);
            }
            return resultadoLimitado;
        }
        
        return respuesta;
    }
    
    int getPID() const {
        return obtenerPID();
    }
};

int main(int argc, char* argv[]) {
    cout << "=== BUSCADOR DE ÍNDICE INVERTIDO === PID: " << obtenerPID() << endl;
    
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <archivo.idx>" << endl;
        return 1;
    }
    
    string archivoIdx = argv[1];
    
    // Validar extensión .idx
    if (archivoIdx.size() < 4 || archivoIdx.substr(archivoIdx.size() - 4) != ".idx") {
        cerr << "❌ El archivo debe terminar en .idx" << endl;
        return 1;
    }
    
    // Validar existencia del archivo
    if (!fs::exists(archivoIdx)) {
        cerr << "❌ El archivo no existe: " << archivoIdx << endl;
        return 1;
    }
    
    Buscador buscador;
    if (!buscador.inicializar(archivoIdx)) {
        return 1;
    }
    
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
        
        // TOPK fijo en 3 (puedes hacerlo configurable)
        int topK = 3;
        
        json resultados = buscador.buscar(consulta, topK);
        
        // Crear respuesta en formato JSON
        json respuesta = json::object();
        respuesta["Query"] = consulta;
        respuesta["PID"] = buscador.getPID();
        respuesta["Respuesta"] = resultados;
        
        // Imprimir respuesta formateada
        cout << "\n" << respuesta.dump(4) << "\n" << endl;
    }
    
    return 0;
}
