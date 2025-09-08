#include "conteo.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <unordered_map>

//Mapa de secuencias UTF-8 comunes en español a ASCII base 
static const std::unordered_map<std::string, char> MAP_UTF8 = {
    {"á",'a'}, {"Á",'A'}, {"é",'e'}, {"É",'E'}, {"í",'i'}, {"Í",'I'},
    {"ó",'o'}, {"Ó",'O'}, {"ú",'u'}, {"Ú",'U'}, {"ü",'u'}, {"Ü",'U'},
    {"ñ",'n'}, {"Ñ",'N'}
};

static bool empiezaCon(const std::string& s, size_t i, const std::string& pat) {
    return s.size() >= i + pat.size() && s.compare(i, pat.size(), pat) == 0;
}

std::string normalizarEspanol(const std::string& s) {
    // Reemplaza acentos/ñ/ü por ASCII base. Cualquier multibyte no mapeado se deja tal cual.
    std::string out;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { // ASCII
            out.push_back(static_cast<char>(c));
            i++;
            continue;
        }
        bool matched = false;
        for (const auto& kv : MAP_UTF8) {
            const std::string& k = kv.first;
            if (empiezaCon(s, i, k)) {
                out.push_back(kv.second);
                i += k.size();
                matched = true;
                break;
            }
        }
        if (!matched) { 
            out.push_back(s[i]);
            i++;
        }
    }
    return out;
}

static bool esVocal(char c) { // vocales
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return c=='a'||c=='e'||c=='i'||c=='o'||c=='u';
}

ResumenConteo contar(const std::string& texto) {
    // 1) clasificar correctamente letras en español.
    std::string t = normalizarEspanol(texto);

    ResumenConteo r{};

    // 2) Conteo de palabras: separados por espacios en blanco, saltos o tabular
    {
        std::istringstream iss(t);
        std::string token;
        while (iss >> token) r.palabras++;
    }

    // 3) Clasificación de caracteres.
    for (unsigned char uc : t) {
        char c = static_cast<char>(uc);
        if (std::isalpha(static_cast<unsigned char>(c))) {
            if (esVocal(c)) r.vocales++;
            else r.consonantes++;
        } else if (std::isspace(static_cast<unsigned char>(c)) || std::isdigit(static_cast<unsigned char>(c))) {
            // Espacios y dígitos NO cuentan como "especiales"
        } else {
            // Puntuación, símbolos...
            r.especiales++;
        }
    }
    return r;
}

void interfazConteo() {
    while (true) {
        std::cout << "\n==== (6) CONTEO SOBRE TEXTO ====\n";
        std::cout << "1) Ingresar texto manualmente\n";
        std::cout << "2) VOLVER\n";
        std::cout << "Seleccione una opcion: ";

        int op = 0;
        if (!(std::cin >> op)) {
            std::cin.clear();
            std::cin.ignore(1<<20, '\n');
            std::cout << "Entrada invalida.\n";
            continue;
        }
        std::cin.ignore(1<<20, '\n'); // limpiar fin de línea

        if (op == 2) {
            // VOLVER al menú principal
            return;
        } else if (op == 1) {
            std::cout << "\nEscriba el texto a analizar (puede pegar varias lineas y presionar Enter al final):\n> ";
            std::string texto;
            std::getline(std::cin, texto);

            ResumenConteo r = contar(texto);

            std::cout << "\n--- RESUMEN DE CONTEO ---\n";
            std::cout << "Vocales:               " << r.vocales      << "\n";
            std::cout << "Consonantes:           " << r.consonantes  << "\n";
            std::cout << "Caracteres especiales: " << r.especiales   << "\n";
            std::cout << "Palabras:              " << r.palabras     << "\n";

            std::cout << "\n(Enter) para VOLVER...";
            std::cin.get();
        } else {
            std::cout << "Opcion invalida.\n";
        }
    }
}
