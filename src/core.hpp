// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __CORE_HPP__
#define __CORE_HPP__

#include "planet.hpp"

class Core : public Planet
{
public:
    Core(Ogre::SceneManager *scene_manager, int radius, const Ogre::Vector3 &pos, int speed);
    virtual ~Core(void) {};
};

#endif
