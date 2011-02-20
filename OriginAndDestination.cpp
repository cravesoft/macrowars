// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <algorithm>
#include <assert.h>

#include "OriginAndDestination.h"
#include "Traveller.h"

void OriginAndDestination::addArrivingTraveller(Traveller* traveller)
{
  assert(find(mArrivingTravellers.begin(),mArrivingTravellers.end(),traveller) == mArrivingTravellers.end());
  mArrivingTravellers.push_back(traveller);
}

void OriginAndDestination::addLeavingTraveller(Traveller* traveller)
{
  assert(find(mLeavingTravellers.begin(),mLeavingTravellers.end(),traveller) == mLeavingTravellers.end());
  mLeavingTravellers.push_back(traveller);
}

void OriginAndDestination::removeArrivingTraveller(Traveller* traveller)
{
  assert(find(mArrivingTravellers.begin(),mArrivingTravellers.end(),traveller) != mArrivingTravellers.end());
	mArrivingTravellers.erase(find(mArrivingTravellers.begin(),mArrivingTravellers.end(),traveller));
}

void OriginAndDestination::removeLeavingTraveller(Traveller* traveller)
{
  assert(find(mLeavingTravellers.begin(),mLeavingTravellers.end(),traveller) != mLeavingTravellers.end());
	mLeavingTravellers.erase(find(mLeavingTravellers.begin(),mLeavingTravellers.end(),traveller));
}