#ifndef LOGICA_H
#define LOGICA_H

#include "estado_servidor.h" // <-- Esto ya incluye 'GameMode'

void gameLoop();
void moverSerpiente(int playerID, Jugador& jugador, Partida& partida);
void generarComida(Partida& partida);
Posicion generarSpawnSeguro(Partida& partida);

// --- LÍNEA CORREGIDA ---
int encontrarOcrearPartida(int playerID, int clientSocket, GameMode modoBuscado);
// --- FIN DE LA CORRECCIÓN ---

#endif // LOGICA_H