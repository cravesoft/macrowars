#ifndef __MACROWARS_HPP__
#define __MACROWARS_HPP__

#include "ogre_framework.hpp"
#include "app_state_manager.hpp"

class Macrowars
{
public:
    Macrowars();
    ~Macrowars();

    void Start();

private:
    AppStateManager* app_state_manager_;
};

#endif
