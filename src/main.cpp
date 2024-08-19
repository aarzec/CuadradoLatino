#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "../include/sfmlbutton.hpp"

template <typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

bool isSafe(int row, int col, int value, const std::vector<std::vector<int>>& grid, int n) {
    for (int i = 0; i < n; ++i) {
        if (grid[row][i] == value || grid[i][col] == value) {
            return false;
        }
    }
    return true;
}

void solveLatinSquare(int n, int row, int col, std::vector<std::vector<int>>& grid, std::vector<std::vector<std::vector<int>>>& solutions) {
    if (row == n) {
        solutions.push_back(grid);  // Store the current valid solution
        return;
    }

    if (col == n) {
        solveLatinSquare(n, row + 1, 0, grid, solutions);  // Move to the next row
        return;
    }

    for (int value = 1; value <= n; ++value) {
        if (isSafe(row, col, value, grid, n)) {
            grid[row][col] = value;

            // Recur to fill the next cell
            solveLatinSquare(n, row, col + 1, grid, solutions);

            // Backtrack
            grid[row][col] = 0;
        }
    }
}

void findLatinSquare(int n, int row, std::vector<std::vector<int>>& grid, std::vector<std::vector<std::vector<int>>>& solutions, std::vector<bool>& usedInRow, std::vector<std::vector<bool>>& usedInCol) {
    if (row == n) {
        // All rows are filled, store the solution
        solutions.push_back(grid);
        return;
    }

    // Try placing each number from 1 to n in this row
    for (int col = 0; col < n; ++col) {
        for (int value = 1; value <= n; ++value) {
            if (!usedInRow[row * n + value - 1] && !usedInCol[col][value - 1]) {
                // Place the value
                grid[row][col] = value;
                usedInRow[row * n + value - 1] = true;
                usedInCol[col][value - 1] = true;

                // Recur to the next row
                findLatinSquare(n, row + 1, grid, solutions, usedInRow, usedInCol);

                // Backtrack
                usedInRow[row * n + value - 1] = false;
                usedInCol[col][value - 1] = false;
                grid[row][col] = 0;
            }
        }
    }
}

std::vector<std::vector<std::vector<int>>> generateAllLatinSquares(int n) {
    std::vector<std::vector<int>> grid(n, std::vector<int>(n, 0));  // Initialize empty grid
    std::vector<std::vector<std::vector<int>>> solutions;  // To store all solutions
    solveLatinSquare(n, 0, 0, grid, solutions);
    return solutions;
}

sf::Color inverseLegibleColor(sf::Color color) {
    int r = color.r;
    int g = color.g;
    int b = color.b;
    if (r * 0.299 + g * 0.7 + b * 0.114 > 150) {
        return sf::Color::Black;
    } else {
        return sf::Color::White;
    }
}

void drawLatinSquare(sf::RenderWindow& window, const std::vector<std::vector<int>>& square, int tileSize, sf::Font& font, const std::vector<sf::Color>& colors) {
    int n = square.size();
    
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
            tile.setPosition(50 + col * tileSize, 100 + row * tileSize);
            tile.setFillColor(colors[square[row][col] - 1]);

            window.draw(tile);

            // Dibujar números
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(square[row][col]));
            text.setCharacterSize(tileSize / 2);
            text.setFillColor(inverseLegibleColor(colors[square[row][col] - 1]));

            // Centrar el texto
            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(
                textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f
            );
            text.setPosition(
                50 + col * tileSize + tileSize / 2.0f,
                100 + row * tileSize + tileSize / 2.0f
            );
            window.draw(text);
        }
    }
}

void actualizarIndiceSolucion(sf::Text &labelSolucionActual, size_t solutionIndex, std::vector<std::vector<std::vector<int>>> &solutions, int n) {
    labelSolucionActual.setString("Solucion " + std::to_string(solutionIndex + 1) + "/" + std::to_string(solutions.size()));
}

std::tuple<int, int, int> HSLToRGB(float h, float s, float l) {
    float r, g, b;
    
    auto hue2rgb = [](float p, float q, float t) -> float {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1.0 / 6.0) return p + (q - p) * 6 * t;
        if (t < 1.0 / 2.0) return q;
        if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6;
        return p;
    };

    if (s == 0) {
        r = g = b = l; // achromatic (gray)
    } else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = hue2rgb(p, q, h + 1.0 / 3.0);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0 / 3.0);
    }

    return std::make_tuple(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
}

std::vector<sf::Color> generateRainbowColors(int n) {
    std::vector<sf::Color> colors;

    for (int i = 0; i < n; ++i) {
        float hue = static_cast<float>(i) / n; // Normalized hue (0 to 1)
        float saturation = 1.0f;              // Full saturation for vibrant colors
        float lightness = 0.5f;               // Mid-range lightness for balance
        auto rgb = HSLToRGB(hue, saturation, lightness);
        colors.push_back(sf::Color(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb)));
    }

    return colors;
}

int main() {
    int n;

    std::cout << "Ingrese n (n filas x n columnas): ";
    std::cin >> n;

    const unsigned int FPS = 10;
    std::vector<std::vector<std::vector<int>>> solutions = generateAllLatinSquares(n);

    if (solutions.empty()) {
        std::cout << "No hay soluciones para cuadrados latinos de " << n << "x" << n << std::endl;
        return 0;
    }

    int tileSize = 40;
    const int windowWidth = clamp(100 + n * tileSize, 400, 800);
    const int windowHeight = clamp(200 + n * tileSize, 450, 800);
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Cuadrado latino");
    window.setFramerateLimit(FPS);

    // Cargar fuentes
    sf::Font ft_arial;
    ft_arial.loadFromFile("fonts/Arial.ttf");

    // Controles
    sf::Text labelTitulo("n: " + std::to_string(n), ft_arial, 20);
    labelTitulo.setFillColor(sf::Color::White);
    labelTitulo.setPosition(50, 50);

    sf::Text labelSolucionActual("Solucion 0/0 ", ft_arial, 20);
    labelSolucionActual.setFillColor(sf::Color::White);
    labelSolucionActual.setPosition(50.f, windowHeight - 50);
    RectButton buttonPrevious(ft_arial, sf::Vector2f(40.f, 25.f), sf::Vector2f(250.f, windowHeight - 50));
    buttonPrevious.setButtonLabel(20.f, " < ");
    RectButton buttonNext(ft_arial, sf::Vector2f(40.f, 25.f), sf::Vector2f(300.f, windowHeight - 50));
    buttonNext.setButtonLabel(20.f, " > ");

    size_t solutionIndex = 0;
    std::vector<sf::Color> colors = generateRainbowColors(n);

    sf::Event event;
    actualizarIndiceSolucion(labelSolucionActual, solutionIndex, solutions, n);
    while (window.isOpen()) {
        // Step
        while (window.pollEvent(event)) {
            buttonNext.getButtonStatus(window, event);
            buttonPrevious.getButtonStatus(window, event);
            if (buttonNext.isPressed) {
                solutionIndex = (solutionIndex + 1) % solutions.size();
                actualizarIndiceSolucion(labelSolucionActual, solutionIndex, solutions, n);
            }
            if (buttonPrevious.isPressed) {
                solutionIndex = (solutionIndex - 1 + solutions.size()) % solutions.size();
                actualizarIndiceSolucion(labelSolucionActual, solutionIndex, solutions, n);
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Dibujar
        window.clear();
        // - Textos
        window.draw(labelTitulo);
        window.draw(labelSolucionActual);
        // - Botones
        buttonPrevious.draw(window);
        buttonNext.draw(window);
        // - Cuadro
        drawLatinSquare(window, solutions[solutionIndex], tileSize, ft_arial, colors);

        window.display();
    }

    return 0;
}
