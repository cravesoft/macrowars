// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "Body.h"

class Movable : public Body
{
public:

  Movable(void);
  virtual ~Movable(void) {};
  
  virtual void move(const float speed) = 0;

protected:

  float mSpeed;
  float mAngle;
  OBoyLib::Vector2 mAccel;
  OBoyLib::Vector2 mVelocity;
};