#define USUARIO_H
#include <string>
using namespace std;

// Definición de la estructura Usuario
struct Usuario {
    int id;
    char nombre[20];
    char perfil[20];
};

// Declaración de las funciones:
void guardarUsuario(const Usuario& usuario, const string& nombreArchivo);
void eliminarUsuario(int ID, const string& nombreArchivo);
void listarUsuarios(const string& nombreArchivo);
