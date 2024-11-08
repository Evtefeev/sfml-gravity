#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "Settings.h"
#include "Particle.h"

int main()
{
    Settings settings;

    if (settings.loadSettings("settings.txt"))
    {
        settings.print_fields();
    }
    else
    {
        std::cerr << "Could not load settings.\n";
    }

    sf::RenderWindow window(sf::VideoMode(800, 600), "Gravity Simulation with Camera Control");
    window.setFramerateLimit(60);

    // Initialize camera view
    sf::View cameraView = window.getDefaultView();

    // Define multiple gravity sources
    // std::vector<sf::Vector2f> gravitySources = {{200.f, 150.f}, {600.f, 450.f}};
    // std::vector<float> sourceMasses = {50.f, 50.f}; // Masses of gravity sources
    std::vector<sf::Vector2f> gravitySources = {};
    std::vector<float> sourceMasses = {}; // Masses of gravity sources

    // Draw gravity sources
    std::vector<sf::CircleShape> gravityShapes;
    for (const auto &source : gravitySources)
    {
        sf::CircleShape gravityCenter(5.f);
        gravityCenter.setPosition(source);
        gravityCenter.setFillColor(sf::Color::Red);
        gravityCenter.setOrigin(5.f, 5.f);
        gravityShapes.push_back(gravityCenter);
    }

    // Create particles
    std::vector<Particle> particles;
    for (int i = 0; i < settings.get("numOfParticles"); ++i)
    {
        float x = static_cast<float>(rand() % 800);
        float y = static_cast<float>(rand() % 600);
        particles.emplace_back(x, y, settings.get("initialMass")); // Mass of 1 for simplicity
    }

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) // Path to system font
    {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    sf::Text particleInfoText;
    particleInfoText.setFont(font);
    particleInfoText.setCharacterSize(14);
    particleInfoText.setFillColor(sf::Color::White);
    particleInfoText.setPosition(10.f, 10.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle zoom
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta > 0)
                    cameraView.zoom(1.f / settings.get("zoomFactor")); // Zoom in
                else
                    cameraView.zoom(settings.get("zoomFactor")); // Zoom out
            }
        }

        // Camera movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cameraView.move(0, -settings.get("cameraSpeed")); // Move up
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cameraView.move(0, settings.get("cameraSpeed")); // Move down
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cameraView.move(-settings.get("cameraSpeed"), 0); // Move left
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cameraView.move(settings.get("cameraSpeed"), 0); // Move right

        // Update particles
        for (auto &particle : particles)
        {
            // Gravity from each source
            for (size_t i = 0; i < gravitySources.size(); ++i)
            {
                sf::Vector2f gravity = calculateGravity(particle, gravitySources[i], sourceMasses[i], settings);
                particle.velocity += gravity * settings.get("dt");
            }

            // Particle-particle attraction
            for (const auto &other : particles)
            {
                if (&particle != &other)
                {
                    sf::Vector2f gravity = calculateGravity(particle, other.shape.getPosition(), other.mass, settings);
                    particle.velocity += gravity * settings.get("dt");
                }
            }

            // Update position based on velocity
            particle.shape.move(particle.velocity * settings.get("dt"));
        }

        // Handle collisions
        handleCollisions(particles, settings);

        // Get mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Check if the mouse is hovering over any particle
        bool mouseOverParticle = false;
        for (const auto &particle : particles)
        {
            if (particle.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                // Format particle info as text
                std::stringstream ss;
                ss << "Particle Info: \n"
                   << "Position: (" << particle.shape.getPosition().x << ", " << particle.shape.getPosition().y << ")\n"
                   << "Velocity: (" << particle.velocity.x << ", " << particle.velocity.y << ")\n"
                   << "Mass: " << particle.mass;
                particleInfoText.setString(ss.str());

                mouseOverParticle = true;
                break;
            }
        }

        // If no particle is under the mouse, hide the particle info
        if (!mouseOverParticle)
        {
            particleInfoText.setString("");
        }

        // Set the view to the camera
        window.setView(cameraView);

        // Draw everything
        window.clear();
        for (const auto &gravityCenter : gravityShapes)
        {
            window.draw(gravityCenter);
        }
        for (const auto &particle : particles)
        {
            window.draw(particle.shape);
        }

        // Draw particle info if hovered
        window.draw(particleInfoText);

        window.display();
    }

    return 0;
}
