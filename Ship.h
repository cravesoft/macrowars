// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include <vector>

#include "OBoy/Environment.h"
#include "OBoyLib/OBoyUtil.h"
#include "OBoyLib/Vector2.h"
#include "OBoy/LineStrip.h"

#include "Body.h"
#include "Traveller.h"
#include "Satellite.h"

class Player;

typedef struct {
  bool	active; // active (yes/no)
  OBoyLib::Color 	life; // particle life
  char fade; // fade speed
  OBoyLib::Color	  color; // color
  OBoyLib::Vector2 pos; // position
  OBoyLib::Vector2	dir; // direction
#if 0
  OBoy::LineStrip *shape;
#endif
} Particle; // particles Structure

class Ship : public Traveller, public Satellite
{
public:

  Ship(OriginAndDestination *origin, OriginAndDestination *destination,
       const Player* player, OBoyLib::Vector2 position);
  virtual ~Ship(void);

  // draw the ship
  virtual void draw(OBoy::Graphics *g, const OBoyLib::Vector2 camera);

  // update the ship
  void update(const float dt);

  // handle collision between two ships
  virtual void handleShipCollision(Ship* ship);

  // move the ship to its destination or orbit around a body
  virtual void move(const float speed);
  void moveAsTraveller(const float speed);
  void moveAsSatellite(const float speed);

  // get and set the ship state
  inline int getShipState() const { return mShipState; };
  inline void setShipState(const int shipState) { mShipState = shipState; };

  // get and set the ship state
  inline const Player* getShipOwner() const { return mShipOwner; };

private:

  void _draw(OBoy::Graphics *g, OBoyLib::Vector2 &pos);

public:

  enum ShipState {
    SHIP_DEAD,
    SHIP_REMOVE,
    SHIP_ALIVE
  };

private:

  typedef void (Ship::*moveFunc)(const float);
  ::std::vector<moveFunc> mMoveFuncs;
  int mShipState;
  int mNumParticles;
  Particle* mParticles;
  const Player* mShipOwner;
  OBoy::LineStrip *mTriangle;
};