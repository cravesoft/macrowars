// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef ORBIT_STRATEGY_HPP
#define ORBIT_STRATEGY_HPP

#include "move_strategy.hpp"

class OrbitStrategy : public MoveStrategy
{
public:
    virtual void move(Movable* movable, const float speed);
};

#endif
