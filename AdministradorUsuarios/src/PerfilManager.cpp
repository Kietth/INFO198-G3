#include "PerfilManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

PerfilManager::PerfilManager(const string& archivo) : archivoPerfiles(archivo) {
    cargarPermisos();
}

void PerfilManager::cargarPermisos() {
    ifstream file(archivoPerfiles);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de perfiles: " << archivoPerfiles << endl;
        return;
    }
    
    string linea;
    while (getline(file, linea)) {
        size_t pos = linea.find(';');
        if (pos != string::npos) {
            string perfil = linea.substr(0, pos);
            string permisosStr = linea.substr(pos + 1);
            
            vector<int> permisos;
            stringstream ss(permisosStr);
            string permiso;
            
            while (getline(ss, permiso, ',')) {
                permisos.push_back(stoi(permiso));
            }
            
            permisosPerfiles[perfil] = permisos;
        }
    }
    file.close();
}

bool PerfilManager::tienePermiso(const string& perfil, int opcionMenu) const {
    auto it = permisosPerfiles.find(perfil);
    if (it != permisosPerfiles.end()) {
        for (int permiso : it->second) {
            if (permiso == opcionMenu) {
                return true;
            }
        }
    }
    return false;
}

vector<string> PerfilManager::getPerfiles() const {
    vector<string> perfiles;
    for (const auto& pair : permisosPerfiles) {
        perfiles.push_back(pair.first);
    }
    return perfiles;
}

vector<int> PerfilManager::getPermisos(const string& perfil) const {
    auto it = permisosPerfiles.find(perfil);
    if (it != permisosPerfiles.end()) {
        return it->second;
    }
    return {};
}

bool PerfilManager::existePerfil(const string& perfil) const {
    return permisosPerfiles.find(perfil) != permisosPerfiles.end();
}