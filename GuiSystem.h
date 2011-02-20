// Copyright (c) 2010 Olivier Crave. All rights reserved.

#pragma once

#include <CEGUI.h>
#include <string>
#include "OBoy/GuiEventArgs.h"

class Macro;

// class managing the whole GUI
// each event will trigger a handle_XXX , calling a XXX function
class GuiSystem
{
public:

	GuiSystem(Macro *m);
	~GuiSystem(void);

	void draw();

	// close the whole application
	void close();

  // list of handle functions
	bool handleClose(const CEGUI::EventArgs&) { onClic(); close(); return true; }
  bool handleDisplayPlayGame(const CEGUI::EventArgs&) { onClic(); displayPlayGame(); return true; }
  bool handleDisplayGameMenu(const CEGUI::EventArgs&);
  bool handleDisplayMapEditor(const CEGUI::EventArgs&) { onClic(); displayMapEditor(); return true; }
  bool handleDisplayOptions(const CEGUI::EventArgs&) { onClic(); displayOptions(); return true; }
	bool handleDisplayMultiplayer(const CEGUI::EventArgs&) { onClic(); displayMultiplayer(); return true; }
  bool handleStartSinglePlayer(const CEGUI::EventArgs&) { onClic(); displayLoading(); return true; }
  bool handleDisplayPlayerInterface(const CEGUI::EventArgs&) { onClic(); displayPlayerInterface(); return true; }
	bool handleDisplaySingleplayer(const CEGUI::EventArgs&) { onClic(); displaySingleplayer(); return true; }
	bool handleDisplayLoading(const CEGUI::EventArgs&) { onClic(); displayLoading(); return true; }
  bool handleReturnToMain(const CEGUI::EventArgs&) { onClic(); displayMenuTitle(); return true; }
	bool handleReturnToPlayGame(const CEGUI::EventArgs&) { onClic(); displayPlayGame(); return true; }
  bool handleReturnToMultiplayer(const CEGUI::EventArgs&)	{ onClic(); displayMultiplayer();	return true; }
	bool handleReturnToSingleplayer(const CEGUI::EventArgs&) { onClic(); displaySingleplayer();	return true; }
  bool handleReturnToGame(const CEGUI::EventArgs&);
  bool handleExitGame(const CEGUI::EventArgs&);
  bool handleOnMenuKey(const CEGUI::EventArgs&);
  bool handleOnGameKey(const CEGUI::EventArgs&);

	void loadGame();

	// list of all the different menus
	enum Menus
  {
		MAIN_MENU,
		SINGLEPLAYER_MENU,
		MULTIPLAYER_MENU,
	};

private:

  // display functions 
  // all of them are triggered by a handle function	

  // displays the main menu: play game, options, map editor, quit
	void displayMenuTitle();
  // displays the play menu: one player, multiplayer, back
  void displayPlayGame();
  // displays the options menu: back
  void displayOptions();
  // displays the map editor menu
  void displayMapEditor();
  // displays the multiplayer screen: LAN, internet, back
	void displayMultiplayer();
  // displays the single player menu: start, back
	void displaySingleplayer();
  // displays the progress bar screen
	void displayLoading();
  // display the ingame options menu
  void displayGameMenu();
  // displays the player interface
  void displayPlayerInterface();

	void onClic();

  Macro *mGame;
};
