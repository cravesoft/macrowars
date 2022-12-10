// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <algorithm>

#include "orbitable.hpp"
#include "satellite.hpp"

void Orbitable::addSatellite(Satellite* satellite)
{
    assert(find(satellites_.begin(),satellites_.end(),satellite) == satellites_.end());
    satellites_.push_back(satellite);
}

void Orbitable::releaseSatellite(Satellite* satellite)
{
    assert(find(satellites_.begin(),satellites_.end(),satellite) != satellites_.end());
    satellites_.erase(find(satellites_.begin(),satellites_.end(),satellite));
}
