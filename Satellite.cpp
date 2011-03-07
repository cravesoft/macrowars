// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <stdio.h>
#include <float.h>

#include "Satellite.h"
#include "Orbitable.h"

Satellite::Satellite(void) : Movable()
{
  mBarycenter = NULL;
  mOrbiting = false;
}

Satellite::~Satellite(void)  
{
  if (mBarycenter != NULL)
    mBarycenter->releaseSatellite(this);
}

void Satellite::startOrbiting(Orbitable *barycenter, int radius)
{
  mOrbiting = true;
  mBarycenter = barycenter;
  mBarycenter->addSatellite(this);
  mOrbitingRadius = radius;
}

void Satellite::stopOrbiting()
{
  mOrbiting = false;
  assert (mBarycenter != NULL);
  mBarycenter->releaseSatellite(this);
  mBarycenter = NULL;
}

void Satellite::move(float speed)
{
  orbit(speed);
}

void Satellite::orbit(float speed)
{
  OBoyLib::Vector2 dir = mBarycenter->getPosition() - mPos;
  OBoyLib::Vector2 perpendicularAccel;
  OBoyLib::Vector2 parallelAccel;

  // set the perpendicular acceleration
  perpendicularAccel = dir.perp().normalize();

  float distanceBetweenBarycenterAndSatellite = dist(mBarycenter->getPosition(), mPos);

  // set the parallel acceleration:
  if (distanceBetweenBarycenterAndSatellite > mOrbitingRadius)
    parallelAccel = dir.normalize();
  else
    parallelAccel = -dir.normalize();

  float distanceBetweenSatelliteAndOrbit = mOrbitingRadius - distanceBetweenBarycenterAndSatellite;

  // set the satellite acceleration based on parallel and perpendicular accceleration:
  if (distanceBetweenSatelliteAndOrbit < 0)
  {
    float factor = -1.0f/distanceBetweenSatelliteAndOrbit;
    if (factor > 1)
      factor = 1.0f;
    mAccel = parallelAccel * (1.0f - factor) + perpendicularAccel * factor;
  }
  else
  {
    mAccel = perpendicularAccel;
  }

  mVelocity = mVelocity*(1.0f-speed) + mAccel*speed;
  mAngle = rad2deg(atan2(mVelocity.y(), mVelocity.x()));
  mPos += mVelocity*speed*40;
}