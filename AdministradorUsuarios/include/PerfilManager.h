#ifndef PERFILMANAGER_H
#define PERFILMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "Perfil.h"
using namespace std;

class PerfilManager {
private:
    string archivoPerfiles;
    map<string, vector<int>> permisosPerfiles;
    
    void cargarPermisos();
    
public:
    PerfilManager(const string& archivo);
    
    bool tienePermiso(const string& perfil, int opcionMenu) const;
    vector<string> getPerfiles() const;
    vector<int> getPermisos(const string& perfil) const;
    bool existePerfil(const string& perfil) const;
};

#endif