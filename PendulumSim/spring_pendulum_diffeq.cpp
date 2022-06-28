#include "spring_pendulum_diffeq.h"

float SpringPendulumDiffeq::gravity = 9.81f;

SpringPendulumDiffeq::SpringPendulumDiffeq(float spring_constant) : spring_constant(spring_constant) {}

void SpringPendulumDiffeq::operator() (const state_type& x, state_type& dxdt, const double /* t */) {
	dxdt[0] = x[1];
	dxdt[1] = (1.0f + x[0]) * x[3] * x[3] - spring_constant * x[0] + gravity * cos(x[2]); // mass 1
	dxdt[2] = x[3];
	dxdt[3] = -gravity * sin(x[2]) / (1.0f + x[0]) - 2 * x[1] * x[3] / (1.0f + x[0]);
}