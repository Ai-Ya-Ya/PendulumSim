#pragma once
#ifndef DOUBLE_PENDULUM_H
#define DOUBLE_PENDULUM_H

#include "pendulum.h"
class DoublePendulum :
    public Pendulum
{
public:

    DoublePendulum(Shader* o_shader, Shader* p_shader, state_type state, glm::vec3 color);
    virtual void update(float t, float dt) override;
    virtual void draw() override;
    virtual glm::vec2 location() override;

protected:

    static void diffeq(const state_type& x, state_type& dxdt, const double /* t */);
    glm::vec2 first_location();
    glm::mat4 trans2;
};

#endif