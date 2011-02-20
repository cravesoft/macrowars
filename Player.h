// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include "Waypoint.h"
#include "Core.h"

enum PlayerId
{
  HUMAN,
  COMPUTER
};

enum PlayerStrategy
{
  DEFENSIVE,
  INTERMEDIATE,
  AGGRESSIVE
};

class Player
{
public:

  Player(void);
  ~Player(void);

  // draw all waypoints and draw the probas of the selected parent
  void drawWaypoints(oboy::Graphics *g, const oboylib::Vector2 camera);

  // create a new waypoint and make it the selected waypoint
  bool createWaypoint(oboylib::Vector2 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships);

  // find a waypoint in the area defined by the mouse position and choose it as the selected waypoint
  bool pickWaypoint(const oboylib::Vector2 position);

  // update the proba of a ring with the mouse
  bool updateProbaRing(const oboylib::Vector2 position);

  // update the proba of an arrow with the mouse
  bool updateProbaArrow(const oboylib::Vector2 position);

  // check if an arrow is touched
  bool isArrowTouched(const oboylib::Vector2 position);

  // check if a ring is touched
  bool isRingTouched(const oboylib::Vector2 position);

  // move a waypoint with the mouse
  bool moveWaypoint(const oboylib::Vector2 position, std::vector<Planet*> &planets);
  
  // find a waypoint in the zone defined by the mouse position and delete it
  void deleteWaypoint(const oboylib::Vector2 position);

  // release the selected waypoint
  void releaseWaypoint(const oboylib::Vector2 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships);

  // release the selected arrow
  void releaseArrow();

  // release the selected ring
  void releaseRing();

  // update the player
  void update(const float dt);

  // update the strategy of the player (only for COMPUTER)
  void updateStrategy();

  // get and set the core planet of the player
  inline Core* getCore() const { return mPlayerCore; };
  inline void setCore(Core *core) { mPlayerCore = core; };

  // get and set the color of the player
  inline oboylib::Color getColor() const { return mColor; };
  inline void setColor(const oboylib::Color color) { mColor = color; };

  // get and set the id of the player (HUMAN or COMPUTER)
  inline int getPlayerId() const { return mPlayerId; };
  inline void setPlayerId(const int id) { mPlayerId = id; };

  // get and set the strategy of the player (only for COMPUTER)
  inline int getPlayerStrategy() const { return mPlayerStrategy; };
  inline void setPlayerStrategy(const int type) { mPlayerStrategy = type; };

private:

  // delete waypoints that do not have any parent or sibling
  void cleanWaypoints();

private:
  
  oboylib::Color mColor;
  int mPlayerId;
  int mPlayerStrategy;
  Core* mPlayerCore;
  oboylib::Vector2 mOriginalMousePos;
  std::vector<Waypoint*> mWaypoints;
};
