#ifndef MOTORBUSQUEDA_H
#define MOTORBUSQUEDA_H

#include <string>
#include <map>
#include <vector>

// Incluir json.hpp desde Buscador/include
#include "../../Buscador/include/json.hpp"

class MotorBusqueda {
private:
    std::map<std::string, std::map<std::string, int>> indiceInvertido;
    std::map<std::string, std::string> mapeoLibros;
    
    std::string normalizar(const std::string& palabra);
    bool cargarIndice(const std::string& archivoIdx);
    std::string obtenerNombreReal(const std::string& idLibro);
    void cargarMapeoLibros(const std::string& carpetaLibros);

public:
    MotorBusqueda(const std::string& archivoIdx, const std::string& carpetaLibros = "");
    nlohmann::json buscar(const std::string& consulta, int topK);
    int getPID() const;
};

#endif
