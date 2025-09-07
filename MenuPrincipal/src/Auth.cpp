#include "Auth.h"
#include "ArchivoManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

Auth::Auth(const string& archivoUsuarios, const string& archivoPerfiles) {
    cargarUsuarios(archivoUsuarios);
    cargarPerfiles(archivoPerfiles);
}

void Auth::cargarUsuarios(const string& archivoUsuarios) {
    ArchivoManager manager(archivoUsuarios);
    usuarios = manager.leerUsuarios();
}

void Auth::cargarPerfiles(const string& archivoPerfiles) {
    ifstream file(archivoPerfiles);
    string linea;
    while (getline(file, linea)) {
        if (linea.empty()) continue;
        size_t pos = linea.find(';');
        if (pos != string::npos) {
            string perfil = linea.substr(0, pos);
            string permisosStr = linea.substr(pos + 1);

            vector<int> listaPermisos;
            stringstream ss(permisosStr);
            string token;
            while (getline(ss, token, ',')) {
                listaPermisos.push_back(stoi(token));
            }
            permisos[perfil] = listaPermisos;
        }
    }
}

bool Auth::login(const string& user, const string& pass) {
    if (usuarios.empty()) {
        cout << "⚠ No hay usuarios registrados. Ejecute el administrador de usuarios.\n";
        return false;
    }

    for (auto& u : usuarios) {
        if (string(u.username) == user && string(u.password) == pass) {
            usuarioActual = u;
            return true;
        }
    }
    return false;
}

bool Auth::tienePermiso(int opcion) {
    auto it = permisos.find(string(usuarioActual.perfil));
    if (it != permisos.end()) {
        for (int p : it->second) {
            if (p == opcion) return true;
        }
    }
    return false;
}

string Auth::getPerfil() const {
    return string(usuarioActual.perfil);
}

string Auth::getNombre() const {
    return string(usuarioActual.nombre);
}
