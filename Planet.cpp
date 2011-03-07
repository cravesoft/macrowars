// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "OBoy/ResourceManager.h"
#include "OBoy/Graphics.h"
#include "OBoy/SoundPlayer.h"
#include "OBoy/TriStrip.h"

#include "Planet.h"
#include "Player.h"

#define PLANET_DEFAULT_RADIUS 30
#define PLANET_DEFAULT_SPEED 5
#define MAX_HP 1000

Planet::Planet()
{
  init();
}

Planet::~Planet(void)
{
#if 0
  delete mSphere;
#endif
  delete mMinHpStrip;
  delete mCurrentHpStrip;
}

void Planet::init()
{
  mColor = OBoyLib::Color::Gray;
  mRadius = PLANET_DEFAULT_RADIUS;
  mSpeed = PLANET_DEFAULT_SPEED;
  mPos = OBoyLib::Vector2(); 
  mCaptured = false;
  mSelected = false;
  mTtl = 0;
  mHp = 0;
  mImage = OBoy::Environment::getImage("IMAGE_PLANET");
  for (int i = 0; i < 3; i++)
  {
    mRot[i] = static_cast<float>(rand()%360);
    if (!(rand()%2))
      mRotVel[i] = static_cast<float>(rand()%100)/1000;
    else
      mRotVel[i] = (-1)*static_cast<float>(rand()%100)/1000;
  }
  OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
  mFont = rm->getFont("FONT_MAIN");
	mExplosionSound = rm->getSound("SOUND_EXPLOSION");
#if 0
  mSphere = OBoy::Environment::instance()->createSphere((float)mRadius, 20, 20);
  mSphere->build();
#endif
  // graphics:
  mMinHpStrip = OBoy::Environment::instance()->createTriStrip(722);
  mMinHpStrip->setColor(OBoyLib::Color::Red & 0x40ffffff);
  mCurrentHpStrip = OBoy::Environment::instance()->createTriStrip(722);
  mCurrentHpStrip->setColor(OBoyLib::Color::Red & 0x40ffffff);
  updateHp(10);
}
#if 0
void Planet::setImage(const ::std::string &id)
{
  OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
  mImage = rm->getImage(id);
  mSphere->setTexture(mImage);
  //rm->saveResource((OBoy::Resource*)mImage, "image");
}
#endif

void Planet::setRadius(const int radius)
{
  Body::setRadius(radius);
#if 0
  delete mSphere;
  mSphere = OBoy::Environment::instance()->createSphere((float)mRadius, 20, 20);
  mSphere->build();
#endif
}

void Planet::draw(OBoy::Graphics *g, OBoyLib::Vector2 camera)
{
}

void Planet::draw(OBoy::Graphics *g, const OBoyLib::Vector2 &camera, const bool drawMinHp, const Player* player)
{
  int w = OBoy::Environment::screenWidth();
	int h = OBoy::Environment::screenHeight();
	OBoyLib::Vector2 pos = mPos - camera;

	g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
  //g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
  _draw(g,pos);
	g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);

  if (drawMinHp)
  {
    //g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    _drawMinHp(g, pos, player);
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    if (mWaypoints[player] == NULL || !mWaypoints[player]->isSelectedAsParent())
      _drawText(g, pos, player);
  }
}

void Planet::_draw(OBoy::Graphics *g, const OBoyLib::Vector2 &pos)
{
	if (mImage==NULL)
	{
		return;
	}
	g->pushTransform();
    g->setColorizationEnabled(true);
    g->setColor(mColor);
#if 0
#ifdef OBOY_PLATFORM_WIN32
    g->rotate(mRot[0], mRot[1], mRot[2]);
    g->translate(pos.x(),pos.y());
#else
    g->translate(pos.x(),pos.y());
    g->rotate(mRot[0], mRot[1], mRot[2]);
#endif
#else
    g->translate(pos.x(),pos.y());
#endif
    g->drawImage(mImage);
    //g->drawSphere(mSphere);
    g->setColorizationEnabled(false);
	g->popTransform();
}

void Planet::_drawMinHp(OBoy::Graphics *g, const OBoyLib::Vector2 &pos, const Player* player)
{
	g->pushTransform();
    g->translate(pos.x(),pos.y());
    if (mMinHp[player] > 0)
    {
      const float outerRadius = 1.2f*mRadius;
      g->setColor(OBoyLib::Color(OBoyLib::Color::Red));
      g->drawCircle(0, 0, outerRadius+mMinHp[player], 1);
      // draw the minimum Hp:
      g->drawTriStrip(mMinHpStrip);
      if (player->getColor() == mColor)
      {
        // draw the current Hp:
        g->drawTriStrip(mCurrentHpStrip);
      }
    }
	g->popTransform();
}

void Planet::_drawText(OBoy::Graphics *g, const OBoyLib::Vector2 &pos, const Player* player)
{
  if (mPlanetOwner == player || mPlanetOwner == NULL || mSelected)
  {
    // draw stats:
    float str2scale = 0.8f;
		OBoy::UString stats;
    if (!mSelected)
      stats = OBoy::UString::format("%d", mHp);
    else if (mPlanetOwner == player || mPlanetOwner == NULL)
      stats = OBoy::UString::format("%d/%d", mHp, mMinHp[player]);
    else
      stats = OBoy::UString::format("?/%d", mMinHp[player]);
    float x = pos.x() - (mFont->getStringWidth(stats)*str2scale) / 2.0f;
		g->setColorizationEnabled(true);
    g->setColor(OBoyLib::Color::White);
		g->pushTransform();
		  g->translate(x, pos.y());
      //g->setZ(mRadius+1);
		  mFont->drawString(g,stats,str2scale);
		g->popTransform();
		g->setColorizationEnabled(false);
  }
}

float Planet::getDistance(const OBoyLib::Vector2 pos)
{
  return abs((mPos - pos).magnitude() - mRadius);
}

void Planet::capture(const Player* player)
{
  mColor = player->getColor();
  mPlanetOwner = player;
  mCaptured = true;
  mTtl = 0;
#if 0
  mSphere->setColor(mColor);
  mSphere->build();
#endif
}

void Planet::release()
{
  mColor = OBoyLib::Color::Gray;
  mCaptured = false;
  mTtl = 0;
#if 0
  mSphere->setColor(mColor);
  mSphere->build();
#endif
}

void Planet::changeMinHp(int diffHp, const Player* player, std::vector<Ship*>& ships)
{
  mMinHp[player] += diffHp;
  if (mMinHp[player] < 0)
    mMinHp[player] = 0;
  else if (mMinHp[player] > MAX_HP)
    mMinHp[player] = MAX_HP;
  if (mHp > mMinHp[player] && mColor == player->getColor())
    releaseShips(ships);
  // graphics
  const float outerRadius = 1.2f*mRadius;
  float angle = 0;
  int i = 0;
  while (angle <= 360)
  {
    mMinHpStrip->setVertPos(i, outerRadius*cos(deg2rad(angle)), outerRadius*sin(deg2rad(angle)));
    mMinHpStrip->setVertPos(++i, (outerRadius+mMinHp[player])*cos(deg2rad(angle)), (outerRadius+mMinHp[player])*sin(deg2rad(angle)));
    ++i;
    ++angle;
  }
  mMinHpStrip->build();
}

void Planet::updateHp(int delta)
{
  mHp += delta;
  // graphics
  const float outerRadius = 1.2f*mRadius;
  float angle = 0;
  int i = 0;
  while (angle <= 360)
  {
    mCurrentHpStrip->setVertPos(i, outerRadius*cos(deg2rad(angle)), outerRadius*sin(deg2rad(angle)));
    mCurrentHpStrip->setVertPos(++i, (outerRadius+mHp)*cos(deg2rad(angle)), (outerRadius+mHp)*sin(deg2rad(angle)));
    ++i;
    ++angle;
  }
  mCurrentHpStrip->build();
}

int Planet::releaseShips(std::vector<Ship*>& ships)
{
  int numShipsReleased = 0;
  if (mWaypoints[mPlanetOwner] != NULL && mWaypoints[mPlanetOwner]->getSiblings().size() > 0)
  {
    for (int i = 0; i < mHp - mMinHp[mPlanetOwner]; i++)
    {
      if (rand()%100 > mWaypoints[mPlanetOwner]->getProbaRing() || mHp >= mMinHp[mPlanetOwner])
      {
        Ship* ship = new Ship(mWaypoints[mPlanetOwner], mWaypoints[mPlanetOwner]->getShipDestination(), mPlanetOwner, mPos);
        ships.push_back(ship);
        numShipsReleased ++;
      }
    }
  }
  updateHp(-numShipsReleased);
  return numShipsReleased;
}

void Planet::launchShip(std::vector<Ship*>& ships)
{
  if (mWaypoints[mPlanetOwner] != NULL && mWaypoints[mPlanetOwner]->getSiblings().size() > 0 && (rand()%100 > mWaypoints[mPlanetOwner]->getProbaRing() || mHp >= mMinHp[mPlanetOwner]))
  {
    Ship* ship = new Ship(mWaypoints[mPlanetOwner], mWaypoints[mPlanetOwner]->getShipDestination(), mPlanetOwner, mPos);
    ships.push_back(ship);
    return;
  }
  updateHp(+1);
}

void Planet::update(float dt, std::vector<Ship*>& ships)
{
  if (mCaptured)
  {
    mTtl += dt;
    if (mTtl > mSpeed)
    {
      // create a ship and release it
      launchShip(ships);
      mTtl = 0;
    }
  }
  for (int i = 0; i < 3; i++)
  {
    mRot[i] += mRotVel[i]*dt;
    if (mRot[i] >= 360 && mRotVel[i] > 0)
      mRot[i] = 0;
    else if (mRot[i] == 0 && mRotVel[i] < 0)
      mRot[i] = 360;
  }
}

void Planet::handleShipCollision(Ship* ship)
{
  if (mPlanetOwner != ship->getShipOwner()) // impact
  {
    ship->setShipState(Ship::SHIP_DEAD);
    // play a sound:
    OBoy::Environment::instance()->getSoundPlayer()->playSound(mExplosionSound);
    if (mHp > 0)
    {
      updateHp(-1);
      if (mHp == 0)
        release();
    }
    else
    {
      capture(ship->getShipOwner());
    }
  }
  else
  {
    Waypoint* waypoint = mWaypoints[ship->getShipOwner()];
    if (waypoint != NULL && waypoint->getSiblings().size() > 0 && (rand()%100 > waypoint->getProbaRing() || mHp >= mMinHp[mPlanetOwner])) // choose a new destination
    {
      ship->setOriginAndDestination(waypoint, waypoint->getShipDestination());
    }
    else // increase planet energy
    {
      updateHp(+1);
      ship->setShipState(Ship::SHIP_REMOVE);
    }
  }
}