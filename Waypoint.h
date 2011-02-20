// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include <map>

#include "OBoy/Font.h"

#include "Orbitable.h"
#include "OriginAndDestination.h"
#include "Ship.h"
#include "Planet.h"
#include "OBoy/TriStrip.h"
#include "OBoy/LineStrip.h"

class Planet;

class Waypoint : public OriginAndDestination, public Orbitable
{
public:

  Waypoint(void);
  explicit Waypoint(oboylib::Vector2);
  virtual ~Waypoint(void);

  void draw(oboy::Graphics *g, const oboylib::Vector2 camera);
  void drawProbas(oboy::Graphics *g, const oboylib::Vector2 camera);

  virtual bool isTouched(const oboylib::Vector2);
  bool isRingTouched(const oboylib::Vector2);
  bool isArrowTouched(const oboylib::Vector2 pos, Waypoint* &parent);

  void addProbaArrow(Waypoint*);
  void removeProbaArrow(Waypoint*);
  void changeProbaArrow(int, Waypoint*);
  void updateProbasArrow(int, Waypoint*);

  void changeProbaRing(const int proba);

  void addParent(Waypoint*);
  void addSibling(Waypoint*);
  void addSibling(Waypoint*, const int proba);

  void addParents(std::vector<Waypoint*>&);
  void addSiblings(std::vector<Waypoint*>&);

  void removeParent(Waypoint*);
  void removeSibling(Waypoint*);

  void removeAllParents();
  void removeAllSiblings();

  void mergeWaypoint(Waypoint*);

  virtual void handleShipCollision(Ship*);
  
  void setDefaultRadius();
  virtual void setRadius(const int radius);
  
  Waypoint* getShipDestination();
  
  inline std::vector<Waypoint*>& getParents() { return mParents; };
  inline std::vector<Waypoint*>& getSiblings() { return mSiblings; };
  
  inline Planet* getPlanet() { return mPlanet; };
  inline void setPlanet(Planet* planet) { mPlanet = planet; };

  inline int getProbaRing() const { return mProbaRing; };
  inline int getProbaArrow(Waypoint* sibling) { return mProbas[sibling]; };

  inline bool isSelected() const { return mSelected; };
  inline void setSelected(const bool selected) { mSelected = selected; };

  inline bool isSelectedAsParent() const { return mSelectedAsParent; };
  inline void setSelectedAsParent(const bool selected) { mSelectedAsParent = selected; };

  inline bool getDrawRing() const { return mDrawRing; };
  inline void setDrawRing(const bool draw) { mDrawRing = draw; };

private:

  void drawArrow(oboy::Graphics *g, const float length, const oboylib::Color color);
#if 0
  void createArrow(Waypoint* sibling);
  void deleteArrow(Waypoint* sibling);
  void updateArrow(Waypoint* sibling, const float length, const oboylib::Color color);
#endif
private:
  
  std::map<Waypoint*, int> mProbas;
  int mProbaRing;
  bool mDrawRing;
  bool mSelected;
  bool mSelectedAsParent;
  std::vector<Waypoint*> mParents;
  std::vector<Waypoint*> mSiblings;
  Planet* mPlanet;
  oboy::Font *mFont;
  std::map<Waypoint*, oboy::TriStrip*> mArrowTailTriStrips;
  std::map<Waypoint*, oboy::TriStrip*> mArrowHeadTriStrips;
  std::map<Waypoint*, oboy::LineStrip*> mArrowLineStrips;
  oboy::TriStrip* mRingStrip;
};
