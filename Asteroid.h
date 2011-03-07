// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "oboylib/OBoyUtil.h"
#include "oboylib/Vector2.h"

#include "Body.h"
#include "Ship.h"

class Asteroid : public Body
{
public:

  Asteroid(void);
  virtual ~Asteroid(void);

  virtual void draw(OBoy::Graphics *g, const OBoyLib::Vector2 camera);
  virtual void handleShipCollision(Ship* ship);
  void update();

private:

	void _draw(OBoy::Graphics *g, OBoyLib::Vector2 &pos);

private:

  OBoy::Image *mImage;
};
