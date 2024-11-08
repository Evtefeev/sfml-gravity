#ifndef __PARTICLE_H__
#define __PARTICLE_H__

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

struct Particle
{
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float mass;

    Particle(float x, float y, float mass) : mass(mass), velocity(0.f, 0.f)
    {
        shape.setPosition(x, y);
        shape.setRadius(3.f + mass * 0.5f); // Adjust radius based on mass
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(shape.getRadius(), shape.getRadius()); // Center origin
    }
};

sf::Vector2f calculateGravity(const Particle &p1, const sf::Vector2f &p2Position, float mass2, Settings settings)
{
    sf::Vector2f direction = p2Position - p1.shape.getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance < 1)
        return {0, 0}; // Avoid division by zero and very close distances

    direction /= distance; // Normalize direction
    float forceMagnitude = (settings.get("G") * p1.mass * mass2) / (distance * distance);
    return direction * forceMagnitude;
}

void handleCollisions(std::vector<Particle> &particles, Settings settings)
{
    for (size_t i = 0; i < particles.size(); ++i)
    {
        for (size_t j = i + 1; j < particles.size(); ++j)
        {
            sf::Vector2f diff = particles[j].shape.getPosition() - particles[i].shape.getPosition();
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (distance < settings.get("collisionDistance"))
            {
                // Calculate the combined mass and velocity
                float newMass = particles[i].mass + particles[j].mass;
                sf::Vector2f newVelocity = (particles[i].velocity * particles[i].mass + particles[j].velocity * particles[j].mass) / newMass / settings.get("energyAbsFactor");

                std::cout << particles[i].velocity.x << " ";
                std::cout << particles[i].velocity.y << " ";
                std::cout << particles[i].mass << std::endl;

                std::cout << particles[j].velocity.x << " ";
                std::cout << particles[j].velocity.y << " ";
                std::cout << particles[j].mass << std::endl;

                // Merge particles by updating particle i
                particles[i].mass = newMass;
                particles[i].velocity = newVelocity;

                // Resize shape of particle i to reflect increased mass
                particles[i].shape.setRadius(3.f + particles[i].mass * 0.5f);
                particles[i].shape.setOrigin(particles[i].shape.getRadius(), particles[i].shape.getRadius());

                // Remove particle j
                particles.erase(particles.begin() + j);
                --j; // Adjust index since we removed a particle
            }
        }
    }
}
#endif // __PARTICLE_H__