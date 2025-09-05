#include "ArchivoManager.h"
#include <fstream>
#include <iostream>
using namespace std;

ArchivoManager::ArchivoManager(const string& archivo) : archivoUsuarios(archivo) {}

vector<Usuario> ArchivoManager::leerUsuarios() const {
    vector<Usuario> usuarios;
    ifstream file(archivoUsuarios);
    
    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo de usuarios: " << archivoUsuarios << endl;
        cerr << "El archivo no existe o no se puede acceder a él." << endl;
        return usuarios;
    }
    
    string linea;
    while (getline(file, linea)) {
        if (!linea.empty()) {
            usuarios.push_back(Usuario::fromString(linea));
        }
    }
    file.close();
    return usuarios;
}

void ArchivoManager::guardarUsuario(const Usuario& usuario) const {
    ofstream file(archivoUsuarios, ios::app);
    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo para escribir: " << archivoUsuarios << endl;
        return;
    }
    file << usuario.toString() << endl;
    file.close();
}

void ArchivoManager::eliminarUsuario(int id) const {
    vector<Usuario> usuarios = leerUsuarios();
    ofstream file(archivoUsuarios);
    
    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo para escribir: " << archivoUsuarios << endl;
        return;
    }
    
    for (const auto& usuario : usuarios) {
        if (usuario.id != id) {
            file << usuario.toString() << endl;
        }
    }
    file.close();
}

int ArchivoManager::obtenerProximoId() const {
    vector<Usuario> usuarios = leerUsuarios();

    if (usuarios.empty()) return 1;

    int maxId = 0;
    for (const auto& usuario : usuarios) {
        if (usuario.id > maxId) {
            maxId = usuario.id;
        }
    }

    for (int i = 1; i <= maxId; i++) {
            bool dispId = 0;

            for (const auto& usuario : usuarios) {
                if (usuario.id == i) {
                    dispId = 1;
                    break;
                }
            }

            if (!dispId) return i;
        }

    return maxId + 1;
}