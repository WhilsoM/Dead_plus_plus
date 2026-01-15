#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <numbers>
#include <sstream>
#include <vector>

std::shared_ptr<sf::Texture> createPlayerTexture() {
  sf::RenderTexture rt;
  if (!rt.resize({32, 32})) {
    std::cerr << "Failed to create player texture" << std::endl;
  }
  rt.clear(sf::Color::Transparent);

  sf::CircleShape head(10);
  head.setFillColor(sf::Color(255, 220, 180));
  head.setPosition({6.f, 2.f});

  sf::RectangleShape body({16.f, 20.f});
  body.setFillColor(sf::Color(50, 50, 200));
  body.setPosition({8.f, 18.f});

  rt.draw(head);
  rt.draw(body);
  rt.display();

  auto texture = std::make_shared<sf::Texture>(rt.getTexture());
  texture->setSmooth(false);
  return texture;
}

std::shared_ptr<sf::Texture> createZombieTexture() {
  sf::RenderTexture rt;
  if (!rt.resize({32, 32})) {
    std::cerr << "Failed to create zombie texture" << std::endl;
  }
  rt.clear(sf::Color::Transparent);

  sf::CircleShape head(10);
  head.setFillColor(sf::Color(100, 150, 80));
  head.setPosition({6.f, 2.f});

  sf::RectangleShape body({16.f, 20.f});
  body.setFillColor(sf::Color(80, 100, 60));
  body.setPosition({8.f, 18.f});

  sf::RectangleShape arm1({4.f, 15.f});
  arm1.setFillColor(sf::Color(100, 150, 80));
  arm1.setPosition({4.f, 20.f});

  sf::RectangleShape arm2({4.f, 15.f});
  arm2.setFillColor(sf::Color(100, 150, 80));
  arm2.setPosition({24.f, 20.f});

  rt.draw(body);
  rt.draw(arm1);
  rt.draw(arm2);
  rt.draw(head);
  rt.display();

  auto texture = std::make_shared<sf::Texture>(rt.getTexture());
  texture->setSmooth(false);
  return texture;
}

std::shared_ptr<sf::Texture> createFloorTexture() {
  sf::RenderTexture rt;
  if (!rt.resize({64, 64})) {
    std::cerr << "Failed to create floor texture" << std::endl;
  }

  for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 64; x++) {
      int val = 40 + rand() % 20;
      sf::RectangleShape pixel({1.f, 1.f});
      pixel.setPosition({static_cast<float>(x), static_cast<float>(y)});
      pixel.setFillColor(sf::Color(val, val, val + 10));
      rt.draw(pixel);
    }
  }

  rt.display();
  auto texture = std::make_shared<sf::Texture>(rt.getTexture());
  texture->setRepeated(true);
  texture->setSmooth(false);
  return texture;
}

std::shared_ptr<sf::Texture> createWallTexture() {
  sf::RenderTexture rt;
  if (!rt.resize({64, 64})) {
    std::cerr << "Failed to create wall texture" << std::endl;
  }

  for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 64; x++) {
      int val = 80 + rand() % 30;
      sf::RectangleShape pixel({1.f, 1.f});
      pixel.setPosition({static_cast<float>(x), static_cast<float>(y)});
      pixel.setFillColor(sf::Color(val, val - 20, val - 20));
      rt.draw(pixel);
    }
  }

  rt.display();
  auto texture = std::make_shared<sf::Texture>(rt.getTexture());
  texture->setRepeated(true);
  texture->setSmooth(false);
  return texture;
}
