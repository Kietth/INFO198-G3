#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <SFML/Graphics.hpp>
#include <string>

// Carga de assets
bool cargarAssets();

// Funciones de dibujado
void drawGrid(sf::RenderWindow& window);
void drawGame(sf::RenderWindow& window);
void drawText(sf::RenderWindow& window, const std::string& str, float x, float y, int size, sf::Color color = sf::Color::White);
void drawTextCentered(sf::RenderWindow& window, const std::string& str, int y_offset, int size, sf::Color color = sf::Color::White);
void dibujarUI(sf::RenderWindow& window);
void dibujarEstado(sf::RenderWindow& window);

#endif // GRAFICOS_H