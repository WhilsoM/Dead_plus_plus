#pragma once
#include "SFML/Audio.hpp"
#include <iostream>
#include <string>

class GameMusic {
private:
  sf::Music menuMusic;

public:
  GameMusic() {
    if (!menuMusic.openFromFile("assets/sounds/menu/sound_1_menu.wav")) {
      std::cerr << "Error: Could not load menu music" << std::endl;
    }

    menuMusic.setLooping(true);
    menuMusic.setVolume(40.f);
  }

  void play_sound() {
    if (menuMusic.getStatus() != sf::Music::Status::Playing) {
      menuMusic.play();
    }
  }

  void stop_music() { menuMusic.stop(); }
};
