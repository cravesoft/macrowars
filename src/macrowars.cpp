#include "macrowars.hpp"

#include "menu_state.hpp"
#include "game_state.hpp"
#include "pause_state.hpp"

Macrowars::Macrowars()
{
    app_state_manager_ = 0;
}

Macrowars::~Macrowars()
{
    delete app_state_manager_;
    delete OgreFramework::getSingletonPtr();
}

void Macrowars::Start()
{
    new OgreFramework();
    if(!OgreFramework::getSingletonPtr()->initOgre("AdvancedOgreFramework", 0, 0))
    {
        return;
    }

    OgreFramework::getSingletonPtr()->log_->logMessage("App initialized!");

    app_state_manager_ = new AppStateManager();

    MenuState::create(app_state_manager_, "MenuState");
    GameState::create(app_state_manager_, "GameState");
    PauseState::create(app_state_manager_, "PauseState");

    app_state_manager_->start(app_state_manager_->findByName("MenuState"));
}
