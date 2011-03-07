// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "GuiSystem.h"

#include "OBoy/Gui.h"
#include "OBoy/GuiWindow.h"
#include "OBoy/Environment.h"
#include "Macro.h"

GuiSystem::GuiSystem(Macro *m)
{
  // load the gui resource groups:
  OBoy::Gui *gui = OBoy::Environment::instance()->getGui();
	gui->setResourceGroupDirectory("schemes", "res/schemes/");
	gui->setResourceGroupDirectory("imagesets", "res/imagesets/");
	gui->setResourceGroupDirectory("fonts", "res/fonts/");
	gui->setResourceGroupDirectory("layouts", "res/layouts/");
	gui->setResourceGroupDirectory("looknfeels", "res/looknfeel/");

  // set the default resource groups to be used
  gui->setDefaultImagesetResourceGroup("imagesets");
  gui->setDefaultFontResourceGroup("fonts");
  gui->setDefaultSchemeResourceGroup("schemes");
  gui->setDefaultWidgetLookManagerResourceGroup("looknfeels");
  gui->setDefaultWindowManagerResourceGroup("layouts");

  // load a skin for the gui:
  gui->loadSkin("MacroWarsLook.scheme");
	
  // load fonts for the gui
  // the first font loaded automatically becomes the default font
  gui->loadFont("Commonwealth-10.font", 10, true, "Commonv2c.ttf");
  gui->loadFont("DejaVuSans-18.font", 18, true, "DejaVuSans.ttf");
  gui->loadFont("DejaVuSans-10.font", 10, true, "DejaVuSans.ttf");
  gui->loadFont("ZeroesThree-40.font", 40, true, "ZeroesThree.ttf");
  gui->loadFont("Tahoma-40.font", 40, true, "Tahoma.ttf");

  gui->setDefaultFont("DejaVuSans-10");
	gui->setDefaultMouseCursor("MacroWarsLook", "MouseArrow");

  displayMenuTitle();

  mGame = m;
}

GuiSystem::~GuiSystem(void)
{
}

// thread safe render function
// it displays the GUI
void GuiSystem::draw()
{
  OBoy::Gui *gui = OBoy::Environment::instance()->getGui();
	gui->draw();
}

void GuiSystem::close()
{
  OBoy::Environment::instance()->stopMainLoop();
}

void GuiSystem::onClic()
{
}

void GuiSystem::displayMenuTitle()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
  CEGUI::Window* menuTitle = winMgr.loadWindowLayout("menu_title.layout");
	winMgr.getWindow("b_play_game")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleDisplayPlayGame, this));
  winMgr.getWindow("b_map_editor")->setEnabled(false);
  winMgr.getWindow("b_options")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleDisplayOptions, this));
  winMgr.getWindow("b_quit")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleClose, this));
  CEGUI::System::getSingleton().setGUISheet(menuTitle);
}

void GuiSystem::displayPlayGame()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
  CEGUI::Window* menuPlayGame = winMgr.loadWindowLayout("menu_play_game.layout");
  winMgr.getWindow("b_one_player")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleDisplaySingleplayer, this));	
  winMgr.getWindow("b_multiplayer")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleDisplayMultiplayer, this));
  winMgr.getWindow("b_return")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleReturnToMain, this));
  CEGUI::System::getSingleton().setGUISheet(menuPlayGame);
}

void GuiSystem::displayOptions()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
  CEGUI::Window* background = winMgr.createWindow("DefaultWindow");
  CEGUI::System::getSingleton().setGUISheet(background);
}

void GuiSystem::displayMapEditor()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
  CEGUI::Window* background = winMgr.createWindow("DefaultWindow");
  CEGUI::System::getSingleton().setGUISheet(background);
}

void GuiSystem::displayMultiplayer()
{
}

void GuiSystem::displaySingleplayer()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
	CEGUI::Window* singlePlayer = winMgr.loadWindowLayout("singleplayer.layout");
  winMgr.getWindow("b_back")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleReturnToPlayGame, this));
	winMgr.getWindow("b_start")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleDisplayLoading, this));
  CEGUI::System::getSingleton().setGUISheet(singlePlayer);
}

void GuiSystem::displayLoading()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
	CEGUI::Window* background = winMgr.createWindow("MacroWarsLook/StaticImage");
  background->setProperty("Image", "set:BackgroundImage image:full_image");
  background->setProperty("FrameEnabled", "false");
  background->setProperty("BackgroundEnabled", "false");
	background->addChildWindow(winMgr.loadWindowLayout("loading.layout"));
	CEGUI::System::getSingleton().setGUISheet(background);
  loadGame();
}

void GuiSystem::displayPlayerInterface()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyAllWindows();
  CEGUI::Window* playerInterface = winMgr.loadWindowLayout("player_interface.layout");
  winMgr.getWindow("b_menu")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleDisplayGameMenu, this));
  playerInterface->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&GuiSystem::handleOnGameKey, this));
	CEGUI::System::getSingleton().setGUISheet(playerInterface);
}

void GuiSystem::displayGameMenu()
{
  CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
  winMgr.destroyAllWindows();

  CEGUI::Window* background = winMgr.createWindow("DefaultWindow");
  background->setWantsMultiClickEvents(false);

  CEGUI::Window* playerInterface = winMgr.loadWindowLayout("player_interface.layout");
  playerInterface->setWantsMultiClickEvents(false);
  background->addChildWindow(playerInterface);

  CEGUI::Window* menuGame = winMgr.loadWindowLayout("menu_game.layout");
  menuGame->setWantsMultiClickEvents(false);
  background->addChildWindow(menuGame);

  winMgr.getWindow("b_back")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleReturnToGame, this));
  winMgr.getWindow("b_exit")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiSystem::handleExitGame, this));
  background->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&GuiSystem::handleOnMenuKey, this));
  CEGUI::System::getSingleton().setGUISheet(background);
}

bool GuiSystem::handleReturnToGame(const CEGUI::EventArgs&)
{
  onClic();
  mGame->resumeGame();
  displayPlayerInterface();
  return true;
}

bool GuiSystem::handleOnMenuKey(const CEGUI::EventArgs& e)
{
  if (static_cast<const CEGUI::KeyEventArgs&>(e).scancode == OBoy::Keyboard::KEY_ESCAPE)
  {
    handleReturnToGame(e);
    return true;
  }
  return false;
}

bool GuiSystem::handleOnGameKey(const CEGUI::EventArgs& e)
{
  if (static_cast<const CEGUI::KeyEventArgs&>(e).scancode == OBoy::Keyboard::KEY_ESCAPE)
  {
    handleDisplayGameMenu(e);
    return true;
  }
  return false;
}

bool GuiSystem::handleDisplayGameMenu(const CEGUI::EventArgs& e)
{
  onClic();
  mGame->pauseGame();
  displayGameMenu();
  return true;
}

bool GuiSystem::handleExitGame(const CEGUI::EventArgs&)
{
  onClic();
  mGame->endGame();
  displayMenuTitle();
  return true;
}

void GuiSystem::loadGame()
{
  onClic();
  mGame->newGame();
  displayPlayerInterface();
}