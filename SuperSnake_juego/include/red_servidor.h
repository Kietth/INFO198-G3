#ifndef RED_SERVIDOR_H
#define RED_SERVIDOR_H

#include <string>
#include "estado_servidor.h" // <-- CAMBIADO

void manejarCliente(int clientSocket, int playerID);
std::string serializarEstado(Partida& partida, int mi_player_id = -1);

#endif // RED_SERVIDOR_H