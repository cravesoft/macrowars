// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __TRAVEL_STRATEGY_HPP__
#define __TRAVEL_STRATEGY_HPP__

#include "move_strategy.hpp"

class TravelStrategy : public MoveStrategy
{
public:
    virtual void move(Movable* movable, const float speed);
};

#endif
