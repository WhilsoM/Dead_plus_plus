#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class Player {
private:
    sf::Vector2f velocity = {0.f, 0.f};
    float acceleration = 0.5f;
    float friction = 0.95f;
    
    float windowX = 800.0f;
    float windowY = 600.0f;
    float diameter;

public:
    sf::CircleShape shape;
    sf::Vector2f pos;

    Player(float startX, float startY) {
        pos = {startX, startY};
        shape.setRadius(50.f);
        shape.setFillColor(sf::Color::Green);
        
        // Считаем диаметр ПОСЛЕ того, как задали радиус
        diameter = shape.getRadius() * 2.f;
    }

    void control() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) velocity.y -= acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) velocity.y += acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) velocity.x -= acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) velocity.x += acceleration;
    }

    void update() {
        velocity *= friction;
        pos += velocity;

        if (pos.x > windowX - diameter) { pos.x = windowX - diameter; velocity.x = 0; }
        if (pos.x < 0) { pos.x = 0; velocity.x = 0; }
        if (pos.y > windowY - diameter) { pos.y = windowY - diameter; velocity.y = 0; }
        if (pos.y < 0) { pos.y = 0; velocity.y = 0; }
        
        shape.setPosition(pos);
    }
};

#endif
