// Copyright (c) 2010 Olivier Crave. All rights reserved.
#include <OgreVector3.h>

#include <stdio.h>
#include <float.h>

#include "satellite.hpp"
#include "orbitable.hpp"

Satellite::Satellite(void) : Movable()
{
    barycenter_ = NULL;
    orbiting_ = false;
}

Satellite::~Satellite(void)
{
    if (barycenter_ != NULL)
    {
        barycenter_->releaseSatellite(this);
    }
}

void Satellite::startOrbiting(Orbitable *barycenter, int radius)
{
    orbiting_ = true;
    barycenter_ = barycenter;
    barycenter_->addSatellite(this);
    orbiting_radius_ = radius;
}

void Satellite::stopOrbiting()
{
    orbiting_ = false;
    assert (barycenter_ != NULL);
    barycenter_->releaseSatellite(this);
    barycenter_ = NULL;
}

void Satellite::move(float speed)
{
    orbit(speed);
}

void Satellite::orbit(float speed)
{
    Ogre::Vector3 dir = barycenter_->getPosition() - pos_;
    Ogre::Vector3 perpendicularAccel;
    Ogre::Vector3 parallelAccel;

    // set the perpendicular acceleration
    perpendicularAccel = dir.perpendicular().normalisedCopy();

    float distanceBetweenBarycenterAndSatellite = barycenter_->getPosition().distance(pos_);

    // set the parallel acceleration:
    if (distanceBetweenBarycenterAndSatellite > orbiting_radius_)
    {
        parallelAccel = dir.normalisedCopy();
    }
    else
    {
        parallelAccel = -dir.normalisedCopy();
    }

    float distanceBetweenSatelliteAndOrbit = orbiting_radius_ - distanceBetweenBarycenterAndSatellite;

    // set the satellite acceleration based on parallel and perpendicular accceleration:
    if (distanceBetweenSatelliteAndOrbit < 0)
    {
        float factor = -1.0f/distanceBetweenSatelliteAndOrbit;
        if (factor > 1)
        {
            factor = 1.0f;
        }
        accel_ = parallelAccel * (1.0f - factor) + perpendicularAccel * factor;
    }
    else
    {
        accel_ = perpendicularAccel;
    }

    velocity_ = velocity_*(1.0f-speed) + accel_*speed;
    angle_ = Ogre::Math::RadiansToDegrees(atan2(velocity_.y, velocity_.x));
    pos_ += velocity_*speed*40;
}
