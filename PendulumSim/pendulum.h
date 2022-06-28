#pragma once
#ifndef PENDULUM_H
#define PENDULUM_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "own_shaders.h"
#include <vector>
#include <boost/numeric/odeint.hpp>
#include "particle_generator.h"

typedef std::vector< float > state_type;

class Pendulum {
public:

	Pendulum(Shader* o_shader, Shader* p_shader, state_type state, glm::vec3 color);
	static void init();

	virtual void update(float t, float dt) = 0;
	virtual void draw() = 0;
	virtual glm::vec2 location() = 0;

protected:

	// things for drawing the actual bar -- 
	// these are shared resources since matrix transformations make up the actual changes
	static unsigned int VAO, VBO, EBO;
	static float vertices[12];
	static unsigned int indices[6];
	static float gravity;
	static int n_particles;
	static float decay;
	static int n_steps;
	static float initial_delay;

	ParticleGenerator* particles;
	glm::mat4 trans;
	glm::vec3 color;
	state_type state;
	Shader* object_shader, * particle_shader;
};

#endif