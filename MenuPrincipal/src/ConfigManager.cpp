#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
using namespace std;

ConfigManager::ConfigManager(const std::string& archivo) : archivoEnv(archivo) {
    cargarConfiguracion();
}

void ConfigManager::cargarConfiguracion() {
    ifstream file(archivoEnv);
    if (!file.is_open()) {
        cerr << "ADVERTENCIA: No se pudo abrir el archivo " << archivoEnv << endl;
        cerr << "Usando valores por defecto." << endl;
        return;
    }
    
    string linea;
    while (getline(file, linea)) {
        // Saltar líneas vacías y comentarios
        if (linea.empty() || linea[0] == '#') {
            continue;
        }
        
        size_t pos = linea.find('=');
        if (pos != string::npos) {
            string clave = linea.substr(0, pos);
            string valor = linea.substr(pos + 1);
            
            // Eliminar espacios en blanco
            clave.erase(0, clave.find_first_not_of(" \t"));
            clave.erase(clave.find_last_not_of(" \t") + 1);
            
            valor.erase(0, valor.find_first_not_of(" \t"));
            valor.erase(valor.find_last_not_of(" \t") + 1);
            
            configuraciones[clave] = valor;
        }
    }
    file.close();
}

void ConfigManager::setVariableEntorno(const std::string& clave, const std::string& valor) {
    #ifdef _WIN32
        _putenv_s(clave.c_str(), valor.c_str());
    #else
        setenv(clave.c_str(), valor.c_str(), 1);
    #endif
}

void ConfigManager::inicializar() {
    cout << "=== CARGANDO CONFIGURACIÓN DESDE .env ===" << endl;
    
    for (const auto& config : configuraciones) {
        setVariableEntorno(config.first, config.second);
        cout << config.first << " = " << config.second << endl;
    }
    
    cout << "========================================" << endl;
}

std::string ConfigManager::obtenerValor(const std::string& clave) const {
    auto it = configuraciones.find(clave);
    if (it != configuraciones.end()) {
        return it->second;
    }
    
    // Si no está en el archivo, intentar obtener de las variables de entorno existentes
    const char* valor = getenv(clave.c_str());
    return valor ? valor : "";
}