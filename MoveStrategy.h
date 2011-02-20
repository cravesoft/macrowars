// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "Movable.h"

class MoveStrategy
{
public:

  virtual void move(Movable* movable, const float speed) const = 0;

protected:

  float mSpeed;
  float mAngle;
  BoyLib::Vector2 mAccel;
  BoyLib::Vector2 mVelocity;
};