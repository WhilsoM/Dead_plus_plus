#pragma once
#include <SFML/Graphics.hpp>

class GameUI {
public:
  sf::Sprite healthBarBg;
  sf::Sprite healthBarFill;
  sf::Texture uiTexture;

  GameUI() : healthBarBg(uiTexture), healthBarFill(uiTexture) {
    // if (!bgTex.loadFromFile("path/to/ui_bg.png")) { ... }

    healthBarBg.setPosition({20, 20});
    healthBarFill.setPosition({20, 20});
    healthBarFill.setColor(sf::Color::Red);
  }

  void update(float currentHealth, float maxHealth) {
    float percentage = currentHealth / maxHealth;
    healthBarFill.setScale({percentage, 1.f});
  }

  void draw(sf::RenderWindow &window) {
    window.draw(healthBarBg);
    window.draw(healthBarFill);
  }
};
