#ifndef SISTEMA_H
#define SISTEMA_H

#include <vector>
#include <string>
#include "Usuario.h"
#include "ArchivoManager.h"
#include "PerfilManager.h"
using namespace std;

class Sistema {
private:
    vector<Usuario> usuarios;
    ArchivoManager archivoManager;
    PerfilManager perfilManager;
    bool datosEnMemoria;
    
    void cargarUsuarios();
    void mostrarMenu() const;
    void ingresarUsuario();
    void listarUsuarios() const;
    void eliminarUsuario();
    void gestionarPerfiles();
    void mostrarMenuPerfiles() const;
    void listarPerfiles() const;
    
public:
    Sistema(const string& archivoUsuarios, const string& archivoPerfiles);
    void ejecutar();
};

#endif