#ifndef CONTEO_H
#define CONTEO_H

#include <string>

struct ResumenConteo {
    long vocales = 0;
    long consonantes = 0;
    long especiales = 0;
    long palabras = 0;
};

/// Normaliza tildes/diéresis/ñ comunes en español a ASCII base (á→a, ñ→n, ü→u, etc.)
std::string normalizarEspanol(const std::string& s);

/// Calcula el resumen de conteo sobre el texto dado (vocales, consonantes, especiales, palabras).
ResumenConteo contar(const std::string& texto);

/// Interfaz de la opción (6) CONTEO SOBRE TEXTO (solo entrada por consola) + VOLVER.
void interfazConteo();

#endif
