#pragma once
#include "Entity.hpp"
#include <SFML/Graphics.hpp>

class Player : public Entity {
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

    Player(float startX, float startY, float health, float dmg) : Entity(health, dmg) {
        pos = {startX, startY};

        loadTexture("assets/sprites/player/player_w_hands_idle.png");
        texture.setSmooth(false); 

        sprite.setTextureRect(sf::IntRect({0, 0}, {13, 16}));

        sprite.setScale({6.f, 6.f});
        diameter = sprite.getGlobalBounds().size.x; 
   }

    void control() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) velocity.y -= acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) velocity.y += acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) velocity.x -= acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) velocity.x += acceleration;
    }

    void update(float windowX, float windowY) {
        animate();
        velocity *= friction;
        pos += velocity;

        if (pos.x > windowX - diameter) { pos.x = windowX - diameter; velocity.x = 0; }
        if (pos.x < 0) { pos.x = 0; velocity.x = 0; }
        if (pos.y > windowY - diameter) { pos.y = windowY - diameter; velocity.y = 0; }
        if (pos.y < 0) { pos.y = 0; velocity.y = 0; }
        
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


