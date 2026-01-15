#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>
#include <numbers>
#include <memory>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const float PLAYER_SPEED = 200.0f;
const float ZOMBIE_SPEED = 120.0f;
const float FAST_ZOMBIE_SPEED = 180.0f;
const int MAX_HEALTH = 100;
const float ZOMBIE_SPAWN_INTERVAL = 2.0f;  // Каждые 2 секунды
const int INITIAL_ZOMBIES = 10;  // Начальное количество зомби

enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

enum class ZombieType {
    NORMAL,
    FAST
};

enum class PowerUpType {
    HEALTH,
    SPEED,
    GRENADE
};

struct Entity {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed;
    int health;
    sf::RectangleShape shape;
    ZombieType type;
};

struct PowerUp {
    sf::Vector2f position;
    PowerUpType type;
    sf::CircleShape shape;
    float lifetime;
};

// Генерация простых звуковых эффектов
sf::SoundBuffer createFootstepSound() {
    sf::SoundBuffer buffer;
    std::vector<int16_t> samples;
    const unsigned sampleRate = 44100;
    const float duration = 0.1f;
    
    for(int i = 0; i < sampleRate * duration; i++) {
        float t = static_cast<float>(i) / sampleRate;
        samples.push_back(static_cast<int16_t>(5000 * std::sin(2 * std::numbers::pi_v<float> * 200 * t) * (1 - t / duration)));
    }
    
    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono})) {
        std::cerr << "Failed to load footstep sound" << std::endl;
    }
    return buffer;
}

sf::SoundBuffer createHitSound() {
    sf::SoundBuffer buffer;
    std::vector<int16_t> samples;
    const unsigned sampleRate = 44100;
    const float duration = 0.15f;
    
    for(int i = 0; i < sampleRate * duration; i++) {
        float t = static_cast<float>(i) / sampleRate;
        samples.push_back(static_cast<int16_t>(15000 * std::sin(2 * std::numbers::pi_v<float> * 150 * t) * (1 - t / duration)));
    }
    
    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono})) {
        std::cerr << "Failed to load hit sound" << std::endl;
    }
    return buffer;
}

sf::SoundBuffer createPickupSound() {
    sf::SoundBuffer buffer;
    std::vector<int16_t> samples;
    const unsigned sampleRate = 44100;
    const float duration = 0.2f;
    
    for(int i = 0; i < sampleRate * duration; i++) {
        float t = static_cast<float>(i) / sampleRate;
        samples.push_back(static_cast<int16_t>(10000 * std::sin(2 * std::numbers::pi_v<float> * (400 + 200 * t) * t)));
    }
    
    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono})) {
        std::cerr << "Failed to load pickup sound" << std::endl;
    }
    return buffer;
}

sf::SoundBuffer createExplosionSound() {
    sf::SoundBuffer buffer;
    std::vector<int16_t> samples;
    const unsigned sampleRate = 44100;
    const float duration = 0.5f;
    
    for(int i = 0; i < sampleRate * duration; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float noise = (rand() % 20000 - 10000) * (1 - t / duration);
        samples.push_back(static_cast<int16_t>(noise));
    }
    
    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono})) {
        std::cerr << "Failed to load explosion sound" << std::endl;
    }
    return buffer;
}

// Генерация текстур процедурно
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

std::shared_ptr<sf::Texture> createZombieTexture(ZombieType type) {
    sf::RenderTexture rt;
    if (!rt.resize({32, 32})) {
        std::cerr << "Failed to create zombie texture" << std::endl;
    }
    rt.clear(sf::Color::Transparent);
    
    sf::Color zombieColor = (type == ZombieType::FAST) ? 
        sf::Color(180, 50, 50) : sf::Color(100, 150, 80);
    
    sf::CircleShape head(10);
    head.setFillColor(zombieColor);
    head.setPosition({6.f, 2.f});
    
    sf::RectangleShape body({16.f, 20.f});
    body.setFillColor(zombieColor);
    body.setPosition({8.f, 18.f});
    
    sf::RectangleShape arm1({4.f, 15.f});
    arm1.setFillColor(zombieColor);
    arm1.setPosition({4.f, 20.f});
    
    sf::RectangleShape arm2({4.f, 15.f});
    arm2.setFillColor(zombieColor);
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
    
    for(int y = 0; y < 64; y++) {
        for(int x = 0; x < 64; x++) {
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
    
    for(int y = 0; y < 64; y++) {
        for(int x = 0; x < 64; x++) {
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

class Game {
private:
    sf::RenderWindow window;
    Entity player;
    std::vector<Entity> zombies;
    std::vector<PowerUp> powerUps;
    sf::Clock clock;
    sf::Clock spawnClock;
    sf::Clock damageClock;
    sf::Clock powerUpSpawnClock;
    sf::Clock speedBoostClock;
    sf::Font font;
    std::shared_ptr<sf::Texture> playerTex, normalZombieTex, fastZombieTex, floorTex, wallTex;
    std::vector<sf::RectangleShape> walls;
    sf::View camera;
    GameState gameState;
    float survivalTime;
    bool hasSpeedBoost;
    
    // Звуки
    sf::SoundBuffer footstepBuffer, hitBuffer, pickupBuffer, explosionBuffer;
    std::unique_ptr<sf::Sound> footstepSound, hitSound, pickupSound, explosionSound;
    sf::Clock footstepClock;
    
    void initTextures() {
        playerTex = createPlayerTexture();
        normalZombieTex = createZombieTexture(ZombieType::NORMAL);
        fastZombieTex = createZombieTexture(ZombieType::FAST);
        floorTex = createFloorTexture();
        wallTex = createWallTexture();
    }
    
    void initSounds() {
        footstepBuffer = createFootstepSound();
        hitBuffer = createHitSound();
        pickupBuffer = createPickupSound();
        explosionBuffer = createExplosionSound();
        
        footstepSound = std::make_unique<sf::Sound>(footstepBuffer);
        hitSound = std::make_unique<sf::Sound>(hitBuffer);
        pickupSound = std::make_unique<sf::Sound>(pickupBuffer);
        explosionSound = std::make_unique<sf::Sound>(explosionBuffer);
        
        footstepSound->setVolume(30);
        hitSound->setVolume(50);
        pickupSound->setVolume(40);
        explosionSound->setVolume(70);
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
        wall.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(w), static_cast<int>(h)}));
        return wall;
    }
    
    void spawnZombie() {
        Entity zombie;
        
        // 40% шанс быстрого зомби (было 30%)
        zombie.type = (rand() % 100 < 40) ? ZombieType::FAST : ZombieType::NORMAL;
        
        // Спавним зомби вне экрана
        float angle = (rand() % 360) * std::numbers::pi_v<float> / 180.0f;
        float distance = 600;
        zombie.position.x = player.position.x + std::cos(angle) * distance;
        zombie.position.y = player.position.y + std::sin(angle) * distance;
        
        if(zombie.type == ZombieType::FAST) {
            zombie.speed = FAST_ZOMBIE_SPEED + (rand() % 30 - 15);
            zombie.health = 1;
            zombie.shape.setTexture(fastZombieTex.get());
        } else {
            zombie.speed = ZOMBIE_SPEED + (rand() % 40 - 20);
            zombie.health = 1;
            zombie.shape.setTexture(normalZombieTex.get());
        }
        
        zombie.shape.setSize({32.f, 32.f});
        zombies.push_back(zombie);
    }
    
    void spawnPowerUp() {
        PowerUp powerUp;
        
        // Случайная позиция на карте
        powerUp.position.x = 100 + rand() % 1800;
        powerUp.position.y = 100 + rand() % 1300;
        
        // Случайный тип
        int r = rand() % 100;
        if(r < 40) {
            powerUp.type = PowerUpType::HEALTH;
            powerUp.shape.setFillColor(sf::Color::Green);
        } else if(r < 70) {
            powerUp.type = PowerUpType::SPEED;
            powerUp.shape.setFillColor(sf::Color::Cyan);
        } else {
            powerUp.type = PowerUpType::GRENADE;
            powerUp.shape.setFillColor(sf::Color::Red);
        }
        
        powerUp.shape.setRadius(15);
        powerUp.shape.setPosition(powerUp.position);
        powerUp.shape.setOutlineColor(sf::Color::White);
        powerUp.shape.setOutlineThickness(2);
        powerUp.lifetime = 15.0f; // 15 секунд до исчезновения
        
        powerUps.push_back(powerUp);
    }
    
    bool checkCollision(const sf::Vector2f& pos, const sf::Vector2f& size) {
        sf::FloatRect entityRect(pos, size);
        
        for(const auto& wall : walls) {
            sf::FloatRect wallBounds = wall.getGlobalBounds();
            if(entityRect.findIntersection(wallBounds).has_value()) {
                return true;
            }
        }
        return false;
    }
    
    void updatePlayer(float dt) {
        sf::Vector2f newPos = player.position;
        bool moved = false;
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            newPos.y -= player.speed * dt;
            moved = true;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            newPos.y += player.speed * dt;
            moved = true;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            newPos.x -= player.speed * dt;
            moved = true;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            newPos.x += player.speed * dt;
            moved = true;
        }
        
        if(!checkCollision(newPos, {32.f, 32.f})) {
            player.position = newPos;
        }
        
        // Звук шагов
        if(moved && footstepClock.getElapsedTime().asSeconds() > 0.4f) {
            footstepSound->play();
            footstepClock.restart();
        }
        
        player.shape.setPosition(player.position);
        
        // Проверка буста скорости
        if(hasSpeedBoost && speedBoostClock.getElapsedTime().asSeconds() > 5.0f) {
            hasSpeedBoost = false;
            player.speed = PLAYER_SPEED;
        }
    }
    
    void updateZombies(float dt) {
        for(auto& zombie : zombies) {
            sf::Vector2f direction = player.position - zombie.position;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            
            if(length > 0) {
                direction /= length;
                
                sf::Vector2f newPos = zombie.position + direction * zombie.speed * dt;
                
                if(!checkCollision(newPos, {32.f, 32.f})) {
                    zombie.position = newPos;
                }
            }
            
            zombie.shape.setPosition(zombie.position);
            
            // Проверка столкновения с игроком
            float dist = std::sqrt(std::pow(player.position.x - zombie.position.x, 2) + 
                            std::pow(player.position.y - zombie.position.y, 2));
            
            int damage = (zombie.type == ZombieType::FAST) ? 10 : 5;
            
            if(dist < 40 && damageClock.getElapsedTime().asSeconds() > 0.5f) {
                player.health -= damage;
                hitSound->play();
                damageClock.restart();
                
                if(player.health <= 0) {
                    gameState = GameState::GAME_OVER;
                }
            }
        }
    }
    
    void updatePowerUps(float dt) {
        for(auto it = powerUps.begin(); it != powerUps.end();) {
            it->lifetime -= dt;
            
            if(it->lifetime <= 0) {
                it = powerUps.erase(it);
                continue;
            }
            
            // Проверка подбора игроком
            float dist = std::sqrt(std::pow(player.position.x - it->position.x, 2) + 
                            std::pow(player.position.y - it->position.y, 2));
            
            if(dist < 40) {
                pickupSound->play();
                
                switch(it->type) {
                    case PowerUpType::HEALTH:
                        player.health = std::min(player.health + 30, MAX_HEALTH);
                        break;
                    
                    case PowerUpType::SPEED:
                        hasSpeedBoost = true;
                        player.speed = PLAYER_SPEED * 1.5f;
                        speedBoostClock.restart();
                        break;
                    
                    case PowerUpType::GRENADE:
                        explosionSound->play();
                        zombies.clear();
                        break;
                }
                
                it = powerUps.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void updateCamera() {
        camera.setCenter({player.position.x + 16.f, player.position.y + 16.f});
        
        // Ограничиваем камеру границами карты
        float camX = camera.getCenter().x;
        float camY = camera.getCenter().y;
        
        if(camX < WINDOW_WIDTH/2.f) camX = WINDOW_WIDTH/2.f;
        if(camX > 2000.f - WINDOW_WIDTH/2.f) camX = 2000.f - WINDOW_WIDTH/2.f;
        if(camY < WINDOW_HEIGHT/2.f) camY = WINDOW_HEIGHT/2.f;
        if(camY > 1500.f - WINDOW_HEIGHT/2.f) camY = 1500.f - WINDOW_HEIGHT/2.f;
        
        camera.setCenter({camX, camY});
    }
    
    void resetGame() {
        player.position = {1000.f, 750.f};
        player.health = MAX_HEALTH;
        player.speed = PLAYER_SPEED;
        hasSpeedBoost = false;
        zombies.clear();
        powerUps.clear();
        for(int i = 0; i < INITIAL_ZOMBIES; i++) {
            spawnZombie();
        }
        gameState = GameState::PLAYING;
        survivalTime = 0;
        clock.restart();
        spawnClock.restart();
        powerUpSpawnClock.restart();
    }
    
public:
    Game() : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Death++") {
        srand(static_cast<unsigned int>(time(nullptr)));
        window.setFramerateLimit(60);
        
        // Инициализация игрока
        player.position = {1000.f, 750.f};
        player.speed = PLAYER_SPEED;
        player.health = MAX_HEALTH;
        player.shape.setSize({32.f, 32.f});
        
        // Инициализация камеры
        camera.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});
        camera.setCenter(player.position);
        
        gameState = GameState::MENU;
        survivalTime = 0;
        hasSpeedBoost = false;
        
        initTextures();
        initSounds();
        player.shape.setTexture(playerTex.get());
        initWalls();
        
        // Начальные зомби
        for(int i = 0; i < INITIAL_ZOMBIES; i++) {
            spawnZombie();
        }
    }
    
    void run() {
        while(window.isOpen()) {
            while(const auto event = window.pollEvent()) {
                if(event->is<sf::Event::Closed>()) {
                    window.close();
                }
                
                if(event->is<sf::Event::KeyPressed>()) {
                    const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
                    if(keyEvent) {
                        if(gameState == GameState::MENU && keyEvent->code == sf::Keyboard::Key::Enter) {
                            resetGame();
                        }
                        
                        if(gameState == GameState::GAME_OVER && keyEvent->code == sf::Keyboard::Key::R) {
                            resetGame();
                        }
                    }
                }
            }
            
            if(gameState == GameState::PLAYING) {
                float dt = clock.restart().asSeconds();
                survivalTime += dt;
                
                updatePlayer(dt);
                updateZombies(dt);
                updatePowerUps(dt);
                updateCamera();
                
                // Спавн новых зомби (каждые 2 секунды)
                if(spawnClock.getElapsedTime().asSeconds() > ZOMBIE_SPAWN_INTERVAL) {
                    // Спавним 2 зомби за раз
                    spawnZombie();
                    spawnZombie();
                    spawnClock.restart();
                }
                
                // Спавн power-ups
                if(powerUpSpawnClock.getElapsedTime().asSeconds() > 8.0f) {
                    spawnPowerUp();
                    powerUpSpawnClock.restart();
                }
            }
            
            render();
        }
    }
    
    void render() {
        window.clear(sf::Color(20, 20, 30));
        
        if(gameState == GameState::MENU) {
            renderMenu();
        } else {
            renderGame();
        }
        
        window.display();
    }
    
    void renderMenu() {
        // Темный фон с градиентом
        sf::RectangleShape bgTop({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT / 2)});
        bgTop.setFillColor(sf::Color(20, 10, 30));
        bgTop.setPosition({0, 0});
        window.draw(bgTop);
        
        sf::RectangleShape bgBottom({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT / 2)});
        bgBottom.setFillColor(sf::Color(10, 5, 15));
        bgBottom.setPosition({0, static_cast<float>(WINDOW_HEIGHT / 2)});
        window.draw(bgBottom);
        
        // Заголовок
        sf::Text title(font);
        title.setString("ZOMBIE HORROR");
        title.setCharacterSize(90);
        title.setFillColor(sf::Color(200, 0, 0));
        title.setOutlineColor(sf::Color(100, 0, 0));
        title.setOutlineThickness(3);
        
        // Центрируем текст
        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setPosition({WINDOW_WIDTH/2.f - titleBounds.size.x/2.f, 100.f});
        window.draw(title);
        
        // Subtitle с анимацией
        sf::Text subtitle(font);
        subtitle.setString("Press ENTER to Start");
        subtitle.setCharacterSize(35);
        
        // Мигающий эффект
        float alpha = 128 + 127 * std::sin(clock.getElapsedTime().asSeconds() * 3);
        subtitle.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
        
        sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
        subtitle.setPosition({WINDOW_WIDTH/2.f - subtitleBounds.size.x/2.f, 250.f});
        window.draw(subtitle);
        
        // Controls в рамке
        sf::RectangleShape controlsBox({550.f, 350.f});
        controlsBox.setFillColor(sf::Color(30, 30, 40, 200));
        controlsBox.setOutlineColor(sf::Color(100, 100, 120));
        controlsBox.setOutlineThickness(2);
        controlsBox.setPosition({WINDOW_WIDTH/2.f - 275.f, 330.f});
        window.draw(controlsBox);
        
        sf::Text controls(font);
        controls.setString(
            "        CONTROLS\n"
            "    WASD or Arrow Keys\n\n"
            "       POWER-UPS\n"
            "  Green: Health +30 HP\n"
            "  Cyan: Speed Boost 5s\n"
            "  Red: Kill All Zombies\n\n"
            "        ENEMIES\n"
            "  Green Zombies: Normal\n"
            "  Red Zombies: Fast & Deadly"
        );
        controls.setCharacterSize(22);
        controls.setFillColor(sf::Color(220, 220, 220));
        controls.setLineSpacing(1.3f);
        
        sf::FloatRect controlsBounds = controls.getLocalBounds();
        controls.setPosition({WINDOW_WIDTH/2.f - controlsBounds.size.x/2.f, 350.f});
        window.draw(controls);
        
        // Иконки power-ups для визуализации
        float iconY = 720.f;
        float iconSpacing = 100.f;
        float startX = WINDOW_WIDTH/2.f - iconSpacing;
        
        // Health icon
        sf::CircleShape healthIcon(15);
        healthIcon.setFillColor(sf::Color::Green);
        healthIcon.setOutlineColor(sf::Color::White);
        healthIcon.setOutlineThickness(2);
        healthIcon.setPosition({startX, iconY});
        window.draw(healthIcon);
        
        // Speed icon
        sf::CircleShape speedIcon(15);
        speedIcon.setFillColor(sf::Color::Cyan);
        speedIcon.setOutlineColor(sf::Color::White);
        speedIcon.setOutlineThickness(2);
        speedIcon.setPosition({startX + iconSpacing, iconY});
        window.draw(speedIcon);
        
        // Grenade icon
        sf::CircleShape grenadeIcon(15);
        grenadeIcon.setFillColor(sf::Color::Red);
        grenadeIcon.setOutlineColor(sf::Color::White);
        grenadeIcon.setOutlineThickness(2);
        grenadeIcon.setPosition({startX + iconSpacing * 2, iconY});
        window.draw(grenadeIcon);
    }
    
    void renderGame() {
        window.setView(camera);
        
        // Рисуем пол
        sf::RectangleShape floor({3000.f, 3000.f});
        floor.setPosition({-500.f, -500.f});
        floor.setTexture(floorTex.get());
        floor.setTextureRect(sf::IntRect({0, 0}, {3000, 3000}));
        window.draw(floor);
        
        // Рисуем стены
        for(const auto& wall : walls) {
            window.draw(wall);
        }
        
        // Рисуем power-ups
        for(const auto& powerUp : powerUps) {
            window.draw(powerUp.shape);
            
            // Иконка на power-up
            sf::Text icon(font);
            if(powerUp.type == PowerUpType::HEALTH) icon.setString("+");
            else if(powerUp.type == PowerUpType::SPEED) icon.setString("S");
            else icon.setString("!");
            
            icon.setCharacterSize(20);
            icon.setFillColor(sf::Color::White);
            icon.setPosition(powerUp.position);
            window.draw(icon);
        }
        
        // Рисуем игрока
        window.draw(player.shape);
        
        // Рисуем зомби
        for(const auto& zombie : zombies) {
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
        
        if(player.health > 60) {
            hpBar.setFillColor(sf::Color::Green);
        } else if(player.health > 30) {
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
        
        // Индикатор буста скорости
        if(hasSpeedBoost) {
            float remaining = 5.0f - speedBoostClock.getElapsedTime().asSeconds();
            sf::Text boostText(font);
            boostText.setString("SPEED BOOST: " + std::to_string(static_cast<int>(remaining)) + "s");
            boostText.setCharacterSize(25);
            boostText.setFillColor(sf::Color::Cyan);
            boostText.setPosition({20.f, 130.f});
            window.draw(boostText);
        }
        
        // Game Over экран
        if(gameState == GameState::GAME_OVER) {
            sf::RectangleShape overlay({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);
            
            sf::Text gameOverText(font);
            gameOverText.setString("GAME OVER");
            gameOverText.setCharacterSize(80);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition({WINDOW_WIDTH/2.f - 250.f, WINDOW_HEIGHT/2.f - 100.f});
            window.draw(gameOverText);
            
            sf::Text restartText(font);
            restartText.setString("Press R to Restart");
            restartText.setCharacterSize(30);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition({WINDOW_WIDTH/2.f - 150.f, WINDOW_HEIGHT/2.f + 20.f});
            window.draw(restartText);
            
            sf::Text finalTime(font);
            finalTime.setString("Survived: " + std::to_string(static_cast<int>(survivalTime)) + " seconds");
            finalTime.setCharacterSize(25);
            finalTime.setFillColor(sf::Color::Yellow);
            finalTime.setPosition({WINDOW_WIDTH/2.f - 150.f, WINDOW_HEIGHT/2.f + 70.f});
            window.draw(finalTime);
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
