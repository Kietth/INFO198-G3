#ifndef COMMON_H
#define COMMON_H

// Constantes compartidas
const int PORT = 51234;

// Estructura de posición compartida
struct Posicion {
    int x; 
    int y;
    bool operator==(const Posicion& other) const { return x == other.x && y == other.y; }
};

#endif // COMMON_H