#pragma once
#ifndef SPRING_PENDULUM_H
#define SPRING_PENDULUM_H

#include "pendulum.h"
#include "spring_pendulum_diffeq.h"

class SpringPendulum : public Pendulum {
public:

	SpringPendulum(Shader* o_shader, Shader* p_shader, state_type state, float k, glm::vec3 color);

	virtual void operator() (float t, float dt) override;
	virtual void update(float t, float dt) override;
	virtual void draw() override;
	virtual glm::vec2 location() override;

protected:

	float spring_constant;
	SpringPendulumDiffeq* trajectory;
};

#endif