// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <algorithm>

#include "Orbitable.h"
#include "Satellite.h"

void Orbitable::addSatellite(Satellite* satellite)
{
  assert(find(mSatellites.begin(),mSatellites.end(),satellite) == mSatellites.end());
  mSatellites.push_back(satellite);
}

void Orbitable::releaseSatellite(Satellite* satellite)
{
  assert(find(mSatellites.begin(),mSatellites.end(),satellite) != mSatellites.end());
  mSatellites.erase(find(mSatellites.begin(),mSatellites.end(),satellite));
}