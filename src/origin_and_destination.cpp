// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <algorithm>
#include <assert.h>

#include "origin_and_destination.hpp"
#include "traveller.hpp"

void OriginAndDestination::addArrivingTraveller(Traveller* traveller)
{
    assert(find(arriving_travellers_.begin(),arriving_travellers_.end(),traveller) == arriving_travellers_.end());
    arriving_travellers_.push_back(traveller);
}

void OriginAndDestination::addLeavingTraveller(Traveller* traveller)
{
    assert(find(leaving_travellers_.begin(),leaving_travellers_.end(),traveller) == leaving_travellers_.end());
    leaving_travellers_.push_back(traveller);
}

void OriginAndDestination::removeArrivingTraveller(Traveller* traveller)
{
    assert(find(arriving_travellers_.begin(),arriving_travellers_.end(),traveller) != arriving_travellers_.end());
    arriving_travellers_.erase(find(arriving_travellers_.begin(),arriving_travellers_.end(),traveller));
}

void OriginAndDestination::removeLeavingTraveller(Traveller* traveller)
{
    assert(find(leaving_travellers_.begin(),leaving_travellers_.end(),traveller) != leaving_travellers_.end());
    leaving_travellers_.erase(find(leaving_travellers_.begin(),leaving_travellers_.end(),traveller));
}
