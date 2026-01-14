#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

class Menu {
private:
    sf::Font font;
    std::vector<sf::Text> items;
    int selectedItemIndex = 0;

public:
    Menu(float width, float height) {
    std::string path = "assets/fonts/horror_font.otf"; 
        
    if (!font.openFromFile(path)) {
            std::cerr << "Error: Could not load texture from " << path << std::endl;
    }

        std::vector<std::string> names = {"START GAME", "OPTIONS", "EXIT"};
        
        for (size_t i = 0; i < names.size(); ++i) {
            sf::Text text(font, names[i], 50);
            text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
            float x = width / 2.f - text.getGlobalBounds().size.x / 2.f;
            float y = height / (names.size() + 1) * (i + 1);
            text.setPosition({x, y});
            items.push_back(std::move(text));
        }
    }

    void draw(sf::RenderWindow& window) {
        for (const auto& item : items) {
            window.draw(item);
        }
    }

    void moveUp() {
        items[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex = (selectedItemIndex - 1 + items.size()) % items.size();
        items[selectedItemIndex].setFillColor(sf::Color::Red);
    }

    void moveDown() {
        items[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex = (selectedItemIndex + 1) % items.size();
        items[selectedItemIndex].setFillColor(sf::Color::Red);
    }

    int getSelectedIndex() const { return selectedItemIndex; }
};
