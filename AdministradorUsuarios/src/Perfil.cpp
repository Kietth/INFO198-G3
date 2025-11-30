#include "Perfil.h"
#include <algorithm>
#include <cctype>
#include <cstring>

PermisosPerfil::PermisosPerfil() : numPermisos(0) {
    nombre[0] = '\0';
}

PermisosPerfil::PermisosPerfil(const char* nombre, const int* permisos, int numPermisos) 
    : numPermisos(numPermisos) {
    
    strncpy(this->nombre, nombre, sizeof(this->nombre) - 1);
    this->nombre[sizeof(this->nombre) - 1] = '\0';
    
    // Convertir a mayúsculas
    for (char* p = this->nombre; *p; ++p) {
        *p = toupper(*p);
    }
    
    // Copiar permisos
    for (int i = 0; i < numPermisos && i < 10; ++i) {
        this->permisos[i] = permisos[i];
    }
}