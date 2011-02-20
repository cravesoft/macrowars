// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <stdio.h>

#include "TravelStrategy.h"

TravellerStrategy::TravellerStrategy(void)
{
  mOrigin = NULL;
  mDestination = NULL;
}

TravellerStrategy::TravellerStrategy(OriginAndDestination *origin, OriginAndDestination *destination) : Movable()
{
  mOrigin = origin;
  mDestination = destination;
  mOrigin->addLeavingTraveller(this);
  mDestination->addArrivingTraveller(this);
}

TravellerStrategy::~TravellerStrategy(void)  
{
  if (mOrigin != NULL)
    mOrigin->removeLeavingTraveller(this);
  if (mDestination != NULL)
    mDestination->removeArrivingTraveller(this);
}

void TravellerStrategy::move(Movable* movable, const float speed);
{
  BoyLib::Vector2 dir = mDestination->getPosition() - movable->mPos;
  movable->mAccel = dir.normalize();
  movable->mVelocity = movable->mVelocity*(1.0f-speed) + movable->mAccel*speed;
  movable->mAngle = rad2deg(atan2(movable->mVelocity.y, movable->mVelocity.x));
  movable->mPos += movable->mVelocity*speed*40;
}

bool TravellerStrategy::hasArrivedAtDestination() const
{
  if ((mPos - mDestination->getPosition()).magnitude() < mDestination->getRadius())
    return true;
  else
    return false;
}

void TravellerStrategy::setOriginAndDestination(OriginAndDestination* origin, OriginAndDestination* destination)
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

void TravellerStrategy::setDestination(OriginAndDestination *destination)
{
  if (mDestination != NULL)
    mDestination->removeArrivingTraveller(this);
  mDestination = destination;
  mDestination->addArrivingTraveller(this);
}

void TravellerStrategy::setOrigin(OriginAndDestination *origin)
{
  if (mOrigin != NULL)
    mOrigin->removeLeavingTraveller(this);
  mOrigin = origin;
  mOrigin->addLeavingTraveller(this);
}