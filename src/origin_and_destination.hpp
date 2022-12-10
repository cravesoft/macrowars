// Copyright (c) 2010 Olivier Crave. All rights reserved.

#ifndef __ORIGIN_AND_DESTINATION_HPP__
#define __ORIGIN_AND_DESTINATION_HPP__

#include <vector>

#include "body.hpp"

class Traveller;

// OriginAndDestination
class OriginAndDestination : virtual public Body
{
public:
    void addArrivingTraveller(Traveller* traveller);
    void addLeavingTraveller(Traveller* traveller);

    void removeLeavingTraveller(Traveller* traveller);
    void removeArrivingTraveller(Traveller* traveller);

    inline std::vector<Traveller*>& getArrivingTravellers() { return arriving_travellers_; };
    inline std::vector<Traveller*>& getLeavingTravellers() { return leaving_travellers_; };

private:
    std::vector<Traveller*> arriving_travellers_;
    std::vector<Traveller*> leaving_travellers_;
};

#endif
