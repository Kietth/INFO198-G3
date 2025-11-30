#ifndef BUSCADORCACHE_H
#define BUSCADORCACHE_H

#include <string>
#include "json.hpp"  // Ahora está en el mismo directorio include

class BuscadorCache {
private:
    std::string cachePath;
    
public:
    BuscadorCache();
    nlohmann::json buscar(const std::string& archivoIdx, const std::string& consulta, int topK);
    int getPID() const;
};

#endif
