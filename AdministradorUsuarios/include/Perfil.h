#ifndef PERFIL_H
#define PERFIL_H

#include <cstring>

struct PermisosPerfil {
    char nombre[20];
    int permisos[10]; // Máximo 10 permisos por perfil
    int numPermisos;
    
    PermisosPerfil();
    PermisosPerfil(const char* nombre, const int* permisos, int numPermisos);
};

#endif