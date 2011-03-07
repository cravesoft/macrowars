// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "OBoy/ResourceManager.h"
#include "OBoy/Graphics.h"

#include "Asteroid.h"

Asteroid::Asteroid(void)
{
  mColor = OBoyLib::Color::Gray;
  mRadius = 16;
  mImage = OBoy::Environment::getImage("IMAGE_ASTEROID");
}

Asteroid::~Asteroid(void)
{
}

void Asteroid::draw(OBoy::Graphics *g, const OBoyLib::Vector2 camera)
{
  int w = OBoy::Environment::screenWidth();
	int h = OBoy::Environment::screenHeight();
	OBoyLib::Vector2 pos = mPos - camera;

	g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
	_draw(g,pos);
	g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}

void Asteroid::_draw(OBoy::Graphics *g, OBoyLib::Vector2 &pos)
{
	if (mImage==NULL)
	{
		return;
	}

	g->pushTransform();
		//g->rotate(mRot);
    g->setColorizationEnabled(true);
    g->setColor(mColor);
		g->translate(pos.x(),pos.y());
		g->drawImage(mImage);
    g->setColorizationEnabled(false);
	g->popTransform();
}

void Asteroid::update()
{
}

void Asteroid::handleShipCollision(Ship* ship)
{
  ship->setShipState(Ship::SHIP_DEAD);
}