#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <vector>

class Entity : public sf::Drawable {
protected:
  sf::Texture texture;
  sf::Sprite sprite;

public:
  float hp;
  float damage;

  Entity(float health, float dmg)
      : texture(), sprite(texture), hp(health), damage(dmg) {}

  virtual void attack() {
    std::cout << "Entity attacks for " << damage << " damage!" << std::endl;
  }

  virtual void draw(sf::RenderTarget &target,
                    sf::RenderStates states) const override {
    target.draw(sprite, states);
  }

  void loadTexture(const std::string &path) {
    if (!texture.loadFromFile(path)) {
      std::cerr << "Error: Could not load texture from " << path << std::endl;
    }
  }

  sf::Vector2f getPosition() const { return sprite.getPosition(); }

  virtual ~Entity() {}

  void resolveCollision(Entity &other) {
    sf::FloatRect b1 = sprite.getGlobalBounds();
    sf::FloatRect b2 = other.sprite.getGlobalBounds();

    if (b1.findIntersection(b2)) { // Для SFML 3
      sf::Vector2f delta = sprite.getPosition() - other.sprite.getPosition();
      float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
      if (distance == 0)
        distance = 0.1f;
      sf::Vector2f normal = delta / distance;

      sprite.move(normal * 2.0f);
      other.sprite.move(-normal * 2.0f);
    }
  }

  sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

  void move(const sf::Vector2f &offset) { sprite.move(offset); }
};
