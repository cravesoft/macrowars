#ifndef __BODY_HPP
#define __BODY_HPP

#include <OgreVector3.h>
#include <OgreColourValue.h>

#define RADIUS_STEP_SIZE 10

class Ship;

// Body
class Body
{
public:
    Body(void) {};
    virtual ~Body(void) {};

    //virtual void draw(OBoy::Graphics *g, const Ogre::Vector3 camera) = 0;
    virtual bool isTouched(const Ogre::Vector3 position) const;
    virtual void handleShipCollision(Ship* ship) = 0;

    virtual Ogre::Vector3 getPosition() const { return pos_; }
    virtual void setPosition(const Ogre::Vector3 &position) { pos_ = position; }

    virtual int getRadius() const { return radius_; }
    virtual void setRadius(int radius) { radius_ = radius; };

    virtual Ogre::ColourValue getColor() const { return color_; }
    virtual void setColor(const Ogre::ColourValue &color) { color_ = color; };

protected:
    int radius_;
    Ogre::ColourValue color_;
    Ogre::Vector3 pos_;
};

#endif
