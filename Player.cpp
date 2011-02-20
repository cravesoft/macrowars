// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <algorithm>

#include "Player.h"

Player::Player(void)
{
  mPlayerId = COMPUTER;
  mPlayerStrategy = INTERMEDIATE;
}

Player::~Player(void)
{
  // delete all waypoints:
  while (mWaypoints.size()>0)
	{
		delete *(mWaypoints.begin());
		mWaypoints.erase(mWaypoints.begin());
	}
}

void Player::drawWaypoints(oboy::Graphics *g, const oboylib::Vector2 camera)
{
  // draw all waypoints:
  g->setDrawMode(oboy::Graphics::DRAWMODE_NORMAL);
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    mWaypoints[i]->draw(g, camera);
    // draw the probas of the selected parent:
    if (mWaypoints[i]->isSelectedAsParent())
      mWaypoints[i]->drawProbas(g, camera);
  }
  g->setDrawMode(oboy::Graphics::DRAWMODE_NORMAL);
}

bool Player::createWaypoint(oboylib::Vector2 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships)
{
  // check if there is already a waypoint at the mouse position:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    // if there is already a waypoint at the mouse position, make it the parent node of the new waypoint and copy its parameters (position and radius):
    if (mWaypoints[i]->isTouched(position))
    {
      Waypoint* waypoint = new Waypoint();
      waypoint->setSelected(true);
      waypoint->setPosition(mWaypoints[i]->getPosition());
      waypoint->addParent(mWaypoints[i]);
      waypoint->setRadius(mWaypoints[i]->getRadius());
      mWaypoints.push_back(waypoint);
      // add the new waypoint in the list of siblings of its parent and release the ships:
      mWaypoints[i]->addSibling(waypoint);

      std::vector<Satellite*> satellites = mWaypoints[i]->getSatellites();
      for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));

      // if the parent sits on top of a planet that belongs to the player and if it previously did not have any sibling, release the ships from the planet:
      if (mWaypoints[i]->getSiblings().size() == 1 && mWaypoints[i]->getPlanet() != NULL && mWaypoints[i]->getPlanet()->getColor() == mColor)
      {
        mWaypoints[i]->getPlanet()->releaseShips(ships);
      }
      return true;
    }
  }
  // check if there is a planet at the mouse position:
  for (int i=0 ; i<(int)planets.size() ; i++)
  {
    // if there is a planet at the mouse position, create a waypoint at the planet position and attach a sibling to it:
    if (planets[i]->isTouched(position))
    {
      // create the parent waypoint:
      Waypoint* waypoint = new Waypoint();
      waypoint->setPosition(planets[i]->getPosition());
      waypoint->setRadius(planets[i]->getRadius() + RADIUS_STEP_SIZE);
      waypoint->setPlanet(planets[i]);
      planets[i]->setWaypoint(waypoint, this);
      mWaypoints.push_back(waypoint);

      // create the sibling waypoint:
      waypoint = new Waypoint();
      // make the sibling the selected waypoint
      waypoint->setSelected(true);
      waypoint->setPosition(planets[i]->getPosition());
      waypoint->addParent(mWaypoints.back());
      mWaypoints.back()->addSibling(waypoint);
      mWaypoints.push_back(waypoint);

      // if the planet belongs to the player, release the ships
      if (planets[i]->getColor() == mColor)
      {
        planets[i]->releaseShips(ships);
      }

      return true;
    }
  }

  // if there is no waypoints, nor planets at the mouse position, create a waypoint at the mouse position and attach a sibling to it:
  Waypoint* waypoint = new Waypoint();
  waypoint->setPosition(position);
  mWaypoints.push_back(waypoint);

  waypoint = new Waypoint();
  // make the sibling the selected waypoint
  waypoint->setSelected(true);
  waypoint->setPosition(position);
  waypoint->addParent(mWaypoints.back());
  mWaypoints.back()->addSibling(waypoint);
  waypoint->setDrawRing(false);
  mWaypoints.push_back(waypoint);

  return true;
}

bool Player::pickWaypoint(oboylib::Vector2 position)
{
  // check if there is a waypoint or an arrow at the mouse position:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    // if there is a waypoint at the mouse position, make it the selected waypoint:
    if (mWaypoints[i]->isTouched(position))
    {
      mWaypoints[i]->setSelected(true);
      // if the waypoint sits on a planet, remove the link between the planet and the waypoint
      if (mWaypoints[i]->getPlanet() != NULL)
      {
        mWaypoints[i]->getPlanet()->setWaypoint(NULL, this);
        mWaypoints[i]->setPlanet(NULL);
      }
      return true;
    }
    Waypoint *parent;
    // if an arrow is touched, create a new waypoint and make it the selected waypoint
    if (mWaypoints[i]->isArrowTouched(position, parent))
    {
      int proba = parent->getProbaArrow(mWaypoints[i]);
      parent->removeSibling(mWaypoints[i]);
      mWaypoints[i]->removeParent(parent);

      Waypoint *waypoint = new Waypoint();
      waypoint->setSelected(true);
      waypoint->setPosition(position);
      waypoint->addParent(parent);
      parent->addSibling(waypoint, proba);
      mWaypoints.push_back(waypoint);
      std::vector<Traveller*> arrivingTravellers = mWaypoints[i]->getArrivingTravellers();
      for (int j=0 ; j<(int)arrivingTravellers.size() ; j++)
      {
        if (parent == arrivingTravellers[j]->getOrigin())
          arrivingTravellers[j]->setDestination(waypoint);
      }
      std::vector<Traveller*> leavingTravellers = mWaypoints[i]->getLeavingTravellers();
      for (int j=0 ; j<(int)leavingTravellers.size() ; j++)
      {
        if (parent == leavingTravellers[j]->getDestination())
          leavingTravellers[j]->setOrigin(waypoint);
      }
      // release the orbiting ships
      std::vector<Satellite*> satellites = mWaypoints[i]->getSatellites();
      for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));

      cleanWaypoints();
      return true;
    }
  }
  return false;
}

bool Player::moveWaypoint(oboylib::Vector2 position, std::vector<Planet*> &planets)
{
  // find the selected waypoint:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    // if a waypoint is selected:
    if (mWaypoints[i]->isSelected())
    {
      mWaypoints[i]->setDrawRing(true);
      // if the selected waypoint touches another waypoint, overlap the two waypoints
      for (int j=0 ; j<(int)mWaypoints.size() ; j++)
      {
        if (mWaypoints[j]->isTouched(position) && mWaypoints[j] != mWaypoints[i])
        {
          mWaypoints[i]->setPosition(mWaypoints[j]->getPosition());
          mWaypoints[i]->setRadius(mWaypoints[j]->getRadius());
          return true;
        }
      }
      for (int j=0 ; j<(int)planets.size() ; j++)
      {
        // if the selected waypoint touches a planet, move it to the same position and adapt its radius
        if (planets[j]->isTouched(position))
        {
          mWaypoints[i]->setPosition(planets[j]->getPosition());
          mWaypoints[i]->setRadius(planets[j]->getRadius() + RADIUS_STEP_SIZE);
          return true;
        }
      }
      // move the waypoint to the mouse position and set its radius to the default value
      mWaypoints[i]->setDrawRing(false);
      mWaypoints[i]->setPosition(position);
      mWaypoints[i]->setDefaultRadius();
      return true;
    }
  }
  return false;
}

void Player::deleteWaypoint(oboylib::Vector2 position)
{
  // find a waypoint or an arrow at the mouse position:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    // if a waypoint is found, delete it:
    if (mWaypoints[i]->isTouched(position))
    {
      // remove all links between the waypoint and its parents and siblings:
      std::vector<Waypoint*> parents = mWaypoints[i]->getParents();
      for (int j=0 ; j<(int)parents.size() ; j++)
        parents[j]->removeSibling(mWaypoints[i]);
      std::vector<Waypoint*> siblings = mWaypoints[i]->getSiblings();
      for (int j=0 ; j<(int)siblings.size() ; j++)
        siblings[j]->removeParent(mWaypoints[i]);
      mWaypoints[i]->removeAllParents();
      mWaypoints[i]->removeAllSiblings();
      break;
    }
    Waypoint *parent;
    // if an arrow is found, remove only the link between the sibling and its parent:
    if (mWaypoints[i]->isArrowTouched(position, parent))
    {
      parent->removeSibling(mWaypoints[i]);
      mWaypoints[i]->removeParent(parent);
      break;
    }
  }
  cleanWaypoints();
}

void Player::releaseWaypoint(oboylib::Vector2 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships)
{
  // find the selected waypoint:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    if (mWaypoints[i]->isSelected())
    {
      // release the selected waypoint
      mWaypoints[i]->setSelected(false);
      mWaypoints[i]->setDefaultRadius();
      // find another waypoint at the same position:
      for (int j=0 ; j<(int)mWaypoints.size() ; j++)
	    {
        // if the selected waypoint sits on top of another waypoint:
        if (i != j && mWaypoints[j]->isTouched(position))
        {
          // transfer all links and ships from the selected waypoint to the other waypoint
          mWaypoints[j]->mergeWaypoint(mWaypoints[i]);
          // if the selected waypoint has a sibling and if the other waypoint sits on a planet and if the planet belongs to the player:
          if (mWaypoints[j]->getSiblings().size() > 0 && mWaypoints[j]->getPlanet() != NULL && mWaypoints[j]->getPlanet()->getColor() == mColor)
          {
            // release the ships
            mWaypoints[j]->getPlanet()->releaseShips(ships);
          }
          std::vector<Satellite*> satellites = mWaypoints[j]->getSatellites();
          for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));
          cleanWaypoints();
          return;
        }
      }
      // find a planet at the same position:
      for (int j=0 ; j<(int)planets.size() ; j++)
      {
        // if a planet is found, attach the selected waypoint to it and adapt its radius
        if (planets[j]->isTouched(position))
        {
          position = planets[j]->getPosition();
          mWaypoints[i]->setRadius(planets[j]->getRadius() + RADIUS_STEP_SIZE);
          mWaypoints[i]->setPlanet(planets[j]);
          planets[j]->setWaypoint(mWaypoints[i], this);
          // if the selected waypoint has a sibling and if the planet belongs to the player:
          if (mWaypoints[i]->getSiblings().size() > 0 && planets[j]->getColor() == mColor)
          {
            // release the ships
            planets[j]->releaseShips(ships);
          }
          //mWaypoints[i]->releaseAllSatellites();
          std::vector<Satellite*> satellites = mWaypoints[i]->getSatellites();
          for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));
          break;
        }
      }
      mWaypoints[i]->setPosition(position);
      mWaypoints[i]->setDrawRing(true);
      return;
    }
  }
}

void Player::cleanWaypoints()
{
  // search for waypoints to delete:
  for (int i=(int)mWaypoints.size()-1 ; i>=0 ; i--)
	{
    // if a waypoint has no sibling and no parent:
    if (mWaypoints[i]->getParents().size() == 0 && mWaypoints[i]->getSiblings().size() == 0)
    { 
      std::vector<Traveller*> arrivingTravellers = mWaypoints[i]->getArrivingTravellers();
      // give a new origin to all ships that were going to this waypoint:
      for (int j=0 ; j<(int)arrivingTravellers.size() ; j++)
      {
        OriginAndDestination* origin = arrivingTravellers[j]->getOrigin();
        // if the origin still exists:
        if (origin != mWaypoints[i] && origin != NULL)
          // send back the ship to its origin
          arrivingTravellers[j]->setDestination(origin);
        else
          // otherwise, send back the ship to the player core
          arrivingTravellers[j]->setDestination(mPlayerCore);
      }

      // set the player core as the new origin of the leaving ships:
      std::vector<Traveller*> leavingTravellers = mWaypoints[i]->getLeavingTravellers();
      for (int j=0 ; j<(int)leavingTravellers.size() ; j++)
      {
        leavingTravellers[j]->setOrigin(mPlayerCore);
      }

      // release the ships that are orbiting around the waypoint
      std::vector<Satellite*> satellites = mWaypoints[i]->getSatellites();
      for_each(satellites.begin(), satellites.end(), std::mem_fun(&Satellite::stopOrbiting));

      // if the waypoint sits on a planet, break the link between the two objects
      if (mWaypoints[i]->getPlanet() != NULL)
        mWaypoints[i]->getPlanet()->setWaypoint(NULL, this);

      // finally, delete the waypoint and remove it from the list:
      delete *(mWaypoints.begin()+i);
			mWaypoints.erase(mWaypoints.begin()+i);
		}
	}
}

bool Player::isArrowTouched(oboylib::Vector2 position)
{
  // find a waypoint which arrow is touched by the mouse:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    Waypoint *parent;
    // if an arrow is touched:
    if (mWaypoints[i]->isArrowTouched(position, parent))
    {
      // set the parent and the sibling as selected:
      parent->setSelectedAsParent(true);
      mWaypoints[i]->setSelected(true);
      // save the current mouse position
      mOriginalMousePos = position;
      return true;
    }
  }
  return false;
}

bool Player::isRingTouched(oboylib::Vector2 position)
{
  // find a waypoint which ring is touched by the mouse:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    // if an arrow is touched:
    if (mWaypoints[i]->isRingTouched(position))
    {
      // set the waypoint as selected as a parent waypoint:
      mWaypoints[i]->setSelectedAsParent(true);
      // save the current mouse position
      mOriginalMousePos = position;
      return true;
    }
  }
  return false;
}

bool Player::updateProbaRing(oboylib::Vector2 position)
{
  // find the selected ring:
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    if (mWaypoints[i]->isSelectedAsParent())
    {
      // update proba at the ring
      mWaypoints[i]->changeProbaRing(static_cast<int>((mOriginalMousePos - position).y()*3));
      // save the new mouse position
      mOriginalMousePos = position;
      return true;
    }
  }
  return false;
}

bool Player::updateProbaArrow(oboylib::Vector2 position)
{
  int i, j;
  // find the selected link (between a parent and one of its sibling)
  for (i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    if (mWaypoints[i]->isSelectedAsParent())
    {
      break;
    }
  }
  std::vector<Waypoint*> siblings = mWaypoints[i]->getSiblings();
  for (j=0 ; j<(int)siblings.size() ; j++)
  {
    if (siblings[j]->isSelected())
    {
      break;
    }
  }
  // update proba between the parent and its sibling
  mWaypoints[i]->changeProbaArrow(static_cast<int>((mOriginalMousePos - position).y()*3), siblings[j]);
  // save the new mouse position
  mOriginalMousePos = position;
  return true;
}

void Player::releaseArrow()
{
  // release the parent and its sibling
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    if (mWaypoints[i]->isSelectedAsParent())
    {
      mWaypoints[i]->setSelectedAsParent(false);
      std::vector<Waypoint*> siblings = mWaypoints[i]->getSiblings();
      for (int j=0 ; j<(int)siblings.size() ; j++)
      {
        if (siblings[j]->isSelected())
          siblings[j]->setSelected(false);
      }
      break;
    }
  }
}

void Player::releaseRing()
{
  // release the waypoint
  for (int i=0 ; i<(int)mWaypoints.size() ; i++)
  {
    if (mWaypoints[i]->isSelectedAsParent())
    {
      mWaypoints[i]->setSelectedAsParent(false);
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