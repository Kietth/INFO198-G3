#ifndef ARCHIVOMANAGER_H
#define ARCHIVOMANAGER_H

#include <string>
#include <vector>
#include "Usuario.h"
using namespace std;

class ArchivoManager {
private:
    string archivoUsuarios;
    
public:
    ArchivoManager(const string& archivo);
    
    vector<Usuario> leerUsuarios() const;
    void guardarUsuario(const Usuario& usuario) const;
    void eliminarUsuario(int id) const;
    int obtenerProximoId() const;
};

#endif