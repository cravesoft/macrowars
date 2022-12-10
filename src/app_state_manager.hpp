#ifndef __APP_STATE_MANAGER_HPP__
#define __APP_STATE_MANAGER_HPP__

#include "app_state.hpp"

class AppStateManager : public AppStateListener
{
public:
    typedef struct
    {
        Ogre::String name;
        AppState* state;
    } state_info;

    AppStateManager();
    ~AppStateManager();

    void manageAppState(Ogre::String state_name, AppState* state);

    AppState* findByName(Ogre::String state_name);

    void start(AppState* state);
    void changeAppState(AppState* state);
    bool pushAppState(AppState* state);
    void popAppState();
    void pauseAppState();
    void shutdown();
    void popAllAndPushAppState(AppState* state);

protected:
    void init(AppState *state);

    std::vector<AppState*> active_state_stack_;
    std::vector<state_info> states_;
    bool shutdown_;
};

#endif
