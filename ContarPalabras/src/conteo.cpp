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

static const unordered_map<string, char> MAP_UTF8 = {
    {"á",'a'}, {"Á",'A'}, {"é",'e'}, {"É",'E'}, {"í",'i'}, {"Í",'I'},
    {"ó",'o'}, {"Ó",'O'}, {"ú",'u'}, {"Ú",'U'}, {"ü",'u'}, {"Ü",'U'},
    {"ñ",'n'}, {"Ñ",'N'}
};

static bool empiezaCon(const string& s, size_t i, const string& pat) {
    return s.size() >= i + pat.size() && s.compare(i, pat.size(), pat) == 0;
}

string normalizarEspanol(const string& s) {
    string out;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { 
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

static bool esVocal(char c) { 
    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return c=='a'||c=='e'||c=='i'||c=='o'||c=='u';
}

ResumenConteo contar(const string& texto) {
    string t = normalizarEspanol(texto);

    ResumenConteo r{};

    
    {
        istringstream iss(t);
        string token;
        while (iss >> token) r.palabras++;
    }

    
    for (unsigned char uc : t) {
        char c = static_cast<char>(uc);
        if (isalpha(static_cast<unsigned char>(c))) {
            if (esVocal(c)) r.vocales++;
            else r.consonantes++;
        } else if (isspace(static_cast<unsigned char>(c)) || isdigit(static_cast<unsigned char>(c))) {
        } else {
            r.especiales++;
        }
    }
    return r;
}


