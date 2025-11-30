#ifndef CONTEO_H
#define CONTEO_H

#include <string>
#include <map>
#include <vector>

int contar_palabras_desde_archivo(const std::string& rutaArchivo);

struct ResumenConteo {
    long vocales = 0;
    long consonantes = 0;
    long especiales = 0;
    long palabras = 0;
};


std::string normalizarEspanol(const std::string& s);


ResumenConteo contar(const std::string& texto);



#endif
