#include "logica.h"
#include "red_servidor.h" // Necesita serializarEstado para el gameLoop
#include <iostream>
#include <vector>
#include <algorithm>
#include <map> 

#include <sys/socket.h> // Para send() y MSG_NOSIGNAL
#include <thread>       // Para this_thread
#include <chrono>       // Para las duraciones de sleep_for

using namespace std;

// Definición de globales del servidor
mutex g_mutex;
map<int, Partida> g_partidas;
map<int, int> g_jugador_a_partida_id;
int g_next_player_id = 1;
int g_next_partida_id = 1;

// Definición de constantes del servidor
const chrono::milliseconds GAME_TICK_RATE(100);

const int MAX_PLAYERS_SOLO = 6;
const int MAX_PLAYERS_DUO = 12;
const int COUNTDOWN_SECONDS = 15;


void generarComida(Partida& partida) {
    if (partida.comida.empty() || (partida.estado == PLAYING && partida.comida.size() < 25 && rand() % 100 < 25)) {
        Posicion new_food; 
        bool position_ok = false; 
        int attempts = 0;
        
        while(!position_ok && attempts < 50) {
             new_food = {rand() % partida.grid_size, rand() % partida.grid_size}; 
             position_ok = true;
             
             for(auto const& [player_id, jugador] : partida.jugadores) { 
                 for(auto const& segmento : jugador.cuerpo) { 
                     if (segmento == new_food) { 
                         position_ok = false; 
                         break; 
                     } 
                 } 
                 if (!position_ok) break; 
             }
             
             if (position_ok) { 
                 for(auto const& comida_existente : partida.comida) { 
                     if (comida_existente == new_food) { 
                         position_ok = false; 
                         break; 
                     } 
                 } 
             }
             attempts++;
        } 
        
        if (position_ok) { 
            partida.comida.push_back(new_food); 
        }
    }
}

void moverSerpiente(int playerID, Jugador& jugador, Partida& partida) {
    if (!jugador.estaVivo) return;

    jugador.direccion = jugador.direccion_siguiente;
    
    Posicion cabeza_nueva = jugador.cuerpo.front();
    if (jugador.direccion == "UP")    cabeza_nueva.y--; 
    else if (jugador.direccion == "DOWN") cabeza_nueva.y++;
    else if (jugador.direccion == "LEFT") cabeza_nueva.x--; 
    else if (jugador.direccion == "RIGHT") cabeza_nueva.x++;

    if (cabeza_nueva.x < 0) cabeza_nueva.x = partida.grid_size - 1; 
    else if (cabeza_nueva.x >= partida.grid_size) cabeza_nueva.x = 0;
    if (cabeza_nueva.y < 0) cabeza_nueva.y = partida.grid_size - 1; 
    else if (cabeza_nueva.y >= partida.grid_size) cabeza_nueva.y = 0;

    bool come = false; 
    int comida_index = -1;
    for (size_t i = 0; i < partida.comida.size(); ++i) { 
        if (partida.comida[i] == cabeza_nueva) { 
            come = true; 
            comida_index = i; 
            break; 
        } 
    }

    for (size_t i = 0; i < jugador.cuerpo.size(); ++i) { 
        if (i == jugador.cuerpo.size() - 1 && !come) continue; 
        if (cabeza_nueva == jugador.cuerpo[i]) { 
            jugador.estaVivo = false; 
            return; 
        } 
    }
    
    for (const auto& other_pair : partida.jugadores) { 
        if (other_pair.first == playerID) continue; 
        if (!other_pair.second.estaVivo) continue; 
        
        const Jugador& otra = other_pair.second;

        if (partida.modo == DUO && otra.equipo_id == jugador.equipo_id) {
            continue;
        }
        
        if (cabeza_nueva == otra.cuerpo.front()) { 
            jugador.estaVivo = false; 
            partida.jugadores[other_pair.first].estaVivo = false; 
            return; 
        } 
        for (const auto& seg : otra.cuerpo) { 
            if (cabeza_nueva == seg) { 
                jugador.estaVivo = false; 
                return; 
            } 
        } 
    }

    jugador.cuerpo.insert(jugador.cuerpo.begin(), cabeza_nueva);

    if (come) { 
        if (comida_index != -1 && comida_index < (int)partida.comida.size()) { 
            partida.comida.erase(partida.comida.begin() + comida_index); 
            generarComida(partida); 
        } 
    } else { 
        if (!jugador.cuerpo.empty()) { 
            jugador.cuerpo.pop_back(); 
        } 
    }
}


// --- BUCLE DEL JUEGO ---
void gameLoop() {
     while (true) {
        auto start_time = chrono::steady_clock::now();
        map<int, string> mensajes_por_socket;
        
        { 
            lock_guard<mutex> lock(g_mutex); 
            vector<int> partidas_a_eliminar;
            
            for (auto& pair_partida : g_partidas) {
                Partida& partida = pair_partida.second;

                if (partida.jugadores.empty() && partida.estado != LOBBY) { 
                    partidas_a_eliminar.push_back(partida.id); 
                    continue; 
                }

                if (partida.estado == LOBBY) { 
                    bool start_solo = (partida.modo == SOLO && partida.jugadores.size() >= 2);
                    // --- Dúo inicia con 4, 6, 8, 10 o 12 ---
                    bool start_duo = (partida.modo == DUO && 
                                      partida.jugadores.size() >= 4 && 
                                      partida.jugadores.size() % 2 == 0);
                    
                    if (start_solo || start_duo) {
                        partida.estado = COUNTDOWN; 
                        partida.tiempoInicioCountdown = chrono::steady_clock::now(); 
                    }
                }
                else if (partida.estado == COUNTDOWN) {
                    bool revert_solo = (partida.modo == SOLO && partida.jugadores.size() < 2);
                    // --- Vuelve a LOBBY si no es par >= 4 ---
                    bool revert_duo = (partida.modo == DUO && 
                                       (partida.jugadores.size() < 4 || partida.jugadores.size() % 2 != 0));
                    
                    if (revert_solo || revert_duo) {
                        partida.estado = LOBBY; 
                    }
                    else if (chrono::steady_clock::now() > (partida.tiempoInicioCountdown + chrono::seconds(COUNTDOWN_SECONDS))) {
                        bool can_start_solo = (partida.modo == SOLO && partida.jugadores.size() >= 2);
                        bool can_start_duo = (partida.modo == DUO && 
                                              partida.jugadores.size() >= 4 && 
                                              partida.jugadores.size() % 2 == 0);

                        if (can_start_solo || can_start_duo) {
                            partida.estado = PLAYING; 
                        } else { 
                            partida.estado = LOBBY; 
                        } 
                    } 
                }

                if (partida.estado == PLAYING) {
                    generarComida(partida); 
                    
                    int vivos = 0;
                    map<int, bool> equipos_vivos; 

                    for (auto& pair_jugador : partida.jugadores) {
                        int player_id = pair_jugador.first; 
                        Jugador& jugador = pair_jugador.second;
                        
                        int movs = jugador.boosting ? 2 : 1;
                        for (int i = 0; i < movs && jugador.estaVivo; ++i) { 
                            moverSerpiente(player_id, jugador, partida); 
                        }
                        
                        if (jugador.estaVivo) { 
                            vivos++; 
                            if (partida.modo == DUO) {
                                equipos_vivos[jugador.equipo_id] = true;
                            }
                        }
                    } 
                    
                    bool min_players_solo = partida.jugadores.size() >= 2;
                    bool min_players_duo = partida.jugadores.size() >= 4; // Mínimo 4 para terminar
                    bool solo_end = (partida.modo == SOLO && vivos <= 1 && min_players_solo);
                    bool duo_end = (partida.modo == DUO && equipos_vivos.size() <= 1 && min_players_duo);

                    if (solo_end || duo_end) { 
                        partida.estado = ENDED; 
                    }
                }
                
                for (auto& pair_jugador : partida.jugadores) { 
                    mensajes_por_socket[pair_jugador.second.socket] = serializarEstado(partida, pair_jugador.first); 
                }
            } 
            
            for (int id : partidas_a_eliminar) { 
                g_partidas.erase(id); 
                cout << "Partida #" << id << " eliminada." << endl; 
            }
        } // Fin del lock

        for (auto const& [socket, msg] : mensajes_por_socket) { 
            send(socket, msg.c_str(), msg.length(), MSG_NOSIGNAL); 
        }
        
        auto end_time = chrono::steady_clock::now(); 
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        if (elapsed < GAME_TICK_RATE) { 
            this_thread::sleep_for(GAME_TICK_RATE - elapsed); 
        }
    }
}

Posicion generarSpawnSeguro(Partida& partida) {
    bool spawn_ok = false; 
    Posicion spawn_pos; 
    int attempts = 0;
    
    while(!spawn_ok && attempts < 50) {
        spawn_pos = {rand() % partida.grid_size, rand() % partida.grid_size}; 
        spawn_ok = true;
        
        for(auto& pair_jugador : partida.jugadores) { 
            for(auto& segmento : pair_jugador.second.cuerpo) { 
                if (segmento == spawn_pos || (segmento.x == spawn_pos.x && segmento.y == (spawn_pos.y + 1) % partida.grid_size)) { 
                    spawn_ok = false; 
                    break; 
                } 
            } 
            if (!spawn_ok) break; 
        }
        
        if (spawn_ok) { 
            for (auto& comida_pos : partida.comida) {
                if (comida_pos == spawn_pos || (comida_pos.x == spawn_pos.x && comida_pos.y == (spawn_pos.y + 1) % partida.grid_size)) { 
                    spawn_ok = false; 
                    break; 
                } 
            } 
        } 
        attempts++;
    }
    
    if (!spawn_ok) { 
        cerr << "Advertencia: No se pudo encontrar spawn seguro." << endl; 
    }
    return spawn_pos;
}


int encontrarOcrearPartida(int playerID, int clientSocket, GameMode modoBuscado) {
    // 1. Buscar partida
    for (auto& pair : g_partidas) { 
        bool modo_ok = (pair.second.modo == modoBuscado);
        bool estado_ok = (pair.second.estado == LOBBY || pair.second.estado == COUNTDOWN);
        bool solo_ok = (modoBuscado == SOLO && pair.second.jugadores.size() < MAX_PLAYERS_SOLO);
        bool duo_ok = (modoBuscado == DUO && pair.second.jugadores.size() < MAX_PLAYERS_DUO);

        if (modo_ok && estado_ok && (solo_ok || duo_ok)) { 
            
            Posicion spawn_pos = generarSpawnSeguro(pair.second); 
            Jugador n; 
            n.socket = clientSocket; 
            n.cuerpo.push_back(spawn_pos); 
            n.cuerpo.push_back({spawn_pos.x, (spawn_pos.y + 1) % pair.second.grid_size}); 
            if(modoBuscado == DUO) {
                n.equipo_id = (pair.second.jugadores.size() / 2) + 1;
            } else {
                n.equipo_id = playerID; 
            }

            pair.second.jugadores[playerID] = n; 
            return pair.first; 
        } 
    }
    
    // 2. Crear partida nueva
    int nueva_partida_id = g_next_partida_id++; 
    g_partidas[nueva_partida_id].id = nueva_partida_id; 
    g_partidas[nueva_partida_id].estado = LOBBY;
    g_partidas[nueva_partida_id].modo = modoBuscado; 
    // --- Asignar grid_size ---
    g_partidas[nueva_partida_id].grid_size = (modoBuscado == SOLO) ? 50 : 100;

    Posicion spawn_pos = generarSpawnSeguro(g_partidas[nueva_partida_id]); 
    Jugador n; 
    n.socket = clientSocket; 
    n.cuerpo.push_back(spawn_pos); 
    n.cuerpo.push_back({spawn_pos.x, (spawn_pos.y + 1) % g_partidas[nueva_partida_id].grid_size}); 

    if(modoBuscado == DUO) {
        n.equipo_id = 1;
    } else {
        n.equipo_id = playerID;
    }

    g_partidas[nueva_partida_id].jugadores[playerID] = n;
    
    cout << "Creando nueva partida #" << nueva_partida_id << " (Modo: " << (modoBuscado == SOLO ? "SOLO" : "DUO") << ")" << endl; 
    return nueva_partida_id;
}