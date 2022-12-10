// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <stdio.h>

#include "travel_strategy.hpp"

TravellerStrategy::TravellerStrategy(void)
{
    origin_ = NULL;
    destination_ = NULL;
}

TravellerStrategy::TravellerStrategy(OriginAndDestination *origin, OriginAndDestination *destination) : Movable()
{
    origin_ = origin;
    destination_ = destination;
    origin_->addLeavingTraveller(this);
    destination_->addArrivingTraveller(this);
}

TravellerStrategy::~TravellerStrategy(void)
{
    if (origin_ != NULL)
    {
        origin_->removeLeavingTraveller(this);
    }
    if (destination_ != NULL)
    {
        destination_->removeArrivingTraveller(this);
    }
}

void TravellerStrategy::move(Movable* movable, const float speed);
{
    BoyLib::Vector3 dir = destination_->getPosition() - movable->pos_;
    movable->accel_ = dir.normalize();
    movable->velocity_ = movable->velocity_*(1.0f-speed) + movable->accel_*speed;
    movable->angle_ = rad2deg(atan2(movable->velocity_.y, movable->velocity_.x));
    movable->pos_ += movable->velocity_*speed*40;
}

bool TravellerStrategy::hasArrivedAtDestination() const
{
    if ((pos_ - destination_->getPosition()).magnitude() < destination_->getRadius())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TravellerStrategy::setOriginAndDestination(OriginAndDestination* origin, OriginAndDestination* destination)
{
    if (origin_ != NULL)
    {
        origin_->removeLeavingTraveller(this);
    }
    if (destination_ != NULL)
    {
        destination_->removeArrivingTraveller(this);
    }
    origin_ = origin;
    destination_ = destination;
    origin_->addLeavingTraveller(this);
    destination_->addArrivingTraveller(this);
}

void TravellerStrategy::setDestination(OriginAndDestination *destination)
{
    if (destination_ != NULL)
    {
        destination_->removeArrivingTraveller(this);
    }
    destination_ = destination;
    destination_->addArrivingTraveller(this);
}

void TravellerStrategy::setOrigin(OriginAndDestination *origin)
{
    if (origin_ != NULL)
    {
        origin_->removeLeavingTraveller(this);
    }
    origin_ = origin;
    origin_->addLeavingTraveller(this);
}
