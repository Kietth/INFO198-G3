#include "red_servidor.h"
#include "logica.h" // Para encontrarOcrearPartida
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <cctype>
#include <cstring> // para strerror

using namespace std;

string serializarEstado(Partida& partida, int mi_player_id) {
    string serialized_data = "";
    if (partida.estado == LOBBY) {
        string modo_str = (partida.modo == SOLO ? "Solo" : "Duo");
        string max_p = (partida.modo == SOLO) ? to_string(MAX_PLAYERS_SOLO) : to_string(MAX_PLAYERS_DUO);
        serialized_data = "LOBBY,Esperando jugadores... ["+modo_str+"] (" + to_string(partida.jugadores.size()) + "/" + max_p + ")";
    } else if (partida.estado == COUNTDOWN) {
        auto restante = chrono::duration_cast<chrono::seconds>(partida.tiempoInicioCountdown + chrono::seconds(COUNTDOWN_SECONDS) - chrono::steady_clock::now()).count();
        serialized_data = "COUNTDOWN," + to_string(restante > 0 ? restante : 0);
    } else if (partida.estado == ENDED) {
        int winner_id = -1;
        int winning_team = -1;
        int vivos = 0;
        
        for(auto& pair : partida.jugadores) { 
            if (pair.second.estaVivo) { 
                vivos++; 
                winner_id = pair.first; 
                winning_team = pair.second.equipo_id;
            } 
        }
        
        string final_msg;
        if (partida.modo == SOLO) {
             if (vivos == 1 && winner_id == mi_player_id) { final_msg = "VICTORIA"; }
             else if (vivos == 1 && winner_id != mi_player_id) { final_msg = "Derrota."; }
             else { final_msg = "Empate"; }
        } else { // MODO DUO
            int mi_equipo = -1;
            if (partida.jugadores.count(mi_player_id)) {
                mi_equipo = partida.jugadores.at(mi_player_id).equipo_id;
            }
            if (vivos > 0 && winning_team == mi_equipo) { final_msg = "VICTORIA DE EQUIPO"; }
            else if (vivos > 0 && winning_team != mi_equipo) { final_msg = "Derrota."; } 
            else { final_msg = "Empate"; }
        }
        serialized_data = "VICTORY," + final_msg;

    } else if (partida.estado == PLAYING) {
        serialized_data = "PLAYING,GS," + to_string(partida.grid_size) + ",";

        for (const auto& pair : partida.jugadores) {
            int id = pair.first; const Jugador& jugador = pair.second;
            serialized_data += "J" + to_string(id) + "," + (jugador.estaVivo ? "1" : "0") + ",T" + to_string(jugador.equipo_id);
            
            for (const auto& pos : jugador.cuerpo) { serialized_data += "," + to_string(pos.x) + "," + to_string(pos.y); }
            serialized_data += "|";
        }
        serialized_data += "F";
        for (const auto& pos : partida.comida) { serialized_data += "," + to_string(pos.x) + "," + to_string(pos.y); }
    }
    if (mi_player_id != -1 && partida.jugadores.count(mi_player_id) && !partida.jugadores[mi_player_id].estaVivo) { serialized_data += ";GAMEOVER"; }
    serialized_data += ";";
    return serialized_data;
}


void manejarCliente(int clientSocket, int playerID) {
    cout << "Cliente #" << playerID << " conectado. Esperando modo..." << endl;

    char mode_buffer[10];
    int bytesReadMode = recv(clientSocket, mode_buffer, sizeof(mode_buffer) - 1, 0);
    if (bytesReadMode <= 0) {
        cerr << "Cliente #" << playerID << " desconectado antes de elegir modo." << endl;
        close(clientSocket);
        return;
    }
    mode_buffer[bytesReadMode] = '\0';
    string mode_str(mode_buffer);
    GameMode modoBuscado = (mode_str == "DUO") ? DUO : SOLO;
    cout << "Cliente #" << playerID << " eligió modo: " << mode_str << endl;


    string welcome_msg = "WELCOME," + to_string(playerID) + ";";
    if (send(clientSocket, welcome_msg.c_str(), welcome_msg.length(), 0) <= 0) { return; }

    int partida_id = -1; bool buscando_partida = true;
    while (buscando_partida) {
        { 
            lock_guard<mutex> lock(g_mutex); 
            partida_id = encontrarOcrearPartida(playerID, clientSocket, modoBuscado); 
            g_jugador_a_partida_id[playerID] = partida_id; 
            buscando_partida = false; 
        }
        char buffer[1024];
        while (true) {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                buscando_partida = false;
                { lock_guard<mutex> lock(g_mutex);
                    cout << "Cliente #" << playerID << " desconectado." << endl;
                    if (g_partidas.count(partida_id) && g_partidas[partida_id].jugadores.count(playerID)) {
                        Partida& partida_actual = g_partidas[partida_id];
                        if (partida_actual.estado == PLAYING) {
                            partida_actual.jugadores[playerID].estaVivo = false;
                            
                            int vivos = 0;
                            map<int, bool> equipos_vivos;
                            for (auto& p : partida_actual.jugadores) { 
                                if (p.second.estaVivo) {
                                    vivos++;
                                    if(partida_actual.modo == DUO) equipos_vivos[p.second.equipo_id] = true;
                                }
                            }
                            
                            bool min_players_solo = partida_actual.jugadores.size() >= 2;
                            bool min_players_duo = partida_actual.jugadores.size() >= 4;
                            bool solo_end = (partida_actual.modo == SOLO && vivos <= 1 && min_players_solo);
                            bool duo_end = (partida_actual.modo == DUO && equipos_vivos.size() <= 1 && min_players_duo);

                            if (solo_end || duo_end) { 
                                partida_actual.estado = ENDED;
                            }

                        } else if (partida_actual.estado == LOBBY || partida_actual.estado == COUNTDOWN) {
                             partida_actual.jugadores.erase(playerID);
                        }
                    }
                } break; 
            }
            buffer[bytesRead] = '\0'; string input(buffer);
            input.erase(remove_if(input.begin(), input.end(), [](unsigned char c){ return isspace(c); }), input.end());

            { lock_guard<mutex> lock(g_mutex); if (!g_partidas.count(partida_id) || !g_partidas[partida_id].jugadores.count(playerID)) break; Partida& mi_partida = g_partidas[partida_id]; Jugador& mi_jugador = mi_partida.jugadores[playerID];
                if (input == "BOOST_ON") { if (mi_jugador.estaVivo) mi_jugador.boosting = true; }
                else if (input == "BOOST_OFF") { mi_jugador.boosting = false; }
                else if ((!mi_jugador.estaVivo || mi_partida.estado == ENDED) && (input == "ENTER" || input == "RESTART")) { 
                    mi_partida.jugadores.erase(playerID); 
                    buscando_partida = true; 
                    break; 
                }
                else if (mi_jugador.estaVivo && (input == "UP" || input == "DOWN" || input == "LEFT" || input == "RIGHT")) { if ( (mi_jugador.direccion != "DOWN" && input == "UP") || (mi_jugador.direccion != "UP" && input == "DOWN") || (mi_jugador.direccion != "RIGHT" && input == "LEFT") || (mi_jugador.direccion != "LEFT" && input == "RIGHT") ) { mi_jugador.direccion_siguiente = input; } }
            }
        }
    }
    
    { lock_guard<mutex> lock(g_mutex);
        if (g_jugador_a_partida_id.count(playerID)) {
            int p_id = g_jugador_a_partida_id[playerID];
            if (g_partidas.count(p_id)) {
                g_partidas[p_id].jugadores.erase(playerID); 
                if (g_partidas[p_id].jugadores.empty()) { 
                    g_partidas.erase(p_id); cout << "Partida #" << p_id << " eliminada (limpieza final)." << endl;
                }
            }
            g_jugador_a_partida_id.erase(playerID);
        }
    }
    close(clientSocket);
    cout << "Cliente #" << playerID << " hilo terminado." << endl;
}