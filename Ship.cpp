// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "OBoy/Graphics.h"

#include "Ship.h"
#include "Player.h"

#define SHIP_WIDTH 4

#define callMemberFunction(object,ptrToMember) ((object)->*(ptrToMember))

Ship::Ship(OriginAndDestination *origin, OriginAndDestination *destination,
           const Player* player, OBoyLib::Vector2 position) : Traveller(origin, destination), Satellite()
{
  mSpeed = .1f;
  mAngle = 0;
  mRadius = SHIP_WIDTH;
  mColor = player->getColor();
  mShipOwner = player;
  mPos = position;
  mShipState = SHIP_ALIVE;
  mNumParticles = 10;
  mParticles = new Particle[mNumParticles];
  mMoveFuncs.push_back(&Ship::moveAsTraveller);
  mMoveFuncs.push_back(&Ship::moveAsSatellite);
  for (int i = 0; i < mNumParticles; ++i)
  {
		mParticles[i].active = true;
    mParticles[i].life = OBoyLib::Color::Black;
		//mParticles[i].fade = (float)(rand()%100)/1000.0f+0.003f;
    mParticles[i].fade = 0x01;
		mParticles[i].color = OBoyLib::Color::Yellow;
    mParticles[i].pos.x() = 0;
		mParticles[i].pos.y() = 0;
		mParticles[i].dir.x() = (float)((rand()%50)-26.0f)*10.0f;
		mParticles[i].dir.y() = (float)((rand()%50)-26.0f)*10.0f;
#if 0
    mParticles[i].shape = OBoy::Environment::instance()->createLineStrip(5);
    mParticles[i].shape->setVertPos(0, -2, -2);
    mParticles[i].shape->setVertPos(1, 4, -2);
    mParticles[i].shape->setVertPos(2, 4, 4);
    mParticles[i].shape->setVertPos(3, -2, 4);
    mParticles[i].shape->setVertPos(4, -2, -2);
    mParticles[i].shape->setColor(mParticles[i].color);
    mParticles[i].shape->build();
#endif
	}
  mTriangle = OBoy::Environment::instance()->createLineStrip(4);
  mTriangle->setVertPos(0, 0, 5);
  mTriangle->setVertPos(1, 12, 0);
  mTriangle->setVertPos(2, 0, -5);
  mTriangle->setVertPos(3, 0, 5);
  mTriangle->setColor(mColor);
  mTriangle->build();
}

Ship::~Ship(void)
{
  delete mTriangle;
  delete[] mParticles;
}

void Ship::draw(OBoy::Graphics *g, const OBoyLib::Vector2 camera)
{
	int w = OBoy::Environment::screenWidth();
	int h = OBoy::Environment::screenHeight();
	OBoyLib::Vector2 pos = mPos - camera;

	g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
	_draw(g,pos);
	g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}

void Ship::_draw(OBoy::Graphics *g, OBoyLib::Vector2 &pos)
{
  if (mShipState > 0)
  {
    g->pushTransform();
      //g->setColor(mColor);
#ifdef OBOY_PLATFORM_WIN32
      g->rotate(-mAngle);
      g->translate(pos.x(), pos.y());
#else
      g->translate(pos.x(), pos.y());
      g->rotate(-mAngle);
#endif
      g->drawLineStrip(mTriangle);
    g->popTransform();
  }
  else
  {
#if 1
    for (int i = 0; i < mNumParticles; ++i)
    {
	    if (mParticles[i].active)
      {
        g->pushTransform();
          g->setColor(mParticles[i].color.argb | mParticles[i].life.argb);
          g->translate(pos.x() + mParticles[i].pos.x(), pos.y() + mParticles[i].pos.y());
          g->drawRect(-2, -2, 4, 4);
#if 0
          g->drawLineStrip(mParticles[i].shape);
#endif
        g->popTransform();
      }
    }
#endif
  }
}

void Ship::handleShipCollision(Ship* ship)
{
  this->setShipState(SHIP_DEAD);
  ship->setShipState(SHIP_DEAD);
}

void Ship::moveAsSatellite(float speed)
{
  Satellite::move(speed);
}

void Ship::moveAsTraveller(float speed)
{
  Traveller::move(speed);
}

void Ship::move(float speed)
{
  callMemberFunction(this, mMoveFuncs[mOrbiting])(speed);
}

void Ship::update(const float dt)
{
  mSpeed = dt*0.5f;

  if (mShipState > 0)
  {
    move(mSpeed);
  }
  else if (mShipState == SHIP_DEAD)
  {
    mShipState = SHIP_REMOVE;
    float	slowdown = 2.0f;
    for (int i = 0; i < mNumParticles; ++i)
    {
		  if (mParticles[i].active)
      {
        mShipState = SHIP_DEAD;
        mParticles[i].pos += mParticles[i].dir/(slowdown*1000);
        mParticles[i].dir -= mAccel/(slowdown*10);
		    mParticles[i].life.argb -= mParticles[i].fade << 24;
#if 0
        mParticles[i].shape->setColor(mParticles[i].color | mParticles[i].life);
        mParticles[i].shape->build();
#endif
        if (mParticles[i].life.argb <= 0xffffff00)
          mParticles[i].active = false;
      }
    }
  }
}