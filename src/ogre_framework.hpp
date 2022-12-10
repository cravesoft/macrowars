#ifndef __OGRE_FRAMEWORK_HPP__
#define __OGRE_FRAMEWORK_HPP__

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

class OgreFramework : public Ogre::Singleton<OgreFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
    OgreFramework();
    ~OgreFramework();

    bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0,
                  OIS::MouseListener *mouse_listener = 0);
    void updateOgre(double timeSinceLastFrame);

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    Ogre::Root* root_;
    Ogre::RenderWindow* render_window_;
    Ogre::Viewport* viewport_;
    Ogre::Log* log_;
    Ogre::Timer* timer;

    OIS::InputManager* input_manager_;
    OIS::Keyboard* keyboard_;
    OIS::Mouse* mouse_;

    Ogre::OverlaySystem* overlay_system_;

    CEGUI::OgreRenderer* gui_renderer_;
    CEGUI::System* gui_system_;

private:
    OgreFramework(const OgreFramework&);
    OgreFramework& operator= (const OgreFramework&);

    //Ogre::Overlay* debug_overlay_;
    //Ogre::Overlay* info_overlay_;
};

#endif
