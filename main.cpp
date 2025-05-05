#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;
const float BIRD_RADIUS = 15.0f;
const float GRAVITY = 1100.0f;
const float FLAP_STRENGTH = -350.0f;
const float PIPE_WIDTH = 80.0f;
const float PIPE_GAP = 150.0f;
const float PIPE_SPEED = 200.0f;
const float PIPE_SPAWN_TIME = 1.8f;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(SCREEN_WIDTH), static_cast<unsigned int>(SCREEN_HEIGHT)), "Flappy Bird C++");
    window.setFramerateLimit(60);

    sf::Clock clock;
    float deltaTime = 0.0f;

    sf::CircleShape bird(BIRD_RADIUS);
    bird.setFillColor(sf::Color::Yellow);
    bird.setOrigin(BIRD_RADIUS, BIRD_RADIUS);
    bird.setPosition(SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f);
    float birdVelocityY = 0.0f;

    std::vector<sf::RectangleShape> topPipes;
    std::vector<sf::RectangleShape> bottomPipes;
    std::vector<bool> pipeScored;
    float pipeSpawnTimer = PIPE_SPAWN_TIME;

    int score = 0;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error: Could not load font arial.ttf" << std::endl;
        return -1;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 10);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over!\nPress R to Restart");
    sf::FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
                         textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

    enum GameState { Playing, GameOver };
    GameState currentState = Playing;

    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && currentState == Playing) {
                if (event.key.code == sf::Keyboard::Space) {
                    birdVelocityY = FLAP_STRENGTH;
                }
            }

             if (event.type == sf::Event::KeyPressed && currentState == GameOver) {
                 if (event.key.code == sf::Keyboard::R) {
                     currentState = Playing;
                     bird.setPosition(SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f);
                     birdVelocityY = 0.0f;
                     topPipes.clear();
                     bottomPipes.clear();
                     pipeScored.clear();
                     pipeSpawnTimer = PIPE_SPAWN_TIME;
                     score = 0;
                 }
            }
        }

        if (currentState == Playing) {
            birdVelocityY += GRAVITY * deltaTime;
            bird.move(0, birdVelocityY * deltaTime);

            if (bird.getPosition().y - BIRD_RADIUS < 0) {
                bird.setPosition(bird.getPosition().x, BIRD_RADIUS);
                birdVelocityY = 0;
            }
            if (bird.getPosition().y + BIRD_RADIUS > SCREEN_HEIGHT) {
                bird.setPosition(bird.getPosition().x, SCREEN_HEIGHT - BIRD_RADIUS);
                birdVelocityY = 0;
                 currentState = GameOver;
            }

            pipeSpawnTimer += deltaTime;
            if (pipeSpawnTimer >= PIPE_SPAWN_TIME) {
                pipeSpawnTimer = 0;

                float gapCenterY = randomFloat(PIPE_GAP / 2.0f + 50.0f, SCREEN_HEIGHT - PIPE_GAP / 2.0f - 50.0f);
                float topPipeHeight = gapCenterY - PIPE_GAP / 2.0f;
                float bottomPipeY = gapCenterY + PIPE_GAP / 2.0f;
                float bottomPipeHeight = SCREEN_HEIGHT - bottomPipeY;

                sf::RectangleShape topPipe(sf::Vector2f(PIPE_WIDTH, topPipeHeight));
                topPipe.setFillColor(sf::Color::Green);
                topPipe.setPosition(SCREEN_WIDTH, 0);
                topPipes.push_back(topPipe);

                sf::RectangleShape bottomPipe(sf::Vector2f(PIPE_WIDTH, bottomPipeHeight));
                bottomPipe.setFillColor(sf::Color::Green);
                bottomPipe.setPosition(SCREEN_WIDTH, bottomPipeY);
                bottomPipes.push_back(bottomPipe);

                pipeScored.push_back(false);
            }

            for (size_t i = 0; i < topPipes.size(); ++i) {
                topPipes[i].move(-PIPE_SPEED * deltaTime, 0);
                bottomPipes[i].move(-PIPE_SPEED * deltaTime, 0);

                if (!pipeScored[i] && topPipes[i].getPosition().x + PIPE_WIDTH < bird.getPosition().x - BIRD_RADIUS) {
                    score++;
                    pipeScored[i] = true;
                }

                if (bird.getGlobalBounds().intersects(topPipes[i].getGlobalBounds()) ||
                    bird.getGlobalBounds().intersects(bottomPipes[i].getGlobalBounds()))
                {
                     currentState = GameOver;
                }
            }

             if (!topPipes.empty() && topPipes[0].getPosition().x + PIPE_WIDTH < 0) {
                 topPipes.erase(topPipes.begin());
                 bottomPipes.erase(bottomPipes.begin());
                 pipeScored.erase(pipeScored.begin());
             }

            scoreText.setString("Score: " + std::to_string(score));
        }

        window.clear(sf::Color::Cyan);

        for (const auto& pipe : topPipes) {
            window.draw(pipe);
        }
        for (const auto& pipe : bottomPipes) {
            window.draw(pipe);
        }

        window.draw(bird);

        window.draw(scoreText);

        if(currentState == GameOver) {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}