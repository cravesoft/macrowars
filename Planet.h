// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include <vector>
#include <map>

#include "OBoy/Font.h"
#include "OBoyLib/Vector2.h"
#include "OBoy/Sphere.h"

#include "OriginAndDestination.h"
#include "Waypoint.h"
#include "Ship.h"

class Player;
class Waypoint;

class Planet : public OriginAndDestination
{
public:

  Planet();
  virtual ~Planet(void);

  void init();
  virtual void draw(OBoy::Graphics *g, const OBoyLib::Vector2 camera);
  void draw(OBoy::Graphics *g, const OBoyLib::Vector2 &camera, const bool drawMinHp, const Player* player);

  float getDistance(const OBoyLib::Vector2);
  void update(const float dt, std::vector<Ship*>& ships);
  void capture(const Player* player);
  void changeMinHp(const int diffHp, const Player* player, std::vector<Ship*>& ships);
  virtual void handleShipCollision(Ship* ship);
  
  int releaseShips(std::vector<Ship*>& ships);
  void launchShip(std::vector<Ship*>& ships);
  
  virtual void setRadius(const int radius);
#if 0
  virtual void setImage(const ::std::string &id);
#endif
  inline void setSpeed(const int speed) { mSpeed = speed; };
  
  inline bool isSelected() const { return mSelected; };
  inline void setSelected(const bool selected) { mSelected = selected; };
  
  inline void setWaypoint(Waypoint* waypoint, const Player* player) { mWaypoints[player] = waypoint; };

private:
	void _draw(OBoy::Graphics *g, const OBoyLib::Vector2 &pos);
  void _drawMinHp(OBoy::Graphics *g, const OBoyLib::Vector2 &pos, const Player* player);
  void _drawText(OBoy::Graphics *g, const OBoyLib::Vector2 &pos, const Player* player);
  void updateHp(int delta);
  void release();

protected:
  int mSpeed;
  OBoy::Image *mImage;

private:
  
  int mHp;
  float mRot[3];
  float mRotVel[3];
  std::map<const Player*, int> mMinHp;
  const Player* mPlanetOwner;
  float mTtl;
  bool mCaptured;
  OBoy::Font *mFont;
  OBoy::Sound *mExplosionSound;
  std::map<const Player*, Waypoint*> mWaypoints;
  bool mSelected;
#if 0
  OBoy::Sphere* mSphere;
#endif
  OBoy::TriStrip* mMinHpStrip;
  OBoy::TriStrip* mCurrentHpStrip;
};
