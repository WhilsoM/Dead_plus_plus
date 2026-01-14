#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

class Options {
private:
  sf::Font font;
  sf::Text title;
  sf::Text footer;

  std::vector<sf::Text> options;
  std::vector<std::string> fpsValues = {"60", "120"};
  std::vector<std::string> musicTracks = {"Menu Wav", "Guitar MP3", "Sound 2"};

  int selectedIndex = 0;
  int currentFpsIdx = 0;
  int currentMusicIdx = 0;

public:
  Options(float width, float height) : title(font), footer(font) {
    if (!font.openFromFile("assets/fonts/horror_font.otf")) {
      std::cerr
          << "Error: Could not load font from assets/fonts/horror_font.otf"
          << std::endl;
    }

    title.setString("SETTINGS");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::Red);
    title.setPosition(
        {width / 2.f - title.getGlobalBounds().size.x / 2.f, 50.f});

    for (int i = 0; i < 3; ++i) {
      options.emplace_back(font);
      options[i].setCharacterSize(35);
    }

    updateText();

    footer.setString("INFO: telegram channel - @moonwqwizliochannel");
    footer.setCharacterSize(20);
    footer.setFillColor(sf::Color(150, 150, 150));
    footer.setPosition(
        {width / 2.f - footer.getGlobalBounds().size.x / 2.f, height - 50.f});
  }

  void updateText() {
    options[0].setString("FPS: < " + fpsValues[currentFpsIdx] + " >");
    options[1].setString("MUSIC: < " + musicTracks[currentMusicIdx] + " >");
    options[2].setString("BACK TO MENU");

    for (int i = 0; i < 3; ++i) {
      options[i].setPosition({400.f - options[i].getGlobalBounds().size.x / 2.f,
                              200.f + i * 100.f});
      if (i == selectedIndex) {
        options[i].setFillColor(sf::Color::Red);
      } else {
        options[i].setFillColor(sf::Color::White);
      }
    }
  }

  void moveUp() {
    if (selectedIndex > 0)
      selectedIndex--;
    updateText();
  }

  void moveDown() {
    if (selectedIndex < (int)options.size() - 1)
      selectedIndex++;
    updateText();
  }

  void changeValue(int delta) {
    if (selectedIndex == 0) {
      currentFpsIdx = (currentFpsIdx + delta + (int)fpsValues.size()) %
                      (int)fpsValues.size();
    } else if (selectedIndex == 1) {
      currentMusicIdx = (currentMusicIdx + delta + (int)musicTracks.size()) %
                        (int)musicTracks.size();
    }
    updateText();
  }

  int getSelectedIndex() const { return selectedIndex; }
  int getFpsValue() const { return std::stoi(fpsValues[currentFpsIdx]); }
  int getMusicTrackIdx() const { return currentMusicIdx; }

  void draw(sf::RenderWindow &window) {
    window.draw(title);
    for (const auto &opt : options)
      window.draw(opt);
    window.draw(footer);
  }
};
