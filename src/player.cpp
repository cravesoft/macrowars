#include <algorithm>

#include "player.hpp"

Player::Player(Ogre::SceneManager *scene_manager)
    : scene_manager_(scene_manager)
    , player_id_(COMPUTER)
    , player_strategy_(INTERMEDIATE)
{
}

Player::~Player(void)
{
    // delete all waypoints:
    while(waypoints_.size() > 0)
    {
        delete *(waypoints_.begin());
        waypoints_.erase(waypoints_.begin());
    }
}

#if TODO
void Player::drawWaypoints(OBoy::Graphics *g, const Ogre::Vector3 camera)
{
    // draw all waypoints:
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        waypoints_[i]->draw(g, camera);
        // draw the probas of the selected parent:
        if(waypoints_[i]->isSelectedAsParent())
        {
            waypoints_[i]->drawProbas(g, camera);
        }
    }
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}
#endif

bool Player::createWaypoint(Ogre::Vector3 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships)
{
    // check if there is already a waypoint at the mouse position:
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        if(!waypoints_[i]->isTouched(position))
        {
            continue;
        }

        // if there is already a waypoint at the mouse position, make it the
        // parent node of the new waypoint and copy its parameters (position
        // and radius):
        Waypoint* waypoint = new Waypoint(scene_manager_);
        waypoint->setSelected(true);
        waypoint->setPosition(waypoints_[i]->getPosition());
        waypoint->addParent(waypoints_[i]);
        waypoint->setRadius(waypoints_[i]->getRadius());
        waypoints_.push_back(waypoint);
        // add the new waypoint in the list of siblings of its parent and release the ships:
        waypoints_[i]->addSibling(waypoint);

        std::vector<Satellite*> satellites = waypoints_[i]->getSatellites();
        for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));

        // if the parent sits on top of a planet that belongs to the player
        // and if it previously did not have any sibling, release the ships
        // from the planet:
        if(waypoints_[i]->getSiblings().size() == 1 &&
           waypoints_[i]->getPlanet() != NULL &&
           waypoints_[i]->getPlanet()->getColor() == color_)
        {
            waypoints_[i]->getPlanet()->releaseShips(ships);
        }
        return true;
    }
    // check if there is a planet at the mouse position:
    for(int i = 0; i < planets.size(); i++)
    {
        if(!planets[i]->isTouched(position))
        {
            continue;
        }

        // if there is a planet at the mouse position, create a waypoint at the
        // planet position and attach a sibling to it:
        // create the parent waypoint:
        Waypoint* waypoint = new Waypoint(scene_manager_);
        waypoint->setPosition(planets[i]->getPosition());
        waypoint->setRadius(planets[i]->getRadius() + RADIUS_STEP_SIZE);
        waypoint->setPlanet(planets[i]);
        planets[i]->setWaypoint(waypoint, this);
        waypoints_.push_back(waypoint);

        // create the sibling waypoint:
        waypoint = new Waypoint(scene_manager_);
        // make the sibling the selected waypoint
        waypoint->setSelected(true);
        waypoint->setPosition(planets[i]->getPosition());
        waypoint->addParent(waypoints_.back());
        waypoints_.back()->addSibling(waypoint);
        waypoints_.push_back(waypoint);

        // if the planet belongs to the player, release the ships
        if(planets[i]->getColor() == color_)
        {
            planets[i]->releaseShips(ships);
        }

        return true;
    }

    // if there is no waypoints, nor planets at the mouse position, create a
    // waypoint at the mouse position and attach a sibling to it:
    Waypoint* waypoint = new Waypoint(scene_manager_);
    waypoint->setPosition(position);
    waypoints_.push_back(waypoint);

    waypoint = new Waypoint(scene_manager_);
    // make the sibling the selected waypoint
    waypoint->setSelected(true);
    waypoint->setPosition(position);
    waypoint->addParent(waypoints_.back());
    waypoints_.back()->addSibling(waypoint);
    waypoint->setDrawRing(false);
    waypoints_.push_back(waypoint);

    return true;
}

bool Player::pickWaypoint(Ogre::Vector3 position)
{
    // check if there is a waypoint or an arrow at the mouse position:
    for(int i = 0; i < waypoints_.size(); i++)
    {
        // if there is a waypoint at the mouse position, make it the selected
        // waypoint:
        if(waypoints_[i]->isTouched(position))
        {
            waypoints_[i]->setSelected(true);
            // if the waypoint sits on a planet, remove the link between the
            // planet and the waypoint
            if(waypoints_[i]->getPlanet() != NULL)
            {
                waypoints_[i]->getPlanet()->setWaypoint(NULL, this);
                waypoints_[i]->setPlanet(NULL);
            }
            return true;
        }
        Waypoint *parent;
        // if an arrow is touched, create a new waypoint and make it the
        // selected waypoint
        if(waypoints_[i]->isArrowTouched(position, parent))
        {
            int proba = parent->getProbaArrow(waypoints_[i]);
            parent->removeSibling(waypoints_[i]);
            waypoints_[i]->removeParent(parent);

            Waypoint *waypoint = new Waypoint(scene_manager_);
            waypoint->setSelected(true);
            waypoint->setPosition(position);
            waypoint->addParent(parent);
            parent->addSibling(waypoint, proba);
            waypoints_.push_back(waypoint);
            std::vector<Traveller*> arrivingTravellers = waypoints_[i]->getArrivingTravellers();
            for(int j = 0 ; j < arrivingTravellers.size(); j++)
            {
                if(parent == arrivingTravellers[j]->getOrigin())
                {
                    arrivingTravellers[j]->setDestination(waypoint);
                }
            }
            std::vector<Traveller*> leavingTravellers = waypoints_[i]->getLeavingTravellers();
            for(int j = 0 ; j < leavingTravellers.size(); j++)
            {
                if(parent == leavingTravellers[j]->getDestination())
                {
                    leavingTravellers[j]->setOrigin(waypoint);
                }
            }
            // release the orbiting ships
            std::vector<Satellite*> satellites = waypoints_[i]->getSatellites();
            for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));

            cleanWaypoints();
            return true;
        }
    }
    return false;
}

bool Player::moveWaypoint(Ogre::Vector3 position, std::vector<Planet*> &planets)
{
    // find the selected waypoint:
    for(int i = 0; i < waypoints_.size(); i++)
    {
        // if a waypoint is selected:
        if(waypoints_[i]->isSelected())
        {
            waypoints_[i]->setDrawRing(true);
            // if the selected waypoint touches another waypoint, overlap the
            // two waypoints
            for(int j = 0 ; j < waypoints_.size(); j++)
            {
                if(waypoints_[j]->isTouched(position) &&
                   waypoints_[j] != waypoints_[i])
                {
                    waypoints_[i]->setPosition(waypoints_[j]->getPosition());
                    waypoints_[i]->setRadius(waypoints_[j]->getRadius());
                    return true;
                }
            }
            for(int j = 0 ; j < planets.size(); j++)
            {
                // if the selected waypoint touches a planet, move it to the
                // same position and adapt its radius
                if(planets[j]->isTouched(position))
                {
                    waypoints_[i]->setPosition(planets[j]->getPosition());
                    waypoints_[i]->setRadius(planets[j]->getRadius() + RADIUS_STEP_SIZE);
                    return true;
                }
            }
            // move the waypoint to the mouse position and set its radius to
            // the default value
            waypoints_[i]->setDrawRing(false);
            waypoints_[i]->setPosition(position);
            waypoints_[i]->setDefaultRadius();
            return true;
        }
    }
    return false;
}

void Player::deleteWaypoint(Ogre::Vector3 position)
{
    // find a waypoint or an arrow at the mouse position:
    for(int i = 0; i < waypoints_.size(); i++)
    {
        // if a waypoint is found, delete it:
        if(waypoints_[i]->isTouched(position))
        {
            // remove all links between the waypoint and its parents and siblings:
            std::vector<Waypoint*> parents = waypoints_[i]->getParents();
            for(int j = 0 ; j < parents.size(); j++)
            {
                parents[j]->removeSibling(waypoints_[i]);
            }
            std::vector<Waypoint*> siblings = waypoints_[i]->getSiblings();
            for(int j = 0 ; j < siblings.size(); j++)
            {
                siblings[j]->removeParent(waypoints_[i]);
            }
            waypoints_[i]->removeAllParents();
            waypoints_[i]->removeAllSiblings();
            break;
        }
        Waypoint *parent;
        // if an arrow is found, remove only the link between the sibling and its parent:
        if(waypoints_[i]->isArrowTouched(position, parent))
        {
            parent->removeSibling(waypoints_[i]);
            waypoints_[i]->removeParent(parent);
            break;
        }
    }
    cleanWaypoints();
}

void Player::releaseWaypoint(Ogre::Vector3 position,
                             std::vector<Planet*> &planets,
                             std::vector<Ship*>& ships)
{
    // find the selected waypoint:
    for(int i = 0; i < waypoints_.size(); i++)
    {
        if(!waypoints_[i]->isSelected())
        {
            continue;
        }
        // release the selected waypoint
        waypoints_[i]->setSelected(false);
        waypoints_[i]->setDefaultRadius();
        // find another waypoint at the same position:
        for(int j = 0; j < waypoints_.size(); j++)
        {
            // if the selected waypoint sits on top of another waypoint:
            if(i != j && waypoints_[j]->isTouched(position))
            {
                // transfer all links and ships from the selected waypoint to the other waypoint
                waypoints_[j]->mergeWaypoint(waypoints_[i]);
                // if the selected waypoint has a sibling and if the other waypoint sits on a planet and if the planet belongs to the player:
                if(waypoints_[j]->getSiblings().size() > 0 &&
                   waypoints_[j]->getPlanet() != NULL &&
                   waypoints_[j]->getPlanet()->getColor() == color_)
                {
                    // release the ships
                    waypoints_[j]->getPlanet()->releaseShips(ships);
                }
                std::vector<Satellite*> satellites = waypoints_[j]->getSatellites();
                for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));
                cleanWaypoints();
                return;
            }
        }
        // find a planet at the same position:
        for(int j = 0; j < planets.size(); j++)
        {
            // if a planet is found, attach the selected waypoint to it and
            // adapt its radius
            if(planets[j]->isTouched(position))
            {
                position = planets[j]->getPosition();
                waypoints_[i]->setRadius(planets[j]->getRadius() + RADIUS_STEP_SIZE);
                waypoints_[i]->setPlanet(planets[j]);
                planets[j]->setWaypoint(waypoints_[i], this);
                // if the selected waypoint has a sibling and if the planet
                // belongs to the player:
                if(waypoints_[i]->getSiblings().size() > 0 &&
                   planets[j]->getColor() == color_)
                {
                    // release the ships
                    planets[j]->releaseShips(ships);
                }
                //waypoints_[i]->releaseAllSatellites();
                std::vector<Satellite*> satellites = waypoints_[i]->getSatellites();
                for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));
                break;
            }
        }
        waypoints_[i]->setPosition(position);
        waypoints_[i]->setDrawRing(true);
        return;
    }
}

void Player::cleanWaypoints()
{
    // search for waypoints to delete:
    for(int i = waypoints_.size() - 1; i >= 0; i--)
    {
        // if a waypoint has no sibling and no parent:
        if(waypoints_[i]->getParents().size() == 0 &&
           waypoints_[i]->getSiblings().size() == 0)
        {
            std::vector<Traveller*> arrivingTravellers = waypoints_[i]->getArrivingTravellers();
            // give a new origin to all ships that were going to this waypoint:
            for(int j = 0 ; j < arrivingTravellers.size(); j++)
            {
                OriginAndDestination* origin = arrivingTravellers[j]->getOrigin();
                // if the origin still exists:
                if(origin != waypoints_[i] && origin != NULL)
                {
                    // send back the ship to its origin
                    arrivingTravellers[j]->setDestination(origin);
                }
                else
                {
                    // otherwise, send back the ship to the player core
                    arrivingTravellers[j]->setDestination(player_core_);
                }
            }

            // set the player core as the new origin of the leaving ships:
            std::vector<Traveller*> leavingTravellers = waypoints_[i]->getLeavingTravellers();
            for(int j = 0 ; j < leavingTravellers.size(); j++)
            {
                leavingTravellers[j]->setOrigin(player_core_);
            }

            // release the ships that are orbiting around the waypoint
            std::vector<Satellite*> satellites = waypoints_[i]->getSatellites();
            for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));

            // if the waypoint sits on a planet, break the link between the two objects
            if(waypoints_[i]->getPlanet() != NULL)
            {
                waypoints_[i]->getPlanet()->setWaypoint(NULL, this);
            }

            // finally, delete the waypoint and remove it from the list:
            delete *(waypoints_.begin() + i);
            waypoints_.erase(waypoints_.begin() + i);
        }
    }
}

bool Player::isArrowTouched(Ogre::Vector3 position)
{
    // find a waypoint which arrow is touched by the mouse:
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        Waypoint *parent;
        // if an arrow is touched:
        if(waypoints_[i]->isArrowTouched(position, parent))
        {
            // set the parent and the sibling as selected:
            parent->setSelectedAsParent(true);
            waypoints_[i]->setSelected(true);
            // save the current mouse position
            original_mouse_pos_ = position;
            return true;
        }
    }
    return false;
}

bool Player::isRingTouched(Ogre::Vector3 position)
{
    // find a waypoint which ring is touched by the mouse:
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        // if an arrow is touched:
        if(waypoints_[i]->isRingTouched(position))
        {
            // set the waypoint as selected as a parent waypoint:
            waypoints_[i]->setSelectedAsParent(true);
            // save the current mouse position
            original_mouse_pos_ = position;
            return true;
        }
    }
    return false;
}

bool Player::updateProbaRing(Ogre::Vector3 position)
{
    // find the selected ring:
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        if(waypoints_[i]->isSelectedAsParent())
        {
            // update proba at the ring
            waypoints_[i]->changeProbaRing((original_mouse_pos_ - position).y * 3);
            // save the new mouse position
            original_mouse_pos_ = position;
            return true;
        }
    }
    return false;
}

bool Player::updateProbaArrow(Ogre::Vector3 position)
{
    int i, j;
    // find the selected link (between a parent and one of its sibling)
    for(i = 0; i < waypoints_.size(); i++)
    {
        if(waypoints_[i]->isSelectedAsParent())
        {
            break;
        }
    }
    std::vector<Waypoint*> siblings = waypoints_[i]->getSiblings();
    for(j = 0 ; j < siblings.size(); j++)
    {
        if(siblings[j]->isSelected())
        {
            break;
        }
    }
    // update proba between the parent and its sibling
    waypoints_[i]->changeProbaArrow((original_mouse_pos_ - position).y * 3, siblings[j]);
    // save the new mouse position
    original_mouse_pos_ = position;
    return true;
}

void Player::releaseArrow()
{
    // release the parent and its sibling
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        if(waypoints_[i]->isSelectedAsParent())
        {
            waypoints_[i]->setSelectedAsParent(false);
            std::vector<Waypoint*> siblings = waypoints_[i]->getSiblings();
            for(int j = 0 ; j < siblings.size(); j++)
            {
                if(siblings[j]->isSelected())
                {
                    siblings[j]->setSelected(false);
                }
            }
            break;
        }
    }
}

void Player::releaseRing()
{
    // release the waypoint
    for(int i = 0 ; i < waypoints_.size(); i++)
    {
        if(waypoints_[i]->isSelectedAsParent())
        {
            waypoints_[i]->setSelectedAsParent(false);
            break;
        }
    }
}

void Player::updateStrategy()
{
}

void Player::update(float dt)
{
}
