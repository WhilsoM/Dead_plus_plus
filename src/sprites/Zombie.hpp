#pragma once
#include "Entity.hpp"
#include <SFML/Graphics.hpp>

class Zombie : public Entity {
private:
  sf::Vector2f velocity = {0.f, 0.f};
  float acceleration = 0.5f;
  float friction = 0.95f;
  float diameter;
  sf::Clock animationClock;
  int currentFrame = 0;
  int totalFrames = 6;
  float frameTime = 0.15f;

public:
  sf::Vector2f pos;

  Zombie(float startX, float startY, float health, float dmg)
      : Entity(health, dmg) {
    pos = {startX, startY};

    loadTexture("assets/sprites/enemies/zombie_small/zombie_down_idle.png");
    texture.setSmooth(false);

    sprite.setTextureRect(sf::IntRect({0, 0}, {13, 16}));

    sprite.setScale({6.f, 6.f});
    diameter = sprite.getGlobalBounds().size.x;
  }

  void update(sf::Vector2f player_position) {
    sf::Vector2f direction = player_position - pos;
    animate();
    float distance =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 1.0f) {
      float speed = 1.5f;
      velocity = (direction / distance) * speed;
    } else {
      attack();
      velocity = {0.f, 0.f};
    }

    pos += velocity;
    sprite.setPosition(pos);
  }

  void animate() {
    if (animationClock.getElapsedTime().asSeconds() >= frameTime) {
      currentFrame = (currentFrame + 1) % totalFrames;

      sprite.setTextureRect(sf::IntRect({currentFrame * 13, 0}, {13, 16}));

      animationClock.restart();
    }
  }
};
