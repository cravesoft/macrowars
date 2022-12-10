#ifndef __APP_STATE_HPP__
#define __APP_STATE_HPP__

#include <OgreFrameListener.h>

#include "ogre_framework.hpp"

class AppState;

class AppStateListener
{
public:
    AppStateListener() {};
    virtual ~AppStateListener() {};

    virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

    virtual AppState* findByName(Ogre::String stateName) = 0;
    virtual void changeAppState(AppState *state) = 0;
    virtual bool pushAppState(AppState* state) = 0;
    virtual void popAppState() = 0;
    virtual void pauseAppState() = 0;
    virtual void shutdown() = 0;
    virtual void popAllAndPushAppState(AppState* state) = 0;
};

class AppState : public OIS::KeyListener, public OIS::MouseListener/*, Ogre::FrameListener*//*, public OgreBites::SdkTrayListener*/
{
public:
    static void create(AppStateListener* parent, const Ogre::String name) {};

    void destroy() { delete this; }

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual bool pause() { return true; }
    virtual void resume() {};
    virtual void update(double timeSinceLastFrame) = 0;

protected:
    AppState(){};

    AppState* findByName(Ogre::String stateName) { return parent_->findByName(stateName); }
    void changeAppState(AppState* state) { parent_->changeAppState(state); }
    bool pushAppState(AppState* state) { return parent_->pushAppState(state); }
    void popAppState() { parent_->popAppState(); }
    void shutdown() { parent_->shutdown(); }
    void popAllAndPushAppState(AppState* state) { parent_->popAllAndPushAppState(state); }

    AppStateListener*   parent_;
    Ogre::Camera*       camera_;
    Ogre::SceneManager* scene_manager_;
    Ogre::FrameEvent    frame_event_;
};

#define DECLARE_APPSTATE_CLASS(T)                                       \
static void create(AppStateListener* parent, const Ogre::String name)   \
{                                                                       \
    T* myAppState = new T();                                            \
    myAppState->parent_ = parent;                                     \
    parent->manageAppState(name, myAppState);                           \
}

#endif
