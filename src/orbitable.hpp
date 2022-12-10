// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __ORBITABLE_HPP__
#define __ORBITABLE_HPP__

#include "body.hpp"
#include "origin_and_destination.hpp"

class Satellite;

class Orbitable : virtual public Body
{
public:
    void addSatellite(Satellite* satellite);
    void releaseSatellite(Satellite* satellite);

    inline std::vector<Satellite*>& getSatellites() { return satellites_; };

private:
    std::vector<Satellite*> satellites_;
};

#endif
