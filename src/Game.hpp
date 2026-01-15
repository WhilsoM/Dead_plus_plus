#pragma once
#include "Textures.hpp"
#include "sprites/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <numbers>
#include <sstream>
#include <vector>

struct Entity {
  sf::Vector2f position;
  sf::Vector2f velocity;
  float speed;
  int health;
  sf::RectangleShape shape;
};

class Game {
private:
  sf::RenderWindow window;
  Entity player;
  std::vector<Entity> zombies;
  sf::Clock clock;
  sf::Clock spawnClock;
  sf::Clock damageClock;
  sf::Font font;
  std::shared_ptr<sf::Texture> playerTex, zombieTex, floorTex, wallTex;
  std::vector<sf::RectangleShape> walls;
  sf::View camera;
  bool gameOver;
  float survivalTime;

  void initTextures() {
    playerTex = createPlayerTexture();
    zombieTex = createZombieTexture();
    floorTex = createFloorTexture();
    wallTex = createWallTexture();
  }

  void initWalls() {
    // Внешние стены
    walls.push_back(createWall(0, 0, 2000, 20));
    walls.push_back(createWall(0, 0, 20, 1500));
    walls.push_back(createWall(0, 1480, 2000, 20));
    walls.push_back(createWall(1980, 0, 20, 1500));

    // Внутренние препятствия - комнаты
    walls.push_back(createWall(400, 300, 300, 20));
    walls.push_back(createWall(400, 300, 20, 300));

    walls.push_back(createWall(900, 200, 20, 400));
    walls.push_back(createWall(900, 200, 250, 20));

    walls.push_back(createWall(300, 800, 400, 20));
    walls.push_back(createWall(680, 800, 20, 300));

    walls.push_back(createWall(1200, 600, 300, 20));
    walls.push_back(createWall(1200, 620, 20, 400));

    walls.push_back(createWall(1400, 200, 20, 300));
    walls.push_back(createWall(1400, 200, 300, 20));
  }

  sf::RectangleShape createWall(float x, float y, float w, float h) {
    sf::RectangleShape wall({w, h});
    wall.setPosition({x, y});
    wall.setTexture(wallTex.get());
    wall.setTextureRect(
        sf::IntRect({0, 0}, {static_cast<int>(w), static_cast<int>(h)}));
    return wall;
  }

  void spawnZombie() {
    Entity zombie;

    float angle = (rand() % 360) * std::numbers::pi_v<float> / 180.0f;
    float distance = 600;
    zombie.position.x = player.position.x + std::cos(angle) * distance;
    zombie.position.y = player.position.y + std::sin(angle) * distance;

    zombie.speed = ZOMBIE_SPEED + (rand() % 40 - 20);
    zombie.health = 1;
    zombie.shape.setSize({32.f, 32.f});
    zombie.shape.setTexture(zombieTex.get());

    zombies.push_back(zombie);
  }

  bool checkCollision(const sf::Vector2f &pos, const sf::Vector2f &size) {
    sf::FloatRect entityRect(pos, size);

    for (const auto &wall : walls) {
      sf::FloatRect wallBounds = wall.getGlobalBounds();
      if (entityRect.findIntersection(wallBounds).has_value()) {
        return true;
      }
    }
    return false;
  }

  void updatePlayer(float dt) {
    sf::Vector2f newPos = player.position;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
      newPos.y -= player.speed * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
      newPos.y += player.speed * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
      newPos.x -= player.speed * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
      newPos.x += player.speed * dt;
    }

    if (!checkCollision(newPos, {32.f, 32.f})) {
      player.position = newPos;
    }

    player.shape.setPosition(player.position);
  }

  void updateZombies(float dt) {
    for (auto &zombie : zombies) {
      sf::Vector2f direction = player.position - zombie.position;
      float length =
          std::sqrt(direction.x * direction.x + direction.y * direction.y);

      if (length > 0) {
        direction /= length;

        sf::Vector2f newPos = zombie.position + direction * zombie.speed * dt;

        if (!checkCollision(newPos, {32.f, 32.f})) {
          zombie.position = newPos;
        }
      }

      zombie.shape.setPosition(zombie.position);

      // Проверка столкновения с игроком
      float dist =
          std::sqrt(std::pow(player.position.x - zombie.position.x, 2) +
                    std::pow(player.position.y - zombie.position.y, 2));

      if (dist < 40 && damageClock.getElapsedTime().asSeconds() > 0.5f) {
        player.health -= 5;
        damageClock.restart();

        if (player.health <= 0) {
          gameOver = true;
        }
      }
    }
  }

  void updateCamera() {
    camera.setCenter({player.position.x + 16.f, player.position.y + 16.f});

    // Ограничиваем камеру границами карты
    float camX = camera.getCenter().x;
    float camY = camera.getCenter().y;

    if (camX < WINDOW_WIDTH / 2.f)
      camX = WINDOW_WIDTH / 2.f;
    if (camX > 2000.f - WINDOW_WIDTH / 2.f)
      camX = 2000.f - WINDOW_WIDTH / 2.f;
    if (camY < WINDOW_HEIGHT / 2.f)
      camY = WINDOW_HEIGHT / 2.f;
    if (camY > 1500.f - WINDOW_HEIGHT / 2.f)
      camY = 1500.f - WINDOW_HEIGHT / 2.f;

    camera.setCenter({camX, camY});
  }

public:
  Game()
      : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
               "Zombie Horror Game") {
    srand(static_cast<unsigned int>(time(nullptr)));
    window.setFramerateLimit(60);

    // Инициализация игрока
    player.position = {1000.f, 750.f};
    player.speed = PLAYER_SPEED;
    player.health = MAX_HEALTH;
    player.shape.setSize({32.f, 32.f});

    // Инициализация камеры
    camera.setSize(
        {static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});
    camera.setCenter(player.position);

    gameOver = false;
    survivalTime = 0;

    initTextures();
    player.shape.setTexture(playerTex.get());
    initWalls();

    // Начальные зомби
    for (int i = 0; i < 5; i++) {
      spawnZombie();
    }
  }

  void run() {
    while (window.isOpen()) {
      while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          window.close();
        }

        if (gameOver && event->is<sf::Event::KeyPressed>()) {
          const auto &keyEvent = event->getIf<sf::Event::KeyPressed>();
          if (keyEvent && keyEvent->code == sf::Keyboard::Key::R) {
            player.position = {1000.f, 750.f};
            player.health = MAX_HEALTH;
            zombies.clear();
            for (int i = 0; i < 5; i++) {
              spawnZombie();
            }
            gameOver = false;
            survivalTime = 0;
            clock.restart();
            spawnClock.restart();
          }
        }
      }

      if (!gameOver) {
        float dt = clock.restart().asSeconds();
        survivalTime += dt;

        updatePlayer(dt);
        updateZombies(dt);
        updateCamera();

        // Спавн новых зомби
        if (spawnClock.getElapsedTime().asSeconds() > 3.0f) {
          spawnZombie();
          spawnClock.restart();
        }
      }

      render();
    }
  }

  void render() {
    window.clear(sf::Color(20, 20, 30));

    window.setView(camera);

    sf::RectangleShape floor({3000.f, 3000.f});
    floor.setPosition({-500.f, -500.f});
    floor.setTexture(floorTex.get());
    floor.setTextureRect(sf::IntRect({0, 0}, {3000, 3000}));
    window.draw(floor);

    // Рисуем стены
    for (const auto &wall : walls) {
      window.draw(wall);
    }

    // Рисуем игрока
    window.draw(player.shape);

    // Рисуем зомби
    for (const auto &zombie : zombies) {
      window.draw(zombie.shape);
    }

    // UI элементы
    window.setView(window.getDefaultView());

    // HP бар
    sf::RectangleShape hpBarBg({204.f, 34.f});
    hpBarBg.setPosition({20.f, 20.f});
    hpBarBg.setFillColor(sf::Color(50, 50, 50));
    hpBarBg.setOutlineColor(sf::Color::White);
    hpBarBg.setOutlineThickness(2.f);
    window.draw(hpBarBg);

    sf::RectangleShape hpBar({200.f * (player.health / 100.0f), 30.f});
    hpBar.setPosition({22.f, 22.f});

    if (player.health > 60) {
      hpBar.setFillColor(sf::Color::Green);
    } else if (player.health > 30) {
      hpBar.setFillColor(sf::Color::Yellow);
    } else {
      hpBar.setFillColor(sf::Color::Red);
    }
    window.draw(hpBar);

    // Текст HP
    sf::Text hpText(font);
    hpText.setString("HP: " + std::to_string(player.health));
    hpText.setCharacterSize(20);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition({25.f, 25.f});

    // Информация о зомби
    sf::Text zombieText(font);
    zombieText.setString("Zombies: " + std::to_string(zombies.size()));
    zombieText.setCharacterSize(20);
    zombieText.setFillColor(sf::Color::White);
    zombieText.setPosition({20.f, 60.f});
    window.draw(zombieText);

    // Время выживания
    sf::Text timeText(font);
    int minutes = static_cast<int>(survivalTime) / 60;
    int seconds = static_cast<int>(survivalTime) % 60;
    std::stringstream ss;
    ss << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
    timeText.setString(ss.str());
    timeText.setCharacterSize(20);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition({20.f, 95.f});
    window.draw(timeText);

    // Game Over экран
    if (gameOver) {
      sf::RectangleShape overlay({static_cast<float>(WINDOW_WIDTH),
                                  static_cast<float>(WINDOW_HEIGHT)});
      overlay.setFillColor(sf::Color(0, 0, 0, 200));
      window.draw(overlay);

      sf::Text gameOverText(font);
      gameOverText.setString("GAME OVER");
      gameOverText.setCharacterSize(80);
      gameOverText.setFillColor(sf::Color::Red);
      gameOverText.setPosition(
          {WINDOW_WIDTH / 2.f - 250.f, WINDOW_HEIGHT / 2.f - 100.f});
      window.draw(gameOverText);

      sf::Text restartText(font);
      restartText.setString("Press R to Restart");
      restartText.setCharacterSize(30);
      restartText.setFillColor(sf::Color::White);
      restartText.setPosition(
          {WINDOW_WIDTH / 2.f - 150.f, WINDOW_HEIGHT / 2.f + 20.f});
      window.draw(restartText);

      sf::Text finalTime(font);
      finalTime.setString(
          "Survived: " + std::to_string(static_cast<int>(survivalTime)) +
          " seconds");
      finalTime.setCharacterSize(25);
      finalTime.setFillColor(sf::Color::Yellow);
      finalTime.setPosition(
          {WINDOW_WIDTH / 2.f - 150.f, WINDOW_HEIGHT / 2.f + 70.f});
      window.draw(finalTime);
    }

    window.display();
  }
};
