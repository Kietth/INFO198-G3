#include "ArchivoManager.h"
#include <fstream>
#include <iostream>
#include <cstring>
using namespace std;

ArchivoManager::ArchivoManager(const string& archivo) : archivoUsuarios(archivo) {}

vector<Usuario> ArchivoManager::leerUsuarios() const {
    vector<Usuario> usuarios;
    ifstream file(archivoUsuarios, ios::binary);
    
    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo de usuarios: " << archivoUsuarios << endl;
        cerr << "El archivo no existe o no se puede acceder a él." << endl;
        return usuarios;
    }
    
    // Leer structs completos en binario hasta encontrar uno vacío
    bool reading = true;
    while (reading) {
        Usuario usuario;
        file.read(reinterpret_cast<char*>(&usuario), sizeof(Usuario));
        
        // Verificar si llegamos al final del archivo o encontramos usuario vacío
        if (file.eof() || usuario.id == 0) {
            reading = false;
            break;
        }
        
        usuarios.push_back(usuario);
    }
    
    file.close();
    return usuarios;
}

void ArchivoManager::guardarUsuario(const Usuario& usuario) const {
    ofstream file(archivoUsuarios, ios::binary | ios::app);
    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo para escribir: " << archivoUsuarios << endl;
        return;
    }
    
    // Escribir struct completo en binario
    file.write(reinterpret_cast<const char*>(&usuario), sizeof(Usuario));
    file.close();
}

void ArchivoManager::eliminarUsuario(int id) const {
    vector<Usuario> usuarios = leerUsuarios();
    ofstream file(archivoUsuarios, ios::binary);
    
    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo para escribir: " << archivoUsuarios << endl;
        return;
    }
    
    for (const auto& usuario : usuarios) {
        if (usuario.id != id) {
            file.write(reinterpret_cast<const char*>(&usuario), sizeof(Usuario));
        }
    }
    
    // Escribir usuario vacío al final como marcador
    Usuario usuarioVacio;
    file.write(reinterpret_cast<const char*>(&usuarioVacio), sizeof(Usuario));
    
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
        bool dispId = false;
        for (const auto& usuario : usuarios) {
            if (usuario.id == i) {
                dispId = true;
                break;
            }
        }
        if (!dispId) return i;
    }

    return maxId + 1;
}