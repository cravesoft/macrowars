// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __SATELLITE_HPP__
#define __SATELLITE_HPP__

#include "movable.hpp"

class Orbitable;

class Satellite : virtual public Movable
{
public:
    Satellite(void);
    virtual ~Satellite(void);

    // start and stop orbit sequence
    virtual void startOrbiting(Orbitable *barycenter, int radius);
    virtual void stopOrbiting();

    inline bool isOrbiting() const { return orbiting_; };

    virtual void move(const float dt);
    void orbit(const float dt);

protected:
    Orbitable *barycenter_;
    int orbiting_radius_;
    bool orbiting_;
};

#endif
