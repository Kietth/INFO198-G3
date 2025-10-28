#include "env_utils.h"
#include <fstream>
#include <string>
#include <algorithm> 

using namespace std;


string trim(const string& str) {
    auto first = find_if_not(str.begin(), str.end(), ::isspace);
    auto last = find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    return (last <= first ? "" : string(first, last));
}



map<string, string> cargarEnv(const string& ruta) {
    map<string, string> envMap;
    ifstream file(ruta);
    string line;

    if (!file.is_open()) {
        return envMap; 
    }

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos == string::npos) {
            continue; 
        }

        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));
        
        if (key.empty() || value.empty()) {
            continue;
        }

        envMap[key] = value;
    }

    file.close();
    return envMap;
}