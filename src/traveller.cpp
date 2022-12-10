// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <stdio.h>

#include "traveller.hpp"
#include "origin_and_destination.hpp"

Traveller::Traveller(void) : Movable()
{
    origin_ = NULL;
    destination_ = NULL;
}

Traveller::Traveller(OriginAndDestination *origin, OriginAndDestination *destination)
    : Movable()
    , origin_(origin)
    , destination_(destination)
{
    origin_->addLeavingTraveller(this);
    destination_->addArrivingTraveller(this);
}

Traveller::~Traveller(void)
{
    if(origin_ != NULL)
    {
        origin_->removeLeavingTraveller(this);
    }
    if(destination_ != NULL)
    {
        destination_->removeArrivingTraveller(this);
    }
}

void Traveller::move(float speed)
{
    travelToDestination(speed);
}

void Traveller::travelToDestination(float speed)
{
    Ogre::Vector3 dir = destination_->getPosition() - pos_;
    accel_ = dir.normalise();
    velocity_ = velocity_*(1.0f-speed) + accel_*speed;
    angle_ = Ogre::Math::RadiansToDegrees(atan2(velocity_.y, velocity_.x));
    pos_ += velocity_*speed*40;
}

bool Traveller::hasArrivedAtDestination() const
{
    if((pos_ - destination_->getPosition()).length() < destination_->getRadius())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Traveller::setOriginAndDestination(OriginAndDestination* origin,
                                        OriginAndDestination* destination)
{
    if(origin_ != NULL)
    {
        origin_->removeLeavingTraveller(this);
    }
    if(destination_ != NULL)
    {
        destination_->removeArrivingTraveller(this);
    }
    origin_ = origin;
    destination_ = destination;
    origin_->addLeavingTraveller(this);
    destination_->addArrivingTraveller(this);
}

void Traveller::setDestination(OriginAndDestination *destination)
{
    if(destination_ != NULL)
    {
        destination_->removeArrivingTraveller(this);
    }
    destination_ = destination;
    destination_->addArrivingTraveller(this);
}

void Traveller::setOrigin(OriginAndDestination *origin)
{
    if(origin_ != NULL)
    {
        origin_->removeLeavingTraveller(this);
    }
    origin_ = origin;
    origin_->addLeavingTraveller(this);
}
