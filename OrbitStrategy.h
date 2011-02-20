// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "MoveStrategy.h"

class OrbitStrategy : public MoveStrategy
{
public:

  virtual void move(Movable* movable, const float speed);
};