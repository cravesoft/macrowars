// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __MOVABLE_HPP__
#define __MOVABLE_HPP__

#include "body.hpp"

class Movable : public Body
{
public:
    Movable(void);
    virtual ~Movable(void) {};

    virtual void move(const float speed) = 0;

protected:
    float speed_;
    float angle_;
    Ogre::Vector3 accel_;
    Ogre::Vector3 velocity_;
};

#endif
