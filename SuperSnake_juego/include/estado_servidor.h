#ifndef ESTADO_SERVIDOR_H
#define ESTADO_SERVIDOR_H

#include <map>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>
#include "common.h"

enum GameMode { SOLO, DUO };

// Estructuras de datos
struct Jugador {
    int socket;
    std::vector<Posicion> cuerpo;
    std::string direccion = "UP";
    std::string direccion_siguiente = "UP";
    bool estaVivo = true;
    bool boosting = false;
    int equipo_id = -1; 
};

enum EstadoPartida { LOBBY, COUNTDOWN, PLAYING, ENDED };

struct Partida {
    int id;
    EstadoPartida estado = LOBBY;
    GameMode modo = SOLO; 
    std::map<int, Jugador> jugadores;
    std::vector<Posicion> comida;
    std::chrono::steady_clock::time_point tiempoInicioCountdown;
    int grid_size = 50; // <-- AÑADIDO (default)
};

// Constantes
extern const std::chrono::milliseconds GAME_TICK_RATE;
// --- CAMBIOS AQUÍ ---
extern const int MAX_PLAYERS_SOLO;
extern const int MAX_PLAYERS_DUO;
// --- FIN CAMBIOS ---
extern const int COUNTDOWN_SECONDS;

// Estado Global
extern std::mutex g_mutex;
extern std::map<int, Partida> g_partidas;
extern std::map<int, int> g_jugador_a_partida_id;
extern int g_next_player_id;
extern int g_next_partida_id;

#endif // ESTADO_SERVIDOR_H