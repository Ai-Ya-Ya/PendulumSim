#pragma once
#ifndef SPRING_PENDULUM_DIFFEQ_H
#define SPRING_PENDULUM_DIFFEQ_H

#include <vector>

typedef std::vector< float > state_type;

class SpringPendulumDiffeq
{
public:

	SpringPendulumDiffeq(float spring_constant);
	void operator() (const state_type& x, state_type& dxdt, const double /* t */);

protected:

	float spring_constant;
	static float gravity;
};

#endif

