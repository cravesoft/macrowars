// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <stdio.h>

#include "Traveller.h"
#include "OriginAndDestination.h"

Traveller::Traveller(void) : Movable()
{
  mOrigin = NULL;
  mDestination = NULL;
}

Traveller::Traveller(OriginAndDestination *origin, OriginAndDestination *destination) : Movable()
{
  mOrigin = origin;
  mDestination = destination;
  mOrigin->addLeavingTraveller(this);
  mDestination->addArrivingTraveller(this);
}

Traveller::~Traveller(void)  
{
  if (mOrigin != NULL)
    mOrigin->removeLeavingTraveller(this);
  if (mDestination != NULL)
    mDestination->removeArrivingTraveller(this);
}

void Traveller::move(float speed)  
{
  travelToDestination(speed);
}

void Traveller::travelToDestination(float speed)  
{
  OBoyLib::Vector2 dir = mDestination->getPosition() - mPos;
  mAccel = dir.normalize();
  mVelocity = mVelocity*(1.0f-speed) + mAccel*speed;
  mAngle = rad2deg(atan2(mVelocity.y(), mVelocity.x()));
  mPos += mVelocity*speed*40;
}

bool Traveller::hasArrivedAtDestination() const
{
  if ((mPos - mDestination->getPosition()).magnitude() < mDestination->getRadius())
    return true;
  else
    return false;
}

void Traveller::setOriginAndDestination(OriginAndDestination* origin, OriginAndDestination* destination)
{
  if (mOrigin != NULL)
    mOrigin->removeLeavingTraveller(this);
  if (mDestination != NULL)
    mDestination->removeArrivingTraveller(this);
  mOrigin = origin;
  mDestination = destination;
  mOrigin->addLeavingTraveller(this);
  mDestination->addArrivingTraveller(this);
}

void Traveller::setDestination(OriginAndDestination *destination)
{
  if (mDestination != NULL)
    mDestination->removeArrivingTraveller(this);
  mDestination = destination;
  mDestination->addArrivingTraveller(this);
}

void Traveller::setOrigin(OriginAndDestination *origin)
{
  if (mOrigin != NULL)
    mOrigin->removeLeavingTraveller(this);
  mOrigin = origin;
  mOrigin->addLeavingTraveller(this);
}