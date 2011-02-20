// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "Body.h"
#include "OriginAndDestination.h"

class Satellite;

class Orbitable : virtual public Body
{
  public:

    void addSatellite(Satellite* satellite);
    void releaseSatellite(Satellite* satellite);

    inline std::vector<Satellite*>& getSatellites() { return mSatellites; };

  private:

    std::vector<Satellite*> mSatellites;
};