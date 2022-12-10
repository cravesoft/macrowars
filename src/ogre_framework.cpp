#include <OgreTextureManager.h>
#include <OgreTimer.h>
#include <CEGUI/DefaultResourceProvider.h>

#include "ogre_framework.hpp"

using namespace Ogre;

template<> OgreFramework* Ogre::Singleton<OgreFramework>::msSingleton = 0;

OgreFramework::OgreFramework()
    : root_(0)
    , render_window_(0)
    , viewport_(0)
    , log_(0)
    , timer(0)
    , input_manager_(0)
    , keyboard_(0)
    , mouse_(0)
{
}

OgreFramework::~OgreFramework()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Shutdown OGRE...");
    if(input_manager_)
    {
        OIS::InputManager::destroyInputSystem(input_manager_);
    }
    if(root_)
    {
        delete root_;
    }
}

bool OgreFramework::initOgre(Ogre::String wndTitle,
                             OIS::KeyListener *pKeyListener,
                             OIS::MouseListener *mouse_listener)
{
    Ogre::LogManager* logMgr = new Ogre::LogManager();

    log_ = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
    log_->setDebugOutputEnabled(true);

    root_ = new Ogre::Root();

    if(!(root_->restoreConfig() || root_->showConfigDialog()))
    {
        return false;
    }
    render_window_ = root_->initialise(true, wndTitle);

    viewport_ = render_window_->addViewport(0);
    viewport_->setBackgroundColour(ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

    viewport_->setCamera(0);

    size_t hWnd = 0;
    OIS::ParamList paramList;
    render_window_->getCustomAttribute("WINDOW", &hWnd);

    // This is useful for non fullscreen mode, especially when debugging.
#if VE_PLATFORM == VE_PLATFORM_LINUX
    paramList.insert({"x11_mouse_grab",     "false"});
    paramList.insert({"x11_mouse_hide",     "false"});
    paramList.insert({"x11_keyboard_grab",  "false"});
    paramList.insert({"XAutoRepeatOn",      "true" });
#elif VE_PLATFORM == VE_PLATFORM_WIN32
    paramList.insert({"w32_mouse",    "DISCL_FOREGROUND"  });
    paramList.insert({"w32_mouse",    "DISCL_NONEXCLUSIVE"});
    paramList.insert({"w32_keyboard", "DISCL_FOREGROUND"  });
    paramList.insert({"w32_keyboard", "DISCL_NONEXCLUSIVE"});
#endif

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    input_manager_ = OIS::InputManager::createInputSystem(paramList);

    mouse_ = static_cast<OIS::Mouse*>(input_manager_->createInputObject(OIS::OISMouse, true));
    mouse_->getMouseState().height = render_window_->getHeight();
    mouse_->getMouseState().width = render_window_->getWidth();

    keyboard_ = static_cast<OIS::Keyboard*>(input_manager_->createInputObject(OIS::OISKeyboard, true));

    if(pKeyListener == 0)
    {
        keyboard_->setEventCallback(this);
    }
    else
    {
        keyboard_->setEventCallback(pKeyListener);
    }

    if(mouse_listener == 0)
    {
        mouse_->setEventCallback(this);
    }
    else
    {
        mouse_->setEventCallback(mouse_listener);
    }

    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while(seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for(i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    overlay_system_ = new Ogre::OverlaySystem();

    timer = new Ogre::Timer();
    timer->reset();

/*
    gui_renderer_ = new CEGUI::OgreCEGUIRenderer(render_window_, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, 0);
    gui_system_ = new CEGUI::System(gui_renderer_);
*/
    gui_renderer_ = &CEGUI::OgreRenderer::bootstrapSystem();
    gui_system_ = CEGUI::System::getSingletonPtr();

    CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

    // Select the skin
    CEGUI::SchemeManager::getSingleton().createFromFile("MacrowarsLook.scheme");

    // Set default mouse cursor image
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("MacrowarsLook/MouseArrow");

    //CEGUI::SchemeManager::getSingleton().createFromFile("AlfiskoSkinSkin.scheme");
    //CEGUI::WindowManager::getSingleton().loadLayoutFromFile("AdvancedOgreFramework.layout");
    //CEGUI::WindowManager::getSingleton().loadLayoutFromFile("AdvancedOgreFramework_Game.layout");

    render_window_->setActive(true);

    return true;
}

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(keyboard_->isKeyDown(OIS::KC_SYSRQ))
    {
        render_window_->writeContentsToTimestampedFile("AOF_Screenshot_", ".jpg");
        return true;
    }

    if(keyboard_->isKeyDown(OIS::KC_O))
    {
#if 0
        if(tray_manager_->isLogoVisible())
        {
            tray_manager_->hideFrameStats();
            tray_manager_->hideLogo();
        }
        else
        {
            tray_manager_->showFrameStats(OgreBites::TL_BOTTOMLEFT);
            tray_manager_->showLogo(OgreBites::TL_BOTTOMRIGHT);
        }
#endif
        //if(debug_overlay_)
        //{
        //    if(!debug_overlay_->isVisible())
        //        debug_overlay_->show();
        //    else
        //        debug_overlay_->hide();
        //}
    }

    return true;
}

bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return true;
}

bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
    return true;
}

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return true;
}

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return true;
}

void OgreFramework::updateOgre(double timeSinceLastFrame)
{
    CEGUI::System::getSingleton().injectTimePulse(timeSinceLastFrame);
}
