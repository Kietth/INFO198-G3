#include "conteo.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif

using namespace std;

//Mapa de secuencias UTF-8 comunes en español a ASCII base 
static const unordered_map<string, char> MAP_UTF8 = {
    {"á",'a'}, {"Á",'A'}, {"é",'e'}, {"É",'E'}, {"í",'i'}, {"Í",'I'},
    {"ó",'o'}, {"Ó",'O'}, {"ú",'u'}, {"Ú",'U'}, {"ü",'u'}, {"Ü",'U'},
    {"ñ",'n'}, {"Ñ",'N'}
};

static bool empiezaCon(const string& s, size_t i, const string& pat) {
    return s.size() >= i + pat.size() && s.compare(i, pat.size(), pat) == 0;
}

string normalizarEspanol(const string& s) {
    // Reemplaza acentos/ñ/ü por ASCII base. Cualquier multibyte no mapeado se deja tal cual.
    string out;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { // ASCII
            out.push_back(static_cast<char>(c));
            i++;
            continue;
        }
        bool matched = false;
        for (const auto& kv : MAP_UTF8) {
            const string& k = kv.first;
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
    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return c=='a'||c=='e'||c=='i'||c=='o'||c=='u';
}

ResumenConteo contar(const string& texto) {
    // 1) clasificar correctamente letras en español.
    string t = normalizarEspanol(texto);

    ResumenConteo r{};

    // 2) Conteo de palabras: separados por espacios en blanco, saltos o tabular
    {
        istringstream iss(t);
        string token;
        while (iss >> token) r.palabras++;
    }

    // 3) Clasificación de caracteres.
    for (unsigned char uc : t) {
        char c = static_cast<char>(uc);
        if (isalpha(static_cast<unsigned char>(c))) {
            if (esVocal(c)) r.vocales++;
            else r.consonantes++;
        } else if (isspace(static_cast<unsigned char>(c)) || isdigit(static_cast<unsigned char>(c))) {
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
        cout << "\n==== CONTEO SOBRE TEXTO ==== PID: " << obtenerPID() << endl;
        cout << "1) Ingresar texto manualmente\n";
        cout << "2) VOLVER\n";
        cout << "Seleccione una opcion: ";

        int op = 0;
        if (!(cin >> op)) {
            cin.clear();
            cin.ignore(1<<20, '\n');
            cout << "Entrada invalida.\n";
            continue;
        }
        cin.ignore(1<<20, '\n'); // limpiar fin de línea

        if (op == 2) {
            // VOLVER al menú principal
            return;
        } else if (op == 1) {
            cout << "\nEscriba el texto a analizar (puede pegar varias lineas y presionar Enter al final):\n> ";
            string texto;
            getline(cin, texto);

            ResumenConteo r = contar(texto);

            cout << "\n--- RESUMEN DE CONTEO ---\n";
            cout << "Vocales:               " << r.vocales      << "\n";
            cout << "Consonantes:           " << r.consonantes  << "\n";
            cout << "Caracteres especiales: " << r.especiales   << "\n";
            cout << "Palabras:              " << r.palabras     << "\n";

            cout << "\n(Enter) para VOLVER...";
            cin.get();
        } else {
            cout << "Opcion invalida.\n";
        }
    }
}
