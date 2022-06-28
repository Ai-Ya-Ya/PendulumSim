#include "pendulum.h"

// static members' initialization
unsigned int Pendulum::VAO = 1, Pendulum::VBO = 1, Pendulum::EBO = 1;
float Pendulum::vertices[12] = {
	 0.002f,  0.5f, 0.0f,  // top right
	 0.002f, -0.5f, 0.0f,  // bottom right
	-0.002f, -0.5f, 0.0f,  // bottom left
	-0.002f,  0.5f, 0.0f   // top left 
};
unsigned int Pendulum::indices[6] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};
float Pendulum::gravity = 9.81f;
int Pendulum::n_particles = 5000;
float Pendulum::decay = 0.5f;
int Pendulum::n_steps = 100;
float Pendulum::initial_delay = 3.0f; // 3-second delay

// set up openGL drawing things
void Pendulum::init() {
	/*unsigned int VAO, VBO, EBO;*/
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// constructor
Pendulum::Pendulum(Shader* o_shader, Shader* p_shader, state_type state, glm::vec3 color)
	: object_shader(o_shader), particle_shader(p_shader), state(state), color(color), trans(1.0f) {
	particles = new ParticleGenerator(p_shader, n_particles, color, decay);
}