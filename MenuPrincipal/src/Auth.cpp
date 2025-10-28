// === MODIFICACIÓN EN MenuPrincipal/src/Auth.cpp ===
#include "Auth.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
using namespace std;

Auth::Auth(const string& archivoUsuarios, const string& archivoPerfiles) {
    cout << "Cargando usuarios desde: " << archivoUsuarios << endl;
    cout << "Cargando perfiles desde: " << archivoPerfiles << endl;
    cargarUsuarios(archivoUsuarios);
    cargarPerfiles(archivoPerfiles);
}

void Auth::cargarUsuarios(const string& archivoUsuarios) {
    ifstream file(archivoUsuarios, ios::binary);
    
    if (!file.is_open()) {
        cout << "⚠ No se pudo abrir el archivo de usuarios: " << archivoUsuarios << endl;
        return;
    }
    
    // Leer usuarios en formato binario
    while (file) {
        Usuario usuario;
        file.read(reinterpret_cast<char*>(&usuario), sizeof(Usuario));
        
        // Verificar si es un usuario válido (ID != 0)
        if (usuario.id != 0) {
            usuarios.push_back(usuario);
        }
    }
    
    file.close();
    
    if (usuarios.empty()) {
        cout << "⚠ No hay usuarios registrados en el archivo binario." << endl;
    } else {
        cout << "✓ " << usuarios.size() << " usuarios cargados desde archivo binario." << endl;
    }
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
                try {
                    listaPermisos.push_back(stoi(token));
                } catch (...) {
                    // Ignorar tokens inválidos
                }
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
        // Comparar username y password (son arrays de char, convertimos a string para comparar)
        string username_str(u.username);
        string password_str(u.password);
        
        // Eliminar caracteres nulos extras
        username_str = username_str.c_str();
        password_str = password_str.c_str();
        
        if (username_str == user && password_str == pass) {
            usuarioActual = u;
            return true;
        }
    }
    return false;
}

bool Auth::tienePermiso(int opcion) {
    string perfil_str(usuarioActual.perfil);
    perfil_str = perfil_str.c_str(); // Eliminar caracteres nulos extras
    
    auto it = permisos.find(perfil_str);
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