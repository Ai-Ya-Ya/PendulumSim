#include "double_pendulum.h"

DoublePendulum::DoublePendulum(Shader* o_shader, Shader* p_shader, state_type state, glm::vec3 color)
	: Pendulum(o_shader, p_shader, state, color), trans2(1.0f) {}

void DoublePendulum::operator() (float t, float dt) {
	update(t, dt);
}

void DoublePendulum::update(float time, float dt) {
	time -= initial_delay;
	if (time >= 0.0f) {
		// solve diffeq and add particles
		boost::numeric::odeint::integrate(diffeq, state, time - dt, time, dt / n_steps);
		particles->Update(dt, location(), 1);
	}

	// update position
	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 0.5f, 0.0f));
	trans = glm::rotate(trans, state[0], glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(1.0f, 0.5f, 1.0f));
	trans = glm::translate(trans, glm::vec3(0.0f, -0.5f, 0.0f));

	trans2 = glm::mat4(1.0f);
	trans2 = glm::translate(trans2, glm::vec3(first_location(), 0.0f));
	trans2 = glm::rotate(trans2, state[1], glm::vec3(0.0f, 0.0f, 1.0f));
	trans2 = glm::scale(trans2, glm::vec3(1.0f, 0.5f, 1.0f));
	trans2 = glm::translate(trans2, glm::vec3(0.0f, -0.5f, 0.0f));
}

void DoublePendulum::draw() {
	glEnable(GL_MULTISAMPLE);
	this->object_shader->use();
	this->object_shader->setVector3f("ourColor", color);
	this->object_shader->setMatrix4("transform", trans);
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	this->object_shader->setMatrix4("transform", trans2);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_MULTISAMPLE);

	particles->Draw();
}

glm::vec2 DoublePendulum::location() {
	return first_location() + glm::vec2(0.5f * sin(state[1]), -0.5f * cos(state[1]));
}

void DoublePendulum::diffeq(const state_type& x, state_type& dxdt, const double /* t */) {
	dxdt[0] = 24 * (2 * x[2] - 3 * cos(x[0] - x[1]) * x[3]) / (16 - 9 * cos(x[0] - x[1]) * cos(x[0] - x[1]));
	dxdt[1] = 24 * (8 * x[3] - 3 * cos(x[0] - x[1]) * x[2]) / (16 - 9 * cos(x[0] - x[1]) * cos(x[0] - x[1]));
	dxdt[2] = -(dxdt[0] * dxdt[1] * sin(x[0] - x[1]) + 6 * gravity * sin(x[0])) / 8.0f;
	dxdt[3] = -(-dxdt[0] * dxdt[1] * sin(x[0] - x[1]) + 2 * gravity * sin(x[1])) / 8.0f;
}

glm::vec2 DoublePendulum::first_location() {
	//std::cout << 0.5f * sin(state[0]) << " " << 0.5f * (1 - cos(state[0])) << std::endl;
	return glm::vec2(0.5f * sin(state[0]), 0.5f * (1 - cos(state[0])));
}