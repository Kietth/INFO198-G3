#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <map>

class ConfigManager {
private:
    std::map<std::string, std::string> configuraciones;
    std::string archivoEnv;
    
    void cargarConfiguracion();
    void setVariableEntorno(const std::string& clave, const std::string& valor);

public:
    ConfigManager(const std::string& archivo = ".env");
    void inicializar();
    std::string obtenerValor(const std::string& clave) const;
};

#endif