#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "own_shaders.h"

// Represents a single particle and its state
struct Particle {
    glm::vec2 Position;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Color(1.0f), Life(0.0f) { }
    Particle(glm::vec4 color) {
        Position = glm::vec2(0.0f);
        Life = 0.0f;
        Color = color;
    }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Shader* shader, unsigned int amount, glm::vec3 color, float decay = 1.0f);
    // update all particles
    void Update(float dt, glm::vec2 position, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void Draw();
    static void setup();
    static unsigned int VAO;
    static unsigned int VBO;
    static float particle_quad[12];
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader* shader;
    float decay;

    // particle color
    glm::vec4 color;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, glm::vec2 position, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif