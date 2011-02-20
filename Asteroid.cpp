// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "OBoy/ResourceManager.h"
#include "OBoy/Graphics.h"

#include "Asteroid.h"

Asteroid::Asteroid(void)
{
  mColor = oboylib::Color::Gray;
  mRadius = 16;
  mImage = oboy::Environment::getImage("IMAGE_ASTEROID");
}

Asteroid::~Asteroid(void)
{
}

void Asteroid::draw(oboy::Graphics *g, const oboylib::Vector2 camera)
{
  int w = oboy::Environment::screenWidth();
	int h = oboy::Environment::screenHeight();
	oboylib::Vector2 pos = mPos - camera;

	g->setDrawMode(oboy::Graphics::DRAWMODE_NORMAL);
	_draw(g,pos);
	g->setDrawMode(oboy::Graphics::DRAWMODE_NORMAL);
}

void Asteroid::_draw(oboy::Graphics *g, oboylib::Vector2 &pos)
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