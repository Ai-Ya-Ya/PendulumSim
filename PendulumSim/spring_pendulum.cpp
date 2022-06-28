#include "spring_pendulum.h"

SpringPendulum::SpringPendulum(Shader* o_shader, Shader* p_shader, state_type state, float k, glm::vec3 color)
	: Pendulum(o_shader, p_shader, state, color) {
	spring_constant = k;
	trajectory = new SpringPendulumDiffeq(k);
}

void SpringPendulum::update(float time, float dt) {
	time -= initial_delay;
	if (time >= 0.0f) {
		// solve diffeq and add particles
		boost::numeric::odeint::integrate(*trajectory, state, time - dt, time, dt / n_steps);
		particles->Update(dt, location(), 1);
	}

	// update position
	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 0.5f, 0.0f));
	trans = glm::rotate(trans, state[2], glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(1.0f, 1.0f + state[0], 1.0f));
	trans = glm::translate(trans, glm::vec3(0.0f, -0.5f, 0.0f));
}

void SpringPendulum::draw() {
	this->object_shader->use();
	this->object_shader->setVector3f("ourColor", color);
	this->object_shader->setMatrix4("transform", trans);
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	particles->Draw();
}

glm::vec2 SpringPendulum::location() {
	return glm::vec2((1.0f + state[0]) * sin(state[2]), 0.5f - (1.0f + state[0]) * cos(state[2]));
}