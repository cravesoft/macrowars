// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "Movable.h"

class OriginAndDestination;

class Traveller : virtual public Movable
{
public:
  
  Traveller(void);
  Traveller(OriginAndDestination *origin, OriginAndDestination *destination);
  virtual ~Traveller(void);

  virtual void move(const float speed);
  void travelToDestination(const float speed);
  bool hasArrivedAtDestination() const;
  void setOriginAndDestination(OriginAndDestination* origin, OriginAndDestination* destination);

  inline OriginAndDestination* getDestination() const { return mDestination; };
  void setDestination(OriginAndDestination *destination);
  
  inline OriginAndDestination* getOrigin() const { return mOrigin; };
  void setOrigin(OriginAndDestination *origin);

protected:

  OriginAndDestination *mDestination;
  OriginAndDestination *mOrigin;
};