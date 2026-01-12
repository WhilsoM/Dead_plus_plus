#include <SFML/Graphics.hpp>
#include "sprites/Player.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Horror Dev Stage 1");
    window.setFramerateLimit(60);

    // Создаем объект игрока
    Player player(400.f, 300.f);

    const auto onClose = [&window](const sf::Event::Closed&) { window.close(); };
    const auto onKeyPressed = [&window](const sf::Event::KeyPressed& key) {
        if (key.scancode == sf::Keyboard::Scancode::Escape) window.close();
    };

    while (window.isOpen()) {
        window.handleEvents(onClose, onKeyPressed);

        player.control();
        player.update();

        window.clear(sf::Color::Black);
        
        window.draw(player.shape);
        
        window.display();
    }
}

