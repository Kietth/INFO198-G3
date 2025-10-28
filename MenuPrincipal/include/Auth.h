#ifndef AUTH_H
#define AUTH_H

#include "Usuario.h"
#include <string>
#include <map>
#include <vector>

class Auth {
private:
    std::vector<Usuario> usuarios;
    std::map<std::string, std::vector<int>> permisos;
    Usuario usuarioActual;

    void cargarUsuarios(const std::string& archivoUsuarios);
    void cargarPerfiles(const std::string& archivoPerfiles);

public:
    Auth(const std::string& archivoUsuarios, const std::string& archivoPerfiles);
    bool login(const std::string& user, const std::string& pass);
    bool tienePermiso(int opcion);
    std::string getPerfil() const;
    std::string getNombre() const;
};

#endif
