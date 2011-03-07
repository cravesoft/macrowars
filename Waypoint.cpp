// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include <algorithm>
#include <functional>
#include <assert.h>

#include "OBoy/ResourceManager.h"
#include "OBoy/Graphics.h"

#include "Waypoint.h"

#define ARROW_HALF_WIDTH 8
#define ARROW_HALF_HEAD_WIDTH 16
#define ARROW_HEAD_HEIGHT 32
#define WAYPOINT_DEFAULT_RADIUS 32

Waypoint::Waypoint(void)
{
  mPos = OBoyLib::Vector2();
  mPlanet = NULL;
  mColor = OBoyLib::Color::White;
  mDrawRing = true;
  mSelected = false;
  mSelectedAsParent = false;
  OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
  mFont = rm->getFont("FONT_MAIN");
  mProbaRing = 50;
  mRingStrip = OBoy::Environment::instance()->createTriStrip(722);
  setRadius(WAYPOINT_DEFAULT_RADIUS);
}

Waypoint::~Waypoint(void)
{
  delete mRingStrip;
}

void Waypoint::setRadius(const int radius)
{
  Body::setRadius(radius);
  //graphics:
  const float outerRadius = 1.2f*mRadius;
  mRingStrip->setColor((mColor.argb & 0x00ffffff) | ((mProbaRing*0xff/100) << 24) );
  float angle = 0;
  int i = 0;
  while (angle <= 360)
  {
    mRingStrip->setVertPos(i, mRadius*cos(deg2rad(angle)), mRadius*sin(deg2rad(angle)));
    mRingStrip->setVertPos(++i, outerRadius*cos(deg2rad(angle)), outerRadius*sin(deg2rad(angle)));
    ++i;
    ++angle;
  }
  mRingStrip->build();
}
#if 0
void Waypoint::createArrow(Waypoint* sibling)
{
  mArrowTailTriStrips[sibling] = OBoy::Environment::instance()->createTriStrip(4);
  mArrowHeadTriStrips[sibling] = OBoy::Environment::instance()->createTriStrip(4);
  mArrowLineStrips[sibling] = OBoy::Environment::instance()->createLineStrip(7);
}

void Waypoint::deleteArrow(Waypoint* sibling)
{
  delete(mArrowTailTriStrips[sibling]);
  delete(mArrowHeadTriStrips[sibling]);
  delete(mArrowLineStrips[sibling]);
}

void Waypoint::updateArrow(Waypoint* sibling, const float length, const OBoy::Color color)
{
  mArrowTailTriStrips[sibling]->setColor(color);
  mArrowTailTriStrips[sibling]->setVertPos(0, length, ARROW_HALF_WIDTH);
  mArrowTailTriStrips[sibling]->setVertPos(1, 0, 0);
  mArrowTailTriStrips[sibling]->setVertPos(2, length, 0);
  mArrowTailTriStrips[sibling]->setVertPos(3, length, -ARROW_HALF_WIDTH);
  mArrowTailTriStrips[sibling]->build();
  
  mArrowHeadTriStrips[sibling]->setColor(color);
  mArrowHeadTriStrips[sibling]->setVertPos(0, length, ARROW_HALF_HEAD_WIDTH);
  mArrowHeadTriStrips[sibling]->setVertPos(1, length, 0);
  mArrowHeadTriStrips[sibling]->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
  mArrowHeadTriStrips[sibling]->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
  mArrowHeadTriStrips[sibling]->build();
    
  mArrowLineStrips[sibling]->setColor(mColor);
  mArrowLineStrips[sibling]->setVertPos(0, length, ARROW_HALF_WIDTH);
  mArrowLineStrips[sibling]->setVertPos(1, length, ARROW_HALF_HEAD_WIDTH);
  mArrowLineStrips[sibling]->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
  mArrowLineStrips[sibling]->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
  mArrowLineStrips[sibling]->setVertPos(4, length, -ARROW_HALF_WIDTH);
  mArrowLineStrips[sibling]->setVertPos(5, 0, 0);
  mArrowLineStrips[sibling]->setVertPos(6, length, ARROW_HALF_WIDTH);
  mArrowLineStrips[sibling]->build();
}
#endif
#if 1
void Waypoint::drawArrow(OBoy::Graphics *g, const float length, const OBoyLib::Color color)
{
  OBoy::TriStrip* arrowTailTriStrip = OBoy::Environment::instance()->createTriStrip(4);
  arrowTailTriStrip->setColor(color);
  arrowTailTriStrip->setVertPos(0, length, ARROW_HALF_WIDTH);
  arrowTailTriStrip->setVertPos(1, 0, 0);
  arrowTailTriStrip->setVertPos(2, length, 0);
  arrowTailTriStrip->setVertPos(3, length, -ARROW_HALF_WIDTH);
  arrowTailTriStrip->build();
  g->drawTriStrip(arrowTailTriStrip);
  delete arrowTailTriStrip;

  OBoy::TriStrip* arrowHeadTriStrip = OBoy::Environment::instance()->createTriStrip(4);
  arrowHeadTriStrip->setColor(color);
  arrowHeadTriStrip->setVertPos(0, length, ARROW_HALF_HEAD_WIDTH);
  arrowHeadTriStrip->setVertPos(1, length, 0);
  arrowHeadTriStrip->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
  arrowHeadTriStrip->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
  arrowHeadTriStrip->build();
  g->drawTriStrip(arrowHeadTriStrip);
  delete arrowHeadTriStrip;

  OBoy::LineStrip* arrowLineStrip = OBoy::Environment::instance()->createLineStrip(7);
  arrowLineStrip->setColor(mColor);
  arrowLineStrip->setVertPos(0, length, ARROW_HALF_WIDTH);
  arrowLineStrip->setVertPos(1, length, ARROW_HALF_HEAD_WIDTH);
  arrowLineStrip->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
  arrowLineStrip->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
  arrowLineStrip->setVertPos(4, length, -ARROW_HALF_WIDTH);
  arrowLineStrip->setVertPos(5, 0, 0);
  arrowLineStrip->setVertPos(6, length, ARROW_HALF_WIDTH);
  arrowLineStrip->build();
  g->drawLineStrip(arrowLineStrip);
  delete arrowLineStrip;
}
#endif
void Waypoint::setDefaultRadius()
{
  mRadius = WAYPOINT_DEFAULT_RADIUS;
}

void Waypoint::draw(OBoy::Graphics *g, const OBoyLib::Vector2 camera)
{
  int w = OBoy::Environment::screenWidth();
	int h = OBoy::Environment::screenHeight();
	OBoyLib::Vector2 pos = mPos - camera;
  const float outerRadius = 1.2f*mRadius;

  if (mDrawRing)
  {
    g->pushTransform();
      g->setColor(mColor);
      g->translate(pos.x(), pos.y());
      // draw the ring:
      g->drawCircle(0, 0, (float)mRadius, 1);
      g->drawCircle(0, 0, outerRadius, 1);
      // draw the inside of the ring:
      g->drawTriStrip(mRingStrip);
    g->popTransform();
  }
  for (int i=0 ; i<(int)mSiblings.size() ; i++)
  {
    if (mSiblings[i]->getPosition() != mPos)
    {
      // compute the angle between the sibling and its parent:
      OBoyLib::Vector2 direction = (mSiblings[i]->getPosition() - mPos).normalize();
      float angle = atan2(direction.y(), direction.x());
      // compute the length of the arrow
      float length = (mSiblings[i]->getPosition() - mPos).magnitude() - ARROW_HEAD_HEIGHT;
      g->pushTransform();
        g->setColor(mColor);
        if (mSiblings[i]->getDrawRing())
        {
          length -= 1.2f*mSiblings[i]->getRadius();
        }
#ifdef OBOY_PLATFORM_WIN32
        if (mDrawRing)
        {
          g->translate(outerRadius, 0);
          length -= outerRadius;
        }
        g->rotate(rad2deg(-angle));
        g->translate(pos.x(), pos.y());
#else
        g->translate(pos.x(), pos.y());
        g->rotate(rad2deg(-angle));
        if (mDrawRing)
        {
          g->translate(outerRadius, 0);
          length -= outerRadius;
        }
#endif
        //g->drawTriStrip(mArrowTailTriStrips[mSiblings[i]]);
        //g->drawTriStrip(mArrowHeadTriStrips[mSiblings[i]]);
        //g->drawLineStrip(mArrowLineStrips[mSiblings[i]]);
        drawArrow(g, length, (mColor.argb & 0x00ffffff) | ((mProbas[mSiblings[i]]*0xff/100) << 24) );
      g->popTransform();
    }
  }
}

void Waypoint::drawProbas(OBoy::Graphics *g, const OBoyLib::Vector2 camera)
{
  int w = OBoy::Environment::screenWidth();
	int h = OBoy::Environment::screenHeight();
	OBoyLib::Vector2 pos = mPos - camera;
  
  OBoy::UString stats;
  float str2scale = 0.8f;
  g->setColor(OBoyLib::Color::White);
  stats = OBoy::UString::format("%d%%", mProbaRing);
  float x = pos.x() - (mFont->getStringWidth(stats)*str2scale) / 2.0f;
	g->pushTransform();
    g->translate(x, pos.y());
	  mFont->drawString(g,stats,str2scale);
	g->popTransform();

  const float outerRadius = 1.2f*mRadius;
  for (int i=0 ; i<(int)mSiblings.size() ; i++)
  {
    stats = OBoy::UString::format("%d%%", mProbas[mSiblings[i]]);
    OBoyLib::Vector2 position;
    OBoyLib::Vector2 direction = (mSiblings[i]->getPosition() - mPos).normalize();
    float angle = atan2(direction.y(), direction.x());
    position.x() = (cos(angle)*((mSiblings[i]->getPosition() - mPos).magnitude() - outerRadius - 1.2f*mSiblings[i]->getRadius()))/2 + pos.x() + cos(angle)*outerRadius;
    position.y() = (sin(angle)*((mSiblings[i]->getPosition() - mPos).magnitude() - outerRadius - 1.2f*mSiblings[i]->getRadius()))/2 + pos.y() + sin(angle)*outerRadius;
    g->pushTransform();
	    g->translate(position.x(), position.y());
	    mFont->drawString(g,stats,str2scale);
	  g->popTransform();
  }
}

bool Waypoint::isTouched(const OBoyLib::Vector2 pos)
{
  if ((mPos - pos).magnitude() < 1.2f*mRadius)
    return true;
  else
    return false;
}

bool Waypoint::isRingTouched(const OBoyLib::Vector2 pos)
{
  float distance = (mPos - pos).magnitude();
  if (distance > mRadius && distance < 1.2f*mRadius)
    return true;
  else
    return false;
}

bool Waypoint::isArrowTouched(const OBoyLib::Vector2 pos, Waypoint* &parent)
{
  for (int i=0 ; i<(int)mParents.size() ; i++)
  {
    OBoyLib::Vector2 direction = (mParents[i]->getPosition() - mPos).normalize();
    float angle = rad2deg(atan2(direction.y(), direction.x()));
    const float outerRadius = 1.2f*mRadius;

    OBoyLib::Vector2 ul = OBoyLib::Vector2();
    ul.x() = cos(deg2rad(angle))*outerRadius- sin(deg2rad(angle))*(-ARROW_HALF_HEAD_WIDTH) + mPos.x();
    ul.y() = sin(deg2rad(angle))*outerRadius + cos(deg2rad(angle))*(-ARROW_HALF_HEAD_WIDTH) + mPos.y();

    OBoyLib::Vector2 ur = OBoyLib::Vector2();
    ur.x() = cos(deg2rad(angle))*outerRadius - sin(deg2rad(angle))*ARROW_HALF_HEAD_WIDTH + mPos.x();
    ur.y() = sin(deg2rad(angle))*outerRadius + cos(deg2rad(angle))*ARROW_HALF_HEAD_WIDTH + mPos.y();

    OBoyLib::Vector2 lr = OBoyLib::Vector2();
    lr.x() = cos(deg2rad(angle))*((mParents[i]->getPosition() - mPos).magnitude()-1.2f*mParents[i]->getRadius()) 
           - sin(deg2rad(angle))*ARROW_HALF_HEAD_WIDTH + mPos.x();
    lr.y() = sin(deg2rad(angle))*((mParents[i]->getPosition() - mPos).magnitude()-1.2f*mParents[i]->getRadius()) 
           + cos(deg2rad(angle))*ARROW_HALF_HEAD_WIDTH + mPos.y();

    OBoyLib::Vector2 ll = OBoyLib::Vector2();
    ll.x() = cos(deg2rad(angle))*((mParents[i]->getPosition() - mPos).magnitude()-1.2f*mParents[i]->getRadius())
           - sin(deg2rad(angle))*(-ARROW_HALF_HEAD_WIDTH) + mPos.x();
    ll.y() = sin(deg2rad(angle))*((mParents[i]->getPosition() - mPos).magnitude()-1.2f*mParents[i]->getRadius())
           + cos(deg2rad(angle))*(-ARROW_HALF_HEAD_WIDTH) + mPos.y();

    OBoyLib::Vector2 axis1 = ur-ul;
    OBoyLib::Vector2 axis2 = ur-lr;

    float ur1 = (axis1*(ur.dot(axis1)/(axis1.magnitude()*axis1.magnitude()))).dot(axis1);
    float ul1 = (axis1*(ul.dot(axis1)/(axis1.magnitude()*axis1.magnitude()))).dot(axis1);
    float ur2 = (axis2*(ur.dot(axis2)/(axis2.magnitude()*axis2.magnitude()))).dot(axis2);
    float lr2 = (axis2*(lr.dot(axis2)/(axis2.magnitude()*axis2.magnitude()))).dot(axis2);
    float pos1 = (axis1*(pos.dot(axis1)/(axis1.magnitude()*axis1.magnitude()))).dot(axis1);
    float pos2 = (axis2*(pos.dot(axis2)/(axis2.magnitude()*axis2.magnitude()))).dot(axis2);

    if (pos1 < std::max(ur1, ul1) && pos1 > std::min(ur1, ul1) && pos2 < std::max(ur2, lr2) && pos2 > std::min(ur2, lr2))
    {
      parent = mParents[i];
      return true;
    }
  }
  return false;
}

void Waypoint::updateProbasArrow(int proba, Waypoint* sibling)
{
  if (mSiblings.size() > 1)
  {
    int size = static_cast<int>(mSiblings.size());
    int minProba = 1;
    int maxProba = 100 - mProbas[sibling] - minProba*(size - 2);
    int sum = mProbas[sibling];
    int inc_proba = -10/(size - 1);
    std::vector<Waypoint*> probasAlreadyUpdated;
    Waypoint* current_waypoint = NULL;
    for (int i = 0; i < size - 2; i++)
    {
      int current_proba = 999;
      for (int j=0 ; j<(int)mSiblings.size() ; j++)
      {
        if (mProbas[mSiblings[j]] < current_proba && mSiblings[j] != sibling)
        {
          bool alreadyUpdated = false;
          for (int k=0 ; k<(int)probasAlreadyUpdated.size() ; k++)
          {
            if (probasAlreadyUpdated[k] == mSiblings[j])
            {
              alreadyUpdated = true;
              break;
            }
          }
          if (!alreadyUpdated)
          {
            current_proba = mProbas[mSiblings[j]];
            current_waypoint = mSiblings[j];
          }
        }
      }
      probasAlreadyUpdated.push_back(current_waypoint);
      inc_proba = proba/(size - 1);
      if (mProbas[current_waypoint] - inc_proba > maxProba)
      {
        proba += maxProba - mProbas[current_waypoint];
        mProbas[current_waypoint] = maxProba;
      }
      else if (mProbas[current_waypoint] - inc_proba < minProba)
      {
        proba -= mProbas[current_waypoint] - minProba;
        mProbas[current_waypoint] = minProba;
      }
      else
      {
        mProbas[current_waypoint] -= inc_proba;
      }
      sum += mProbas[current_waypoint];
    }
    current_waypoint = NULL;
    for (int j=0 ; j<(int)mSiblings.size() ; j++)
    {
      if (mSiblings[j] != sibling)
      {
        bool alreadyUpdated = false;
        for (int k=0 ; k<(int)probasAlreadyUpdated.size() ; k++)
        {
          if (probasAlreadyUpdated[k] == mSiblings[j])
          {
            alreadyUpdated = true;
            break;
          }
        }
        if (!alreadyUpdated)
        {
          current_waypoint = mSiblings[j];
          break;
        }
      }
    }
    mProbas[current_waypoint] = 100 - sum;
  }
}

void Waypoint::removeProbaArrow(Waypoint* sibling)
{
  int proba = mProbas[sibling];
  mProbas[sibling] = 0;
  updateProbasArrow(-proba, sibling);
}

void Waypoint::addProbaArrow(Waypoint* sibling)
{
  int size = static_cast<int>(mSiblings.size());
  mProbas[sibling] = 100/size;
  updateProbasArrow(mProbas[sibling], sibling);
}

void Waypoint::changeProbaArrow(int proba, Waypoint* sibling)
{
  int size = static_cast<int>(mSiblings.size());
  if (size > 1)
  {
    int minProba = 1;
    int maxProba = 100 - minProba*(size - 1);
    if (mProbas[sibling] + proba > maxProba)
      proba = maxProba - mProbas[sibling];
    else if (mProbas[sibling] + proba < minProba)
      proba = minProba - mProbas[sibling];
    mProbas[sibling] += proba;
    updateProbasArrow(proba, sibling);
  }
}

void Waypoint::changeProbaRing(const int proba)
{
  mProbaRing += proba;
  if (mProbaRing > 100)
    mProbaRing = 100;
  else if (mProbaRing < 0)
    mProbaRing = 0;
  //graphics:
  mRingStrip->setColor((mColor.argb & 0x00ffffff) | ((mProbaRing*0xff/100) << 24));
  mRingStrip->build();
}

void Waypoint::addParent(Waypoint* parent)
{
  if (this == parent || find(mParents.begin(),mParents.end(),parent) != mParents.end())
    return;
  mParents.push_back(parent);
}

void Waypoint::addSibling(Waypoint* sibling)
{
  if (this == sibling || find(mSiblings.begin(),mSiblings.end(),sibling) != mSiblings.end())
    return;
  mSiblings.push_back(sibling);
  addProbaArrow(sibling);
}

void Waypoint::addSibling(Waypoint* sibling, const int proba)
{
  if (this == sibling || find(mSiblings.begin(),mSiblings.end(),sibling) != mSiblings.end())
    return;
  mSiblings.push_back(sibling);
  mProbas[sibling] = proba;
  updateProbasArrow(mProbas[sibling], sibling);
}

void Waypoint::addParents(std::vector<Waypoint*> &parents)
{
  for_each(parents.begin(), parents.end(), std::bind1st(std::mem_fun(&Waypoint::addParent),this));
}

void Waypoint::addSiblings(std::vector<Waypoint*> &siblings)
{
  for_each(siblings.begin(), siblings.end(), std::bind1st(std::mem_fun(&Waypoint::addSibling),this));
}

void Waypoint::removeParent(Waypoint* parent)
{
  assert(find(mParents.begin(),mParents.end(),parent) != mParents.end());
	mParents.erase(find(mParents.begin(),mParents.end(),parent));
}

void Waypoint::removeSibling(Waypoint* sibling)
{
  removeProbaArrow(sibling);
  assert(find(mSiblings.begin(),mSiblings.end(),sibling) != mSiblings.end());
	mSiblings.erase(find(mSiblings.begin(),mSiblings.end(),sibling));
}

void Waypoint::removeAllSiblings()
{
  mSiblings.erase(mSiblings.begin(), mSiblings.end());
}

void Waypoint::removeAllParents()
{
  mParents.erase(mParents.begin(), mParents.end());
}

Waypoint* Waypoint::getShipDestination()
{
  assert (mSiblings.size() > 0);
  int index_dest = rand()%100;
  int sum = 0;
  for (int i=0 ; i<(int)mSiblings.size() ; ++i)
  {
    sum += mProbas[mSiblings[i]];
    if (index_dest <= sum)
    {
      return mSiblings[i];
    }
  }
  return mSiblings[0];
}

void Waypoint::mergeWaypoint(Waypoint* waypoint)
{
  this->addParents(waypoint->getParents());
  this->addSiblings(waypoint->getSiblings());
  std::vector<Waypoint*> parents = waypoint->getParents();
  for (int i=0 ; i<(int)parents.size() ; i++)
  {
    int proba = parents[i]->getProbaArrow(waypoint);
    parents[i]->removeSibling(waypoint);
    parents[i]->addSibling(this, proba);
  }
  std::vector<Waypoint*> siblings = waypoint->getSiblings();
  for (int i=0 ; i<(int)siblings.size() ; i++)
  {
    siblings[i]->removeParent(waypoint);
    siblings[i]->addParent(this);
  }
  waypoint->removeAllParents();
  waypoint->removeAllSiblings();
  
  std::vector<Traveller*> leavingTravellers = waypoint->getLeavingTravellers();
  for (int i=0 ; i<(int)leavingTravellers.size() ; i++)
    leavingTravellers[i]->setOrigin(this);

  std::vector<Traveller*> arrivingTravellers = waypoint->getArrivingTravellers();
  for (int i=0 ; i<(int)arrivingTravellers.size() ; i++)
    arrivingTravellers[i]->setDestination(this);
}

void Waypoint::handleShipCollision(Ship* ship)
{
  if (mPlanet != NULL)
  {
    ship->setOriginAndDestination(this, mPlanet);
  }
  else
  {
    if (mSiblings.size() > 0)
      ship->setOriginAndDestination(this, getShipDestination());
    else
      ship->startOrbiting(this, mRadius + RADIUS_STEP_SIZE);
  }
}