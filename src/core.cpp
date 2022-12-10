// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "core.hpp"

#define CORE_DEFAULT_SPEED 5
#define CORE_DEFAULT_RADIUS 36

Core::Core(Ogre::SceneManager *scene_manager, int radius,
           const Ogre::Vector3 &pos, int speed)
    : Planet(scene_manager, radius, pos, speed)
{
    this->setRadius(CORE_DEFAULT_RADIUS);
    speed_ = CORE_DEFAULT_SPEED;
#if TODO
    image_ = OBoy::Environment::getImage("IMAGE_CORE");
#endif
}
