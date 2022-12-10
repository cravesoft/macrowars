#ifndef __MENU_STATE_HPP__
#define __MENU_STATE_HPP__

#include "app_state.hpp"

class MenuState : public AppState
{
public:
    MenuState();

    DECLARE_APPSTATE_CLASS(MenuState)

    void enter();
    void createScene();
    void exit();

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    //void buttonHit(OgreBites::Button* button);
	bool onExitButton(const CEGUI::EventArgs &args);
	bool onEnterButton(const CEGUI::EventArgs &args);

    void update(double timeSinceLastFrame);

private:
    void displayMenuTitle();
    void displayMenuPlayGame();
    void displayMenuOptions();
    void displayMenuMapEditor();
    void displayMenuMultiplayer();
    void displayMenuSingleplayer();
    void displayMenuLoading();

    void loadGame();

    // list of handle functions
    bool handlePlayButtonPressed(const CEGUI::EventArgs &e);
    bool handleQuitButtonPressed(const CEGUI::EventArgs &e);
    //bool handleDisplayMapEditor(const CEGUI::EventArgs&) { displayMapEditor(); return true; }
    bool handleDisplayOptions(const CEGUI::EventArgs&) { displayMenuOptions(); return true; }
    bool handleDisplayMultiplayer(const CEGUI::EventArgs&) { displayMenuMultiplayer(); return true; }
    //bool handleStartSinglePlayer(const CEGUI::EventArgs&) { displayMenuLoading(); return true; }
    //bool handleDisplayPlayerInterface(const CEGUI::EventArgs&) { displayMenuPlayerInterface(); return true; }
    bool handleDisplaySingleplayer(const CEGUI::EventArgs&) { displayMenuSingleplayer(); return true; }
    bool handleDisplayLoading(const CEGUI::EventArgs&) { displayMenuLoading(); return true; }
    bool handleReturnToMain(const CEGUI::EventArgs&) { displayMenuTitle(); return true; }
    bool handleReturnToPlayGame(const CEGUI::EventArgs&) { displayMenuPlayGame(); return true; }
    //bool handleReturnToMultiplayer(const CEGUI::EventArgs&) { displayMultiplayer(); return true; }
    //bool handleReturnToSingleplayer(const CEGUI::EventArgs&) { displaySingleplayer(); return true; }

    bool quit_;
    CEGUI::Window* main_window_;
};

#endif
