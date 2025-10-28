#include "env_utils.h"
#include <fstream>
#include <sstream>

using namespace std;

map<string, string> cargarEnv(const string& filename){
    map<string, string> env;
    ifstream file(filename);
    string line;
    while(getline(file, line)){
        istringstream is_line(line);
        string key;
        if(getline(is_line, key, '=')){
            string value;
            if(getline(is_line, value)){
                key.erase(key.find_last_not_of(" \n\r\t")+1);
                value.erase(0, value.find_first_not_of(" \n\r\t"));
                env[key] = value;
            }
        }
    }
    return env;
}