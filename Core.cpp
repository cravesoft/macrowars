// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "Core.h"

#define CORE_DEFAULT_SPEED 5
#define CORE_DEFAULT_RADIUS 36

Core::Core(void) : Planet()
{
  this->setRadius(CORE_DEFAULT_RADIUS);
  mSpeed = CORE_DEFAULT_SPEED;
  mImage = oboy::Environment::getImage("IMAGE_CORE");
}