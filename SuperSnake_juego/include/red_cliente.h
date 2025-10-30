#ifndef RED_CLIENTE_H
#define RED_CLIENTE_H

#include <string>
#include <vector>

// Conexión y recepción
bool connect_to_server(int& outSocket);
void receive_data_thread();

// Utilidades de red
std::vector<std::string> split(const std::string& s, char delimiter);

#endif // RED_CLIENTE_H