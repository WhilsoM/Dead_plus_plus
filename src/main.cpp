#include "config/GameState.hpp"
#include "config/GameUi.hpp"
#include "screens/Menu.hpp"
#include "screens/Options.hpp"
#include "sounds/GameMusic.hpp"
#include "sprites/Player.hpp"
#include "sprites/Zombie.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

int main() {
  float windowX = 800.0f;
  float windowY = 600.0f;

  sf::RenderWindow window(sf::VideoMode({800, 600}), "Death++");
  sf::View view(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));

  GameState currentState = GameState::Menu;
  GameMusic menuMusic;
  GameUI ui;

  Options options(windowX, windowY);
  Menu menu(windowX, windowY);

  window.setFramerateLimit(60);

  Player player(400.f, 300.f, 100.f, 10.f);

  std::vector<Zombie> enemies;
  enemies.push_back(Zombie(50.f, 50.f, 15.f, 5.f));
  enemies.push_back(Zombie(100.f, 100.f, 15.f, 5.f));

  const auto onClose = [&window](const sf::Event::Closed &) { window.close(); };
  const auto onKeyPressed = [&window](const sf::Event::KeyPressed &key) {
    if (key.scancode == sf::Keyboard::Scancode::Escape)
      window.close();
  };
  const auto onResized = [&view](const sf::Event::Resized &event) {
    float windowRatio =
        static_cast<float>(event.size.x) / static_cast<float>(event.size.y);
    float viewRatio = 800.f / 600.f;

    view.setSize({800.f, 600.f});
  };

  sf::Clock damageClock;

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (currentState == GameState::Menu) {
          if (keyPressed->scancode == sf::Keyboard::Scancode::W)
            menu.moveUp();
          if (keyPressed->scancode == sf::Keyboard::Scancode::S)
            menu.moveDown();

          if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {
            if (menu.getSelectedIndex() == 0) {
              currentState = GameState::Playing;
            } else if (menu.getSelectedIndex() == 1) {
              currentState = GameState::Options;
            } else if (menu.getSelectedIndex() == 2) {
              window.close();
            }
          }
        }

        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
          window.close();

        else if (currentState == GameState::Options) {
          if (keyPressed->scancode == sf::Keyboard::Scancode::W ||
              keyPressed->scancode == sf::Keyboard::Scancode::Up)
            options.moveUp();
          if (keyPressed->scancode == sf::Keyboard::Scancode::S ||
              keyPressed->scancode == sf::Keyboard::Scancode::Down)
            options.moveDown();

          if (keyPressed->scancode == sf::Keyboard::Scancode::A ||
              keyPressed->scancode == sf::Keyboard::Scancode::Left) {
            options.changeValue(-1);
          }
          if (keyPressed->scancode == sf::Keyboard::Scancode::D ||
              keyPressed->scancode == sf::Keyboard::Scancode::Right) {
            options.changeValue(1);
          }

          if (keyPressed->scancode == sf::Keyboard::Scancode::Enter &&
              options.getSelectedIndex() == 2) {
            currentState = GameState::Menu;
          }
          if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
            currentState = GameState::Menu;
          }
        }
      }
    }

    window.clear(sf::Color::Black);

    if (currentState == GameState::Menu) {
      menuMusic.play_sound();
      menu.draw(window);
    } else if (currentState == GameState::Options) {
      options.draw(window);
    } else if (currentState == GameState::Playing) {
      menuMusic.stop_music();

      player.control();
      player.update(windowX, windowY);

      for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].update(player.getPosition());

        // ПРЕДОТВРАЩЕНИЕ НАКЛАДЫВАНИЯ
        for (size_t j = i + 1; j < enemies.size(); ++j) {
          enemies[i].resolveCollision(enemies[j]);
        }

        // СНЯТИЕ ХП ПРИ КОНТАКТЕ (SFML 3 использует findIntersection)
        if (player.getGlobalBounds().findIntersection(
                enemies[i].getGlobalBounds())) {
          if (damageClock.getElapsedTime().asSeconds() > 1.2f) {
            player.takeDamage(15.f);
            damageClock.restart();
            std::cout << "Player HP: " << player.getHealth() << std::endl;
          }
        }
      }

      view.setCenter(player.getPosition());
      window.setView(view);

      window.draw(player);
      for (auto &zombie : enemies) {
        window.draw(zombie);
      }

      window.setView(window.getDefaultView());
      ui.update(player.getHealth(), 100.f);
      ui.draw(window);
    }
    window.display();
  }
  return 0;
}
