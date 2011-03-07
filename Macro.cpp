#include "Macro.h"

#include <functional>
#include <algorithm>
#include <assert.h>
#include "GuiSystem.h"
#include "Player.h"
#include "Asteroid.h"
#include "Core.h"
#include "Ship.h"
#include "OBoy/Environment.h"
#include "OBoy/GamePad.h"
#include "OBoy/Graphics.h"
#include "OBoy/Mouse.h"
#include "OBoy/ResourceManager.h"
#include "OBoy/SoundPlayer.h"
#include "OBoy/CoherentNoise.h"

#include "OBoyLib/Vector3.h"
#include <GL/glew.h>

#define SHIP_ROT_SPEED 200.0f
#define SHIP_ACCELERATION 30.0f
#define SHIP_MAX_SPEED 250.0f
#define SHIP_DAMPENING_FACTOR 1.0f
#define BULLET_SPEED 400.0f
#define MIN_ASTEROIDS 4
#define INVULNERABILITY_PERIOD 2.5f

#define NUM_STARS 1000
#define MAP_WIDTH 3000
#define MAP_HEIGHT 2500
#define NUM_PLAYERS 5
#define NUM_PLANETS 30
#define NUM_ASTEROIDS 100

#define callMemberFunction(object,ptrToMember) ((object)->*(ptrToMember))

Macro *Macro::gInstance = NULL;

Macro::Macro()
{
  mMainPlayer = NULL;
	mLoadComplete = false;
	mGameOver = true;
	mDrawCount = 0;
  mKeyPressed[OBoy::Keyboard::KEY_UP] = false;
  mKeyPressed[OBoy::Keyboard::KEY_DOWN] = false;
  mKeyPressed[OBoy::Keyboard::KEY_LEFT] = false;
  mKeyPressed[OBoy::Keyboard::KEY_RIGHT] = false;
  mUpdateFuncs.push_back(&Macro::updateGame);
  mUpdateFuncs.push_back(&Macro::updateMenu);
  mDrawFuncs.push_back(&Macro::drawGame);
  mDrawFuncs.push_back(&Macro::drawMenu);
}

Macro::~Macro()
{
  delete mGuiSystem; 
	// stop listening to the keyboard and mouse:
	OBoy::Environment::instance()->getKeyboard(0)->removeListener(this);
  OBoy::Environment::instance()->getMouse(0)->removeListener(this);
}

Macro *Macro::instance()
{
	if (gInstance==NULL)
	{
		gInstance = new Macro();
	}

	return gInstance;
}

void Macro::destroy()
{
  // destroy the singleton:
	assert(gInstance!=NULL);
	delete gInstance;
	gInstance = NULL;
}

void Macro::init()
{
  // create the gui system to handle menus and interfaces
  mGuiSystem = new GuiSystem(this);
}

void Macro::load()
{
	// load the common resource group:
	OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
	rm->parseResourceFile("res/resources.xml",NULL);
	rm->loadResourceGroup("common");
  rm->createResourceGroup("textures");
}

void Macro::loadComplete()
{
	// start listening to the keyboard:
	OBoy::Environment::instance()->getKeyboard(0)->addListener(this);

	// start listening to the mouse:
	OBoy::Environment::instance()->getMouse(0)->addListener(this);

	// set the load complete flag (this will trigger 
	// the start of the game in the update method):
	mLoadComplete = true;

	// fetch whatever resources we need:
	OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
	mFont = rm->getFont("FONT_MAIN");
	mBoomSound = rm->getSound("SOUND_BOOM");
	mFireSound = rm->getSound("SOUND_FIRE");
	mThrustSound = rm->getSound("SOUND_THRUST");
  mExplosionSound = rm->getSound("SOUND_EXPLOSION");
}

void Macro::updateGame(float dt)
{
  // Update camera:
  int command = 0;
  if (mKeyPressed[OBoy::Keyboard::KEY_UP])
		command |= 0x8;
  if (mKeyPressed[OBoy::Keyboard::KEY_RIGHT])
		command |= 0x4;
  if (mKeyPressed[OBoy::Keyboard::KEY_DOWN])
    command |= 0x2;
  if (mKeyPressed[OBoy::Keyboard::KEY_LEFT])
    command |= 0x1;
  switch (command)
  {
    case 0x8:  // Up
      mMove = OBoyLib::Vector2(0, -1);
      break;
    case 0x4:  // Right
      mMove = OBoyLib::Vector2(1, 0);
      break;
    case 0x2:  // Down
      mMove = OBoyLib::Vector2(0, 1);
      break;
    case 0x1:  // Left
      mMove = OBoyLib::Vector2(-1, 0);
      break;
    case 0xc:  // Up-Right
      mMove = OBoyLib::Vector2(1.0f/OSQRT2, -1.0f/OSQRT2);
      break;
    case 0x6:  // Down-Right
      mMove = OBoyLib::Vector2(1.0f/OSQRT2, 1.0f/OSQRT2);
      break;
    case 0x3:  // Down-Left
      mMove = OBoyLib::Vector2(-1.0f/OSQRT2, 1.0f/OSQRT2);
      break;
    case 0x9:  // Up-Left
      mMove = OBoyLib::Vector2(-1.0f/OSQRT2, -1.0f/OSQRT2);
      break;
    case 0x0:
      mMove = OBoyLib::Vector2();
      break;
    default:
      ;
  }

  OBoy::Graphics *g = OBoy::Environment::instance()->getGraphics();
	float w = (float)g->getWidth() - 10;
	float h = (float)g->getHeight() - 10;
#if 0
  if (mMousePos.x() < 10)
    mMove.x() -= .5f;
  if (mMousePos.x() > w)
    mMove.x() += .5f;
  if (mMousePos.y() < 10)
    mMove.y() -= .5f;
  if (mMousePos.y() > h)
    mMove.y() += .5f;
#endif
	w = -g->getWidth()/2.0f;
	h = -g->getHeight()/2.0f;
  mCamera = mCamera + (mMove*10.0);
  if (mCamera.x() < w)
		mCamera.x() = w;
	if (mCamera.x() > mWorldSize.x() + w)
		mCamera.x() = mWorldSize.x() + w;
	if (mCamera.y() < h)
		mCamera.y() = h;
	if (mCamera.y() > mWorldSize.y() + h)
		mCamera.y() = mWorldSize.y() + h;

  // Update the speed
  if (mGameSpeed <= 0)
    return;
  dt *= mGameSpeed;

  // Update players:
  for (int i=(int)mPlayers.size()-1 ; i>=0 ; i--)
  {
    mPlayers[i]->update(dt);
    if (mPlayers[i]->getPlayerId() == COMPUTER)
      mPlayers[i]->updateStrategy();
  }

  // Update planets:
  for (int i=(int)mPlanets.size()-1 ; i>=0 ; i--)
  {
		mPlanets[i]->update(dt, mShips);    
  }

  // Update ships:
  for (int i=(int)mShips.size()-1 ; i>=0 ; i--)
  {
    mShips[i]->update(dt);
    if (mShips[i]->getShipState() == Ship::SHIP_ALIVE)
    {
      if (!mShips[i]->isOrbiting() && mShips[i]->hasArrivedAtDestination())
      {
        mShips[i]->getDestination()->handleShipCollision(mShips[i]);
        break;
      }
#if 0
      // collision with other ships:
      for (int j=0 ; j<i ; ++j)
      {
        if (mShips[j]->getColor() != mShips[i]->getColor() &&
            mShips[j]->getShipState() == Ship::SHIP_ALIVE &&
            mShips[i]->isTouched(mShips[j]->getPosition()))
        {
          mShips[i]->handleShipCollision(mShips[j]);
          // play a sound:
				  OBoy::Environment::instance()->getSoundPlayer()->playSound(mExplosionSound);
          break;
        }
      }
      // collision with asteroids:
      for (int j=0 ; j<(int)mAsteroids.size() ; j++)
      {
        if(mAsteroids[j]->isTouched(mShips[i]->getPosition()))
        {
          mAsteroids[j]->handleShipCollision(mShips[i]);
          // play a sound:
				  OBoy::Environment::instance()->getSoundPlayer()->playSound(mExplosionSound);
          break;
        }
      }
#endif
    }
    else
    {
      if (mShips[i]->getShipState() == Ship::SHIP_REMOVE)
      {
        // destroy dead ships:
        delete *(mShips.begin()+i);
        mShips.erase(mShips.begin()+i);
      }
    }
  }
}

void Macro::updateMenu(float dt)
{
}

void Macro::update(float dt)
{
  callMemberFunction(this, mUpdateFuncs[mGameOver])(dt);
}

void Macro::createBackground()
{
  int numGridCols = 16;
	int numGridRows = 16;
	int xinc = static_cast<int>(mWorldSize.x()/numGridCols);
	int yinc = static_cast<int>(mWorldSize.y()/numGridRows);
	numGridCols--;
	numGridRows--;
  int vertIndex=0;

  mGrid = OBoy::Environment::instance()->createLines((numGridCols+numGridRows)*2+8);
  //mGrid->setColor(0x3300ffff);
  mGrid->setColor(OBoyLib::Color::White);

  // draw grid:
  for (int i = 0; i < numGridCols; i++)
  {
    mGrid->setVertPos(vertIndex++, static_cast<int>((i+1)*xinc), 0);
    mGrid->setVertPos(vertIndex++, static_cast<int>((i+1)*xinc), static_cast<int>(mWorldSize.y()));
  }
  for (int i = 0; i < numGridRows; i++)
  {
    mGrid->setVertPos(vertIndex++, 0, static_cast<int>((i+1)*yinc));
    mGrid->setVertPos(vertIndex++, static_cast<int>(mWorldSize.x()), static_cast<int>((i+1)*yinc));
  }

  // draw border:
  mGrid->setVertPos(vertIndex++, 0, 0);
  mGrid->setVertPos(vertIndex++, static_cast<int>(mWorldSize.x()), 0);

  mGrid->setVertPos(vertIndex++, static_cast<int>(mWorldSize.x()), 0);
  mGrid->setVertPos(vertIndex++, static_cast<int>(mWorldSize.x()), static_cast<int>(mWorldSize.y()));

  mGrid->setVertPos(vertIndex++, static_cast<int>(mWorldSize.x()), static_cast<int>(mWorldSize.y()));
  mGrid->setVertPos(vertIndex++, 0, static_cast<int>(mWorldSize.y()));

  mGrid->setVertPos(vertIndex++, 0, static_cast<int>(mWorldSize.y()));
  mGrid->setVertPos(vertIndex++, 0, 0);

  mGrid->build();
}

void Macro::deleteBackground()
{
  delete(mGrid);
}

void Macro::drawBackground(OBoy::Graphics *g, OBoyLib::Vector2 camera)
{
  g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
  g->setColor(OBoyLib::Color::LightPink);
  g->drawRect(0, 0, static_cast<int>(mWorldSize.x()), static_cast<int>(mWorldSize.y()));
#if 0
  g->setLineWidth(3.0f);
  g->pushTransform();
		g->translate(-camera.x(), -camera.y());
    g->drawLines(mGrid);
	g->popTransform();
  g->setLineWidth(1.0f);
#endif
  g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}

void Macro::drawMenu(OBoy::Graphics *g)
{
  if (mLoadComplete)
	{
		/*OBoy::UString str1("MACROWARS");
		OBoy::UString str2("press ENTER to start");
		float str2scale = 0.8f;
		float x1 = (OBoy::Environment::screenWidth() - mFont->getStringWidth(str1)) / 2.0f;
		float x2 = (OBoy::Environment::screenWidth() - mFont->getStringWidth(str2)*str2scale) / 2.0f;
		g->pushTransform();
			g->translate(x1, OBoy::Environment::screenHeight()/2.0f - 50.0f);
			mFont->drawString(g,str1);
		g->popTransform();
		g->pushTransform();
			g->translate(x2, OBoy::Environment::screenHeight()/2.0f + 50.0f);
			mFont->drawString(g,str2,str2scale);
		g->popTransform();*/
	}

  // draw the gui:
  mGuiSystem->draw();
}

void Macro::drawGame(OBoy::Graphics *g)
{
  // draw the background:
  drawBackground(g, mCamera);

	// draw the asteroids:
	for (int i=0 ; i<(int)mAsteroids.size() ; i++)
	{
		mAsteroids[i]->draw(g, mCamera);
	}

	// draw the planets:
	for (int i=0 ; i<(int)mPlanets.size() ; i++)
	{
		mPlanets[i]->draw(g, mCamera, mDrawMinHp, mMainPlayer);
	}

	// draw the ships:
  if (mDrawShips)
  {
	  for (int i=0 ; i<(int)mShips.size() ; i++)
	  {
		  mShips[i]->draw(g, mCamera);
	  }
  }

  // draw the waypoints:
  if (mMainPlayer != NULL && mDrawWaypoints)
    mMainPlayer->drawWaypoints(g, mCamera);

  // draw the gui:
  mGuiSystem->draw();
}

void Macro::draw(OBoy::Graphics *g)
{
	mDrawCount++;

  callMemberFunction(this, mDrawFuncs[mGameOver])(g);
}

void Macro::keyUp(wchar_t unicode, OBoy::Keyboard::Key key, OBoy::Keyboard::Modifiers mods)
{
  // like before we inject the scancode directly
  CEGUI::System::getSingleton().injectKeyUp(key);

  mKeyPressed[key] = false;
  if (unicode=='P' || unicode=='p')
    mGameSpeed = mGameSpeed > 0 ? 0 : 1.0f;
  if (unicode=='I' || unicode=='i')
    mGameSpeed = 1.0;
  if (unicode=='O' || unicode=='o')
    mGameSpeed = 5.0;
  if (unicode=='F' || unicode=='f')
    mDrawMinHp = !mDrawMinHp;
  if (unicode=='D' || unicode=='d')
    mDrawWaypoints = !mDrawWaypoints;
  if (unicode=='E' || unicode=='e')
  {
    mDrawShips = !mDrawShips;
    mDrawWaypoints = mDrawShips;
    mDrawMinHp = true;
  }
  if (unicode=='H' || unicode=='h')
  {
    OBoy::Graphics *g = OBoy::Environment::instance()->getGraphics();
	  float w = g->getWidth() / 2.0f;
	  float h = g->getHeight() / 2.0f;
    mCamera = mMainPlayer->getCore()->getPosition() - OBoyLib::Vector2(w, h);
  }
  if (unicode==' ') // when spacebar is hit change main player
  {
    for (int i=0 ; i<(int)mPlayers.size() ; i++)
	  {
      if (mMainPlayer->getColor() == mPlayers[i]->getColor())
      {
        i++;
        if (i == mPlayers.size())
          mMainPlayer = mPlayers.front();
        else
          mMainPlayer = mPlayers[i];
        break;
      }
    }
  }
}

void Macro::keyDown(wchar_t unicode, OBoy::Keyboard::Key key, OBoy::Keyboard::Modifiers mods)
{
  // to tell CEGUI that a key was pressed, we inject the scancode
  CEGUI::System::getSingleton().injectKeyDown(key);
  // as for the character it's a litte more complicated. we'll use for translated unicode value.
  if (unicode != 0)
	  CEGUI::System::getSingleton().injectChar(unicode);

  mKeyPressed[key] = true;
}
#ifdef OBOY_PLATFORM_LINUX
OBoyLib::Vector2 getGLPos(OBoyLib::Vector2 pos)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winx, winy, winz;
	GLdouble posx, posy, posz;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	winx = static_cast<GLfloat>(pos.x());
	winy = static_cast<GLfloat>(viewport[3] - pos.y());
	glReadPixels(pos.x(), static_cast<GLint>(winy), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);
	gluUnProject(winx, winy, winz, modelview, projection, viewport, &posx, &posy, &posz);
  return OBoyLib::Vector2(static_cast<float>(posx), static_cast<float>(posy));
}
#endif
void Macro::mouseMove(OBoy::Mouse *mouse)
{
  if (CEGUI::System::getSingleton().injectMousePosition(mouse->getPosition().x(), mouse->getPosition().y()))
    return;

  if (mGameOver)
    return;

  static OBoyLib::Vector2 previousMousePos;
  OBoyLib::Vector2 mousePos = mouse->getPosition() + mCamera;
  mMousePos = mouse->getPosition();

  if (mousePos.x() < 0)
    mousePos.x() = 0;
  else if (mousePos.x() > mWorldSize.x())
    mousePos.x() = mWorldSize.x();
  if (mousePos.y() < 0)
    mousePos.y() = 0;
  else if (mousePos.y() > mWorldSize.y())
    mousePos.y() = mWorldSize.y();

  if (mButtonPressed[OBoy::Mouse::BUTTON_LEFT] || mButtonPressed[OBoy::Mouse::BUTTON_RIGHT])
  {
    if (mButtonPressed[OBoy::Mouse::BUTTON_LEFT] && mPlanetSelected)
    {
      for (int i=0 ; i<(int)mPlanets.size() ; i++)
      {
        if (mPlanets[i]->isSelected())
        {
          mPlanets[i]->changeMinHp(static_cast<int>((previousMousePos.y() - mouse->getPosition().y())), mMainPlayer, mShips);
          break;
        }
      }
    }
    else if (mWaypointSelected == true)
      mMainPlayer->moveWaypoint(mousePos, mPlanets);
    else if (mButtonPressed[OBoy::Mouse::BUTTON_RIGHT] && mArrowSelected == true)
      mMainPlayer->updateProbaArrow(mousePos);
    else if (mButtonPressed[OBoy::Mouse::BUTTON_RIGHT] && mRingSelected == true)
      mMainPlayer->updateProbaRing(mousePos);
    else
    {
      mCamera.x() += (previousMousePos.x()-mouse->getPosition().x())*4.0f;
      mCamera.y() += (previousMousePos.y()-mouse->getPosition().y())*4.0f;
    }
  }
  previousMousePos = mouse->getPosition();
}

void Macro::mouseButtonDown(OBoy::Mouse *mouse, OBoy::Mouse::Button button, int clickCount)
{
#ifdef OBOY_PLATFORM_WIN32 /* Direct3D */
  OBoyLib::Vector2 mousePos = mouse->getPosition() + mCamera;
#else /* OpenGL */
  OBoyLib::Vector2 mousePos = getGLPos(mouse->getPosition()) + mCamera;
#endif

  switch (button)
  {
		case OBoy::Mouse::BUTTON_LEFT:
      if (CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton))
        return;
			if (mButtonPressed[OBoy::Mouse::BUTTON_LEFT] == false)
      {
        for (int i=0 ; i<(int)mPlanets.size() ; i++)
        {
          if (mPlanets[i]->isTouched(mousePos))
          {
            mPlanetSelected = true;
            mPlanets[i]->setSelected(true);
            break;
          }
        }
        if (mDrawWaypoints && !mPlanetSelected)
          mWaypointSelected = mMainPlayer->pickWaypoint(mousePos);
        mButtonPressed[OBoy::Mouse::BUTTON_LEFT] = true;
	    }
			break;

		case OBoy::Mouse::BUTTON_MIDDLE:
      if (CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton))
        return;
			if (mButtonPressed[OBoy::Mouse::BUTTON_MIDDLE] == false)
      {
        if (mDrawWaypoints && mButtonPressed[OBoy::Mouse::BUTTON_MIDDLE] == false)
          mMainPlayer->deleteWaypoint(mousePos);
		    mButtonPressed[OBoy::Mouse::BUTTON_MIDDLE] = true;
	    }
			break;

		case OBoy::Mouse::BUTTON_RIGHT:
      if (CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton))
        return;
			if (mButtonPressed[OBoy::Mouse::BUTTON_RIGHT] == false)
      {
        if (mDrawWaypoints)
        {
          mArrowSelected = mMainPlayer->isArrowTouched(mousePos);
          if (!mArrowSelected)
          {
            mRingSelected = mMainPlayer->isRingTouched(mousePos);
            if (!mRingSelected)
              mWaypointSelected = mMainPlayer->createWaypoint(mousePos, mPlanets, mShips);
          }
        }
		    mButtonPressed[OBoy::Mouse::BUTTON_RIGHT] = true;
	    }
			break;
  }
}

void Macro::mouseButtonUp(OBoy::Mouse *mouse, OBoy::Mouse::Button button)
{
  OBoyLib::Vector2 mousePos = mouse->getPosition() + mCamera;
  switch (button)
  {
		case OBoy::Mouse::BUTTON_LEFT:
      if (CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton))
        return;
			if (mButtonPressed[OBoy::Mouse::BUTTON_LEFT] == true)
      {
        if (mWaypointSelected)
        {
          mMainPlayer->releaseWaypoint(mousePos, mPlanets, mShips);
          mWaypointSelected = false;
        }
        if (mPlanetSelected)
        {
          for (int i=0 ; i<(int)mPlanets.size() ; i++)
            if (mPlanets[i]->isSelected())
              mPlanets[i]->setSelected(false);
          mPlanetSelected = false;
        }
		    mButtonPressed[OBoy::Mouse::BUTTON_LEFT] = false;
	    }
			break;
		case OBoy::Mouse::BUTTON_MIDDLE:
      if (CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton))
        return;
        mButtonPressed[OBoy::Mouse::BUTTON_MIDDLE] = false;
			break;
		case OBoy::Mouse::BUTTON_RIGHT:
      if (CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton))
        return;
      if (mButtonPressed[OBoy::Mouse::BUTTON_RIGHT] == true)
      {
        if (mWaypointSelected)
          mMainPlayer->releaseWaypoint(mousePos, mPlanets, mShips);
        if (mArrowSelected == true)
          mMainPlayer->releaseArrow();
        if (mRingSelected == true)
          mMainPlayer->releaseRing();
        mWaypointSelected = false;
        mArrowSelected = false;
        mRingSelected = false;
        mButtonPressed[OBoy::Mouse::BUTTON_RIGHT] = false;
		  }
			break;
	}
}

void Macro::mouseWheel(OBoy::Mouse *mouse, int wheelDelta)
{
  //mZoomFactor += wheelDelta;
}

void Macro::mouseEnter(OBoy::Mouse *mouse)
{
}

void Macro::mouseLeave(OBoy::Mouse *mouse)
{
}

void Macro::loadRandomMap(const int numPlayers, const int numPlanets, const int numAsteroids)
{
  OBoyLib::Vector2 pos;
  bool found = false;
  int radius;
  int speed;
  float minDistanceBetweenTwoCores = static_cast<float>(sqrt(static_cast<float>(mWorldSize.x()*mWorldSize.x() + mWorldSize.y()*mWorldSize.y()))/(numPlayers+1));
  int minDistanceBetweenTwoPlanets = 140;
  int minDistanceFromBorder = 60;
  OBoyLib::Color::ColorConstant playerColors[] = {OBoyLib::Color::Red, OBoyLib::Color::Blue, OBoyLib::Color::Yellow, OBoyLib::Color::Magenta, OBoyLib::Color::Cyan, OBoyLib::Color::Green};
  for (int i = 0; i < numPlayers; i++)
  {
    Player* player = new Player();
    mPlayers.push_back(player);
    player->setColor(playerColors[i]);
    found = false;
    Core* core = new Core();
    mCores.push_back(core);
    mPlanets.push_back(core);
    core->capture(player);
    while (!found)
    {
      pos.x() = static_cast<float>(rand()%(static_cast<int>(mWorldSize.x() - core->getRadius()) - minDistanceFromBorder));
      pos.y() = static_cast<float>(rand()%(static_cast<int>(mWorldSize.y() - core->getRadius()) - minDistanceFromBorder));
      found = true;
      for (int i=0 ; i<(int)mCores.size() ; i++)
      {
        if (mCores[i]->getDistance(pos) < minDistanceBetweenTwoCores)
        {
          found = false;
          break;
        }
      }
    }
    core->setPosition(pos);
    player->setCore(core);
  }

  for (int i = 0; i < numPlanets; i++)
  {
    Planet* planet = new Planet();
    mPlanets.push_back(planet);
    int proba = rand()%100;
    if (proba < 50)
    {
      speed = 8;
      radius = 20;
    }
    else if (proba < 80)
    {
      speed = 7;
      radius = 25;
    }
    else
    {
      speed = 6;
      radius = 30;
    }
    planet->setRadius(radius);
    planet->setSpeed(speed);
    found = false;
    while (!found)
    {
      pos.x() = static_cast<float>(rand()%(static_cast<int>(mWorldSize.x() - planet->getRadius()) - minDistanceFromBorder));
      pos.y() = static_cast<float>(rand()%(static_cast<int>(mWorldSize.y() - planet->getRadius()) - minDistanceFromBorder));
      found = true;
      for (int i=0 ; i<(int)mPlanets.size() ; i++)
      {
        if (mPlanets[i]->getDistance(pos) < minDistanceBetweenTwoPlanets)
        {
          found = false;
          break;
        }
      }
    }
    planet->setPosition(pos);
  }
  for (int i = 0; i < numAsteroids; i++)
  {
    int proba = rand()%100;
    if (proba < 50)
      radius = 15;
    else if (proba < 80)
      radius = 20;
    else
      radius = 25;
    Asteroid* asteroid = new Asteroid();
    mAsteroids.push_back(asteroid);
    asteroid->setRadius(radius);
    found = false;
    while (!found)
    {
      pos.x() = static_cast<float>(rand()%(static_cast<int>(mWorldSize.x() - asteroid->getRadius()) - minDistanceFromBorder));
      pos.y() = static_cast<float>(rand()%(static_cast<int>(mWorldSize.y() - asteroid->getRadius()) - minDistanceFromBorder));
      found = true;
      for (int i=0 ; i<(int)mPlanets.size() ; i++)
      {
        if (mPlanets[i]->getDistance(pos) < minDistanceBetweenTwoPlanets)
        {
          found = false;
          break;
        }
      }
    }
    asteroid->setPosition(pos);
  }
}

void Macro::endGame()
{
  mGameOver = true;

  OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
  rm->unloadResourceGroup("textures");

  while (mShips.size()>0)
	{
		delete *(mShips.begin());
		mShips.erase(mShips.begin());
	}
	while (mPlayers.size()>0)
	{
		delete *(mPlayers.begin());
		mPlayers.erase(mPlayers.begin());
	}
	while (mPlanets.size()>0)
	{
		delete *(mPlanets.begin());
		mPlanets.erase(mPlanets.begin());
	}
  while (mCores.size()>0)
    mCores.erase(mCores.begin());
	while (mAsteroids.size()>0)
	{
		delete *(mAsteroids.begin());
		mAsteroids.erase(mAsteroids.begin());
	}

  deleteBackground();

  mMainPlayer = NULL;
}

void Macro::newGame()
{
	mGameOver = false;

  mWorldSize.x() = MAP_WIDTH;
	mWorldSize.y() = MAP_HEIGHT;

  OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();

  loadRandomMap(NUM_PLAYERS, NUM_PLANETS, NUM_ASTEROIDS);
#if 0
  for (int i=(int)mPlanets.size()-1 ; i>=0 ; i--)
  {
    std::ostringstream path;
    path << "res/planet_" << i;
    OBoy::CoherentNoise::createSphereHeightMap(path.str(), "planet", 128, 64);
    std::ostringstream id;
    id << "IMAGE_PLANET_" << i;
    rm->addResource(id.str(), "image", path.str(), "textures");
  }

  rm->loadResourceGroup("textures");

  for (int i=(int)mPlanets.size()-1 ; i>=0 ; i--)
  {
    std::ostringstream id;
    id << "IMAGE_PLANET_" << i;
		mPlanets[i]->setImage(id.str());
  }
#endif
	mMainPlayer = mPlayers.front();
  OBoy::Graphics *g = OBoy::Environment::instance()->getGraphics();
	float w = g->getWidth()/2.0f;
	float h = g->getHeight()/2.0f;
  mCamera = mMainPlayer->getCore()->getPosition() - OBoyLib::Vector2(w, h);
  mGameSpeed = 1.0f;
  mMove = OBoyLib::Vector2();
  mMousePos = OBoyLib::Vector2(w, h);
  mDrawShips = true;
  mDrawWaypoints = true;
  mDrawMinHp = true;
  mPlanetSelected = false;

  createBackground();
}

void Macro::pauseGame()
{
  mSavedGameSpeed = mGameSpeed;
  mGameSpeed = 0;
}

void Macro::resumeGame()
{
  mGameSpeed = mSavedGameSpeed;
}