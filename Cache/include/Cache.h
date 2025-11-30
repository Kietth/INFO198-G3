#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>
#include <map>

// Incluir json.hpp desde Buscador/include
#include "../../Buscador/include/json.hpp"

struct CacheEntry {
    std::string consulta;
    nlohmann::json respuesta;
    time_t timestamp;
};

class CacheSystem {
private:
    std::vector<CacheEntry> cache;
    size_t maxSize;
    std::string motorBusquedaPath;
    
    nlohmann::json ejecutarMotorBusqueda(const std::string& consulta, const std::string& archivoIdx, int topK);
    
public:
    CacheSystem(size_t size, const std::string& motorPath);
    nlohmann::json buscar(const std::string& consulta, const std::string& archivoIdx, int topK);
    void limpiarExpirados(int maxAgeSeconds = 300); // 5 minutos por defecto
    int getPID() const;
};

#endif
