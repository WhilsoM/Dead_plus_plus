#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

class Entity : public sf::Drawable {
protected:
    sf::Texture texture;
    sf::Sprite sprite;

public:
    float hp;
    float damage;

    Entity(float health, float dmg) 
        : texture(), sprite(texture), hp(health), damage(dmg) 
    {}

    virtual void attack() {
        std::cout << "Entity attacks for " << damage << " damage!" << std::endl;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(sprite, states);
    }

    void loadTexture(const std::string& path) {
        if (!texture.loadFromFile(path)) {
            std::cerr << "Error: Could not load texture from " << path << std::endl;
        } 
    }

    sf::Vector2f getPosition() const {
        return sprite.getPosition(); 
    }

    virtual ~Entity() {}
};
