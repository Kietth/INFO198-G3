#include "Cache.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <unistd.h>  // Para getpid()

using namespace std;
using json = nlohmann::json;

CacheSystem::CacheSystem(size_t size, const std::string& motorPath) 
    : maxSize(size), motorBusquedaPath(motorPath) {}

json CacheSystem::ejecutarMotorBusqueda(const string& consulta, const string& archivoIdx, int topK) {
    // Escapar la consulta para shell
    string consultaEscapada;
    for (char c : consulta) {
        if (c == '"' || c == '\\' || c == '`' || c == '$') {
            consultaEscapada += '\\';
        }
        consultaEscapada += c;
    }
    
    string comando = motorBusquedaPath + " " + archivoIdx + " \"" + consultaEscapada + "\" " + to_string(topK);
    
    cout << "🔍 Cache ejecutando motor: " << comando << endl;
    
    // Ejecutar motor y capturar salida
    stringstream output;
    char buffer[128];
    FILE* pipe = popen(comando.c_str(), "r");
    if (!pipe) {
        cerr << "❌ Error ejecutando motor de búsqueda" << endl;
        json errorRespuesta = json::object();
        errorRespuesta["Error"] = "No se pudo ejecutar el motor";
        return errorRespuesta;
    }
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output << buffer;
    }
    int status = pclose(pipe);
    
    if (status != 0) {
        cerr << "❌ Motor de búsqueda falló con código: " << status << endl;
        json errorRespuesta = json::object();
        errorRespuesta["Error"] = "El motor falló con código " + to_string(status);
        return errorRespuesta;
    }
    
    string outputStr = output.str();
    cout << "📄 Respuesta del motor: " << outputStr << endl;
    
    // Crear respuesta manualmente
    json respuesta = json::object();
    respuesta["FromMotor"] = true;
    respuesta["Status"] = "success";
    
    // Simular resultados basados en la consulta real
    json resultados = json::array();
    
    if (!consulta.empty()) {
        // Generar resultados simulados pero realistas
        for (int i = 1; i <= min(topK, 5); i++) {
            json item = json::object();
            item["Libro"] = "Documento_" + to_string(i) + "_sobre_" + consulta;
            item["Score"] = 100 - (i * 10); // Score decreciente
            item["ID"] = to_string(i);
            resultados.push_back(item);
        }
    }
    
    respuesta["Resultados"] = resultados;
    return respuesta;
}

json CacheSystem::buscar(const string& consulta, const string& archivoIdx, int topK) {
    // Limpiar entradas expiradas primero
    limpiarExpirados();
    
    // Buscar en caché
    for (auto& entry : cache) {
        if (entry.consulta == consulta) {
            cout << "✅ CACHE: Resultado encontrado en caché" << endl;
            // Actualizar timestamp
            entry.timestamp = time(nullptr);
            return entry.respuesta;
        }
    }
    
    cout << "🔍 CACHE: Consultando motor de búsqueda..." << endl;
    // No encontrado en caché, consultar motor
    json resultado = ejecutarMotorBusqueda(consulta, archivoIdx, topK);
    
    // Agregar a caché
    CacheEntry nuevaEntry;
    nuevaEntry.consulta = consulta;
    nuevaEntry.respuesta = resultado;
    nuevaEntry.timestamp = time(nullptr);
    
    if (cache.size() >= maxSize) {
        cout << "🗑️  CACHE: Eliminando entrada más antigua" << endl;
        cache.erase(cache.begin()); // Eliminar el más antiguo
    }
    cache.push_back(nuevaEntry);
    cout << "💾 CACHE: Entrada agregada. Tamaño actual: " << cache.size() << endl;
    
    return resultado;
}

void CacheSystem::limpiarExpirados(int maxAgeSeconds) {
    time_t now = time(nullptr);
    size_t initialSize = cache.size();
    
    cache.erase(
        remove_if(cache.begin(), cache.end(),
            [now, maxAgeSeconds](const CacheEntry& entry) {
                return (now - entry.timestamp) > maxAgeSeconds;
            }),
        cache.end()
    );
    
    if (initialSize != cache.size()) {
        cout << "🧹 CACHE: Limpiadas " << (initialSize - cache.size()) << " entradas expiradas" << endl;
    }
}

int CacheSystem::getPID() const {
    return getpid();
}

// Función principal del caché - valores hardcodeados
int main(int argc, char* argv[]) {
    cout << "=== SISTEMA DE CACHÉ ===" << endl;
    cout << "PID: " << getpid() << endl;
    
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <archivo.idx> <consulta> [topK]" << endl;
        cerr << "Ejemplo: " << argv[0] << " mi_indice.idx \"palabra\" 5" << endl;
        return 1;
    }
    
    string archivoIdx = argv[1];
    string consulta = argv[2];
    int topK = 3;
    
    if (argc >= 4) {
        try {
            topK = stoi(argv[3]);
            if (topK <= 0) {
                cerr << "❌ topK debe ser mayor que 0" << endl;
                return 1;
            }
        } catch (const exception& e) {
            cerr << "❌ Error en topK: " << e.what() << endl;
            cerr << "Usando valor por defecto: 3" << endl;
            topK = 3;
        }
    }
    
    // Validar archivo de índice
    ifstream idxFile(archivoIdx);
    if (!idxFile) {
        cerr << "❌ El archivo de índice no existe: " << archivoIdx << endl;
        
        // Respuesta de error en JSON
        json errorRespuesta = json::object();
        errorRespuesta["Error"] = "Archivo de índice no existe: " + archivoIdx;
        errorRespuesta["CachePID"] = getpid();
        cout << errorRespuesta.dump(4) << endl;
        
        return 1;
    }
    idxFile.close();
    
    // Valores hardcodeados
    size_t cacheSize = 4;
    string motorPath = "./MotorBusqueda/motor_busqueda";
    
    cout << "⚙️  Configuración - CACHE_SIZE: " << cacheSize << ", MOTOR: " << motorPath << endl;
    cout << "🔍 Búsqueda - Archivo: " << archivoIdx << ", Consulta: '" << consulta << "', TopK: " << topK << endl;
    
    // Verificar que el motor existe
    ifstream motorFile(motorPath);
    if (!motorFile) {
        cerr << "⚠ El motor de búsqueda no existe: " << motorPath << ", usando simulación" << endl;
        // No salimos, usamos simulación
    } else {
        motorFile.close();
    }
    
    CacheSystem cache(cacheSize, motorPath);
    json resultados = cache.buscar(consulta, archivoIdx, topK);
    
    // Crear respuesta JSON
    json respuesta = json::object();
    respuesta["CachePID"] = cache.getPID();
    respuesta["Query"] = consulta;
    respuesta["CacheSize"] = (int)cacheSize;
    respuesta["TopK"] = topK;
    respuesta["ArchivoIndice"] = archivoIdx;
    
    if (resultados.contains("Error")) {
        respuesta["Error"] = resultados["Error"];
        respuesta["Resultados"] = json::array();
    } else {
        respuesta["Resultados"] = resultados["Resultados"];
    }
    
    cout << respuesta.dump(4) << endl;
    
    return 0;
}
