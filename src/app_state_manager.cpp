#include "app_state_manager.hpp"

#include <OgreWindowEventUtilities.h>
#include <OgreTimer.h>

AppStateManager::AppStateManager()
    : shutdown_(false)
{
}

AppStateManager::~AppStateManager()
{
    state_info si;

    while(!active_state_stack_.empty())
    {
        active_state_stack_.back()->exit();
        active_state_stack_.pop_back();
    }

    while(!states_.empty())
    {
        si = states_.back();
        si.state->destroy();
        states_.pop_back();
    }
}

void AppStateManager::manageAppState(Ogre::String state_name, AppState* state)
{
    try
    {
        state_info new_state_info;
        new_state_info.name = state_name;
        new_state_info.state = state;
        states_.push_back(new_state_info);
    }
    catch(std::exception& e)
    {
        delete state;
        throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" + Ogre::String(e.what()), "AppStateManager.cpp (39)");
    }
}

AppState* AppStateManager::findByName(Ogre::String state_name)
{
    for(std::vector<state_info>::iterator itr = states_.begin()
        ; itr != states_.end(); itr++)
    {
        if(itr->name == state_name)
        {
            return itr->state;
        }
    }

    return 0;
}

void AppStateManager::start(AppState* state)
{
    changeAppState(state);

    int timeSinceLastFrame = 1;
    int startTime = 0;

    while(!shutdown_)
    {
        if(OgreFramework::getSingletonPtr()->render_window_->isClosed())
        {
            shutdown_ = true;
        }

        Ogre::WindowEventUtilities::messagePump();

        if(OgreFramework::getSingletonPtr()->render_window_->isActive())
        {
            startTime = OgreFramework::getSingletonPtr()->timer->getMillisecondsCPU();

            OgreFramework::getSingletonPtr()->keyboard_->capture();
            OgreFramework::getSingletonPtr()->mouse_->capture();

            active_state_stack_.back()->update(timeSinceLastFrame);

            OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
            OgreFramework::getSingletonPtr()->root_->renderOneFrame();

            timeSinceLastFrame = OgreFramework::getSingletonPtr()->timer->getMillisecondsCPU() - startTime;
        }
        else
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }

    OgreFramework::getSingletonPtr()->log_->logMessage("Main loop quit");
}

void AppStateManager::changeAppState(AppState* state)
{
    if(!active_state_stack_.empty())
    {
        active_state_stack_.back()->exit();
        active_state_stack_.pop_back();
    }

    active_state_stack_.push_back(state);
    init(state);
    active_state_stack_.back()->enter();
}

bool AppStateManager::pushAppState(AppState* state)
{
    if(!active_state_stack_.empty())
    {
        if(!active_state_stack_.back()->pause())
        {
            return false;
        }
    }

    active_state_stack_.push_back(state);
    init(state);
    active_state_stack_.back()->enter();

    return true;
}

void AppStateManager::popAppState()
{
    if(!active_state_stack_.empty())
    {
        active_state_stack_.back()->exit();
        active_state_stack_.pop_back();
    }

    if(!active_state_stack_.empty())
    {
        init(active_state_stack_.back());
        active_state_stack_.back()->resume();
    }
    else
    {
        shutdown();
    }
}

void AppStateManager::popAllAndPushAppState(AppState* state)
{
    while(!active_state_stack_.empty())
    {
        active_state_stack_.back()->exit();
        active_state_stack_.pop_back();
    }

    pushAppState(state);
}

void AppStateManager::pauseAppState()
{
    if(!active_state_stack_.empty())
    {
        active_state_stack_.back()->pause();
    }

    if(active_state_stack_.size() > 2)
    {
        init(active_state_stack_.at(active_state_stack_.size() - 2));
        active_state_stack_.at(active_state_stack_.size() - 2)->resume();
    }
}

void AppStateManager::shutdown()
{
    shutdown_ = true;
}

void AppStateManager::init(AppState* state)
{
    OgreFramework::getSingletonPtr()->keyboard_->setEventCallback(state);
    OgreFramework::getSingletonPtr()->mouse_->setEventCallback(state);
    //OgreFramework::getSingletonPtr()->tray_manager_->setListener(state);

    OgreFramework::getSingletonPtr()->render_window_->resetStatistics();
}
