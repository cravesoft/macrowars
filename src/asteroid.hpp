#ifndef __ASTEROID_HPP__
#define __ASTEROID_HPP__

#include <OgreImage.h>

#include "body.hpp"
#include "ship.hpp"

class Asteroid : public Body
{
public:
    Asteroid(Ogre::SceneManager *scene_manager, int radius, const Ogre::Vector3 &pos);
    virtual ~Asteroid(void);

#if TODO
    virtual void draw(OBoy::Graphics *g, const Ogre::Vector2 camera);
#endif
    virtual void handleShipCollision(Ship* ship);
    void update();

private:
#if TODO
    void _draw(OBoy::Graphics *g, Ogre::Vector2 &pos);
#endif

    static const std::string filename_;
    Ogre::Image image_;
    Ogre::SceneNode *scene_node_;
    Ogre::Entity *entity_;
    Ogre::SceneManager *scene_manager_;
};

#endif
