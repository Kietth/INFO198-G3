#pragma once
#include <string>
#include <map>

using namespace std;

map<string, string> cargarEnv(const string& filename);

string getJuegoDir();
string getJuegoServerBin();
string getJuegoClientBin();
string getJuegoScript();