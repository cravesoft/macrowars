// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "Movable.h"

class Orbitable;

class Satellite : virtual public Movable
{
public:
  
  Satellite(void);
  virtual ~Satellite(void);

  // start and stop orbit sequence
  virtual void startOrbiting(Orbitable *barycenter, int radius);
  virtual void stopOrbiting();

  inline bool isOrbiting() const { return mOrbiting; };

  virtual void move(const float dt);
  void orbit(const float dt);

protected:

  Orbitable *mBarycenter;
  int mOrbitingRadius;
  bool mOrbiting;
};