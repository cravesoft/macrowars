#pragma once

#include "OBoy/Environment.h"
#include "OBoy/Font.h"
#include "OBoy/Game.h"
#include "OBoy/Graphics.h"
#include "OBoy/KeyboardListener.h"
#include "OBoy/MouseListener.h"
#include "OBoy/Lines.h"
#include "oboylib/OBoyUtil.h"

class GuiSystem;
class Player;
class Planet;
class Core;
class Asteroid;
class Ship;

class Macro: public oboy::Game, public oboy::KeyboardListener, public oboy::MouseListener
{
public:

	Macro();
	virtual ~Macro();

	// singleton:
	static Macro *instance();
	static void destroy();

	// implementation of Game:
	virtual void init();
	virtual void load();
	virtual void loadComplete();
	virtual void update(float dt);
	virtual void draw(oboy::Graphics *g);

	// implementation of KeyboardListener:
	virtual void keyUp(wchar_t unicode, oboy::Keyboard::Key key, oboy::Keyboard::Modifiers mods);
	virtual void keyDown(wchar_t unicode, oboy::Keyboard::Key key, oboy::Keyboard::Modifiers mods);

  // implementation of MouseListener:
  virtual void mouseMove(oboy::Mouse *mouse);
  virtual void mouseButtonDown(oboy::Mouse *mouse, oboy::Mouse::Button button, int clickCount);
	virtual void mouseButtonUp(oboy::Mouse *mouse, oboy::Mouse::Button button);
	virtual void mouseWheel(oboy::Mouse *mouse, int wheelDelta);
	virtual void mouseEnter(oboy::Mouse *mouse);
	virtual void mouseLeave(oboy::Mouse *mouse);

	void newGame();
  void endGame();
  void pauseGame();
  void resumeGame();

private:

  void loadRandomMap(const int numPlayers, const int numPlanets, const int numAsteroids);
  void createBackground();
  void deleteBackground();
  void drawBackground(oboy::Graphics *g, const oboylib::Vector2 camera);
  void updateGame(float dt);
  void updateMenu(float dt);
	void drawGame(oboy::Graphics *g);
  void drawMenu(oboy::Graphics *g);

private:

  typedef void (Macro::*updateFunc)(float);
  ::std::vector<updateFunc> mUpdateFuncs;

  typedef void (Macro::*drawFunc)(oboy::Graphics *g);
  ::std::vector<drawFunc> mDrawFuncs;

	static Macro *gInstance;

	std::vector<Asteroid*> mAsteroids;
	std::vector<Planet*> mPlanets;
  std::vector<Core*> mCores;
  std::vector<Ship*> mShips;
	std::vector<Player*> mPlayers;

  GuiSystem *mGuiSystem;
	bool mLoadComplete;
	oboy::Sound *mBoomSound;
  oboy::Sound *mExplosionSound;
	oboy::Sound *mFireSound;
	oboy::Sound *mThrustSound;
	oboy::Font *mFont;
	bool mGameOver;
	int mDrawCount;
  bool mDrawMinHp;
  bool mDrawWaypoints;
  bool mDrawShips;
  bool mPlanetSelected;
  bool mArrowSelected;
  bool mRingSelected;
  bool mWaypointSelected;
  oboylib::Vector2 mCamera;
  oboylib::Vector2 mMove;
  oboylib::Vector2 mWorldSize;
  oboylib::Vector2 mMousePos;
  std::map<oboy::Keyboard::Key, bool> mKeyPressed;
  std::map<oboy::Mouse::Button, bool> mButtonPressed;
  float mGameSpeed;
  float mSavedGameSpeed;
  oboy::Lines *mGrid;

  Player *mMainPlayer;
};
