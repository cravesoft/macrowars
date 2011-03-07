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

class Macro: public OBoy::Game, public OBoy::KeyboardListener, public OBoy::MouseListener
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
	virtual void draw(OBoy::Graphics *g);

	// implementation of KeyboardListener:
	virtual void keyUp(wchar_t unicode, OBoy::Keyboard::Key key, OBoy::Keyboard::Modifiers mods);
	virtual void keyDown(wchar_t unicode, OBoy::Keyboard::Key key, OBoy::Keyboard::Modifiers mods);

  // implementation of MouseListener:
  virtual void mouseMove(OBoy::Mouse *mouse);
  virtual void mouseButtonDown(OBoy::Mouse *mouse, OBoy::Mouse::Button button, int clickCount);
	virtual void mouseButtonUp(OBoy::Mouse *mouse, OBoy::Mouse::Button button);
	virtual void mouseWheel(OBoy::Mouse *mouse, int wheelDelta);
	virtual void mouseEnter(OBoy::Mouse *mouse);
	virtual void mouseLeave(OBoy::Mouse *mouse);

	void newGame();
  void endGame();
  void pauseGame();
  void resumeGame();

private:

  void loadRandomMap(const int numPlayers, const int numPlanets, const int numAsteroids);
  void createBackground();
  void deleteBackground();
  void drawBackground(OBoy::Graphics *g, const OBoyLib::Vector2 camera);
  void updateGame(float dt);
  void updateMenu(float dt);
	void drawGame(OBoy::Graphics *g);
  void drawMenu(OBoy::Graphics *g);

private:

  typedef void (Macro::*updateFunc)(float);
  ::std::vector<updateFunc> mUpdateFuncs;

  typedef void (Macro::*drawFunc)(OBoy::Graphics *g);
  ::std::vector<drawFunc> mDrawFuncs;

	static Macro *gInstance;

	std::vector<Asteroid*> mAsteroids;
	std::vector<Planet*> mPlanets;
  std::vector<Core*> mCores;
  std::vector<Ship*> mShips;
	std::vector<Player*> mPlayers;

  GuiSystem *mGuiSystem;
	bool mLoadComplete;
	OBoy::Sound *mBoomSound;
  OBoy::Sound *mExplosionSound;
	OBoy::Sound *mFireSound;
	OBoy::Sound *mThrustSound;
	OBoy::Font *mFont;
	bool mGameOver;
	int mDrawCount;
  bool mDrawMinHp;
  bool mDrawWaypoints;
  bool mDrawShips;
  bool mPlanetSelected;
  bool mArrowSelected;
  bool mRingSelected;
  bool mWaypointSelected;
  OBoyLib::Vector2 mCamera;
  OBoyLib::Vector2 mMove;
  OBoyLib::Vector2 mWorldSize;
  OBoyLib::Vector2 mMousePos;
  std::map<OBoy::Keyboard::Key, bool> mKeyPressed;
  std::map<OBoy::Mouse::Button, bool> mButtonPressed;
  float mGameSpeed;
  float mSavedGameSpeed;
  OBoy::Lines *mGrid;

  Player *mMainPlayer;
};
