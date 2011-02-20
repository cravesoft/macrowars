// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include <vector>

#include "Body.h"

class Traveller;

// OriginAndDestination
class OriginAndDestination : virtual public Body
{
  public:

    void addArrivingTraveller(Traveller* traveller);
    void addLeavingTraveller(Traveller* traveller);

    void removeLeavingTraveller(Traveller* traveller);
    void removeArrivingTraveller(Traveller* traveller);

    inline std::vector<Traveller*>& getArrivingTravellers() { return mArrivingTravellers; };
    inline std::vector<Traveller*>& getLeavingTravellers() { return mLeavingTravellers; };
    
  private:

    std::vector<Traveller*> mArrivingTravellers;
    std::vector<Traveller*> mLeavingTravellers;
};