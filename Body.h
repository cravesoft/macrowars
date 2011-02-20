// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "OBoy/Environment.h"
#include "OBoy/Graphics.h"
#include "oboylib/OBoyUtil.h"
#include "oboylib/Vector2.h"

#define RADIUS_STEP_SIZE 10

class Ship;

#if 0
enum Colors {RED, BLUE, YELLOW, MAGENTA, CYAN, GREEN, WHITE, GRAY};

const oboy::Color kColors[9] = {
    0xffff0000,  // RED
    0xff0000ff,  // BLUE
    0xffffff00,  // YELLOW
    0xffff00ff,  // MAGENTA
    0xff00ffff,  // CYAN
    0xff00ff00,  // GREEN
    0xffffffff,  // WHITE
    0xfff1f1f1   // GRAY
};
#endif

// Body
class Body
{
public:

  Body(void) {};
  virtual ~Body(void) {};

  virtual void draw(oboy::Graphics *g, const oboylib::Vector2 camera) = 0;
  virtual bool isTouched(const oboylib::Vector2 position) const;
  virtual void handleShipCollision(Ship* ship) = 0;

  virtual oboylib::Vector2 getPosition() const { return mPos; }
  virtual void setPosition(const oboylib::Vector2 position) { mPos = position; }
  
  virtual int getRadius() const { return mRadius; }
  virtual void setRadius(const int radius) { mRadius = radius; };

  virtual oboylib::Color getColor() const { return mColor; }
  virtual void setColor(const oboylib::Color color) { mColor = color; };

protected:

  int mRadius;
  oboylib::Color mColor;
  oboylib::Vector2 mPos;
};
