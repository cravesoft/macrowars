// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __TRAVELLER_HPP__
#define __TRAVELLER_HPP__

#include "movable.hpp"

class OriginAndDestination;

class Traveller : virtual public Movable
{
public:
    Traveller(void);
    Traveller(OriginAndDestination *origin, OriginAndDestination *destination);
    virtual ~Traveller(void);

    virtual void move(const float speed);
    void travelToDestination(const float speed);
    bool hasArrivedAtDestination() const;
    void setOriginAndDestination(OriginAndDestination* origin, OriginAndDestination* destination);

    inline OriginAndDestination* getDestination() const { return destination_; };
    void setDestination(OriginAndDestination *destination);

    inline OriginAndDestination* getOrigin() const { return origin_; };
    void setOrigin(OriginAndDestination *origin);

protected:
    OriginAndDestination *destination_;
    OriginAndDestination *origin_;
};

#endif
