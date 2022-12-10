#include "body.hpp"

bool Body::isTouched(const Ogre::Vector3 pos) const
{
    if((pos_ - pos).length() < radius_)
    {
        return true;
    }
    else
    {
        return false;
    }
}
