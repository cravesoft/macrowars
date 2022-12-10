// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __MOVE_STRATEGY_HPP__
#define __MOVE_STRATEGY_HPP__

#include "movable.hpp"

class MoveStrategy
{
public:
    virtual void move(Movable* movable, const float speed) const = 0;

protected:
    float speed_;
    float angle_;
    OBoyLib::Vector2 accel_;
    OBoyLib::Vector2 velocity_;
};

#endif
