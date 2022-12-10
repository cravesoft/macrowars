#include "menu_state.hpp"

using namespace Ogre;

MenuState::MenuState()
    : quit_(false)
{
    frame_event_ = Ogre::FrameEvent();
}

void MenuState::enter()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Entering MenuState...");

    scene_manager_ = OgreFramework::getSingletonPtr()->root_->createSceneManager(ST_GENERIC, "MenuSceneMgr");
    scene_manager_->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    scene_manager_->addRenderQueueListener(OgreFramework::getSingletonPtr()->overlay_system_);

    camera_ = scene_manager_->createCamera("MenuCam");
    camera_->setPosition(Vector3(0, 25, -50));
    camera_->lookAt(Vector3(0, 0, 0));
    camera_->setNearClipDistance(1);

    camera_->setAspectRatio(Real(OgreFramework::getSingletonPtr()->viewport_->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->viewport_->getActualHeight()));

    OgreFramework::getSingletonPtr()->viewport_->setCamera(camera_);

    //CEGUI::WindowManager &win_mgr = CEGUI::WindowManager::getSingleton();
    //CEGUI::Window *sheet = win_mgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

    //CEGUI::Window *play = win_mgr.createWindow("TaharezLook/Button", "CEGUIDemo/PlayButton");
    //play->setText("Play");
    //play->setSize(CEGUI::USize(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.05, 0)));
    //play->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::play, this));

    //CEGUI::Window *quit = win_mgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
    //quit->setText("Quit");
    //quit->setSize(CEGUI::USize(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    //quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::quit, this));

    //sheet->addChild(play);
    //sheet->addChild(quit);
    //CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

    //OgreFramework::getSingletonPtr()->tray_manager_->destroyAllWidgets();
    //OgreFramework::getSingletonPtr()->tray_manager_->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    //OgreFramework::getSingletonPtr()->tray_manager_->showLogo(OgreBites::TL_BOTTOMRIGHT);
    //OgreFramework::getSingletonPtr()->tray_manager_->showCursor();
    //OgreFramework::getSingletonPtr()->tray_manager_->createButton(OgreBites::TL_CENTER, "EnterBtn", "Enter GameState", 250);
    //OgreFramework::getSingletonPtr()->tray_manager_->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit AdvancedOgreFramework", 250);
    //OgreFramework::getSingletonPtr()->tray_manager_->createLabel(OgreBites::TL_TOP, "MenuLbl", "Menu mode", 250);

    displayMenuTitle();
    createScene();
}

void MenuState::displayMenuTitle()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    main_window_ = win_mgr.loadLayoutFromFile("menu_title.layout");

    main_window_->getChild("b_play")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handlePlayButtonPressed,
            this));
    //main_window_->getChild("b_map_editor")->setEnabled(false);
    main_window_->getChild("b_options")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleDisplayOptions,
            this));
    main_window_->getChild("b_quit")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleQuitButtonPressed,
            this));

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(main_window_);
}

void MenuState::displayMenuPlayGame()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    main_window_ = win_mgr.loadLayoutFromFile("menu_play_game.layout");

    main_window_->getChild("b_one_player")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleDisplaySingleplayer,
            this));
    main_window_->getChild("b_multiplayer")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleDisplayMultiplayer,
            this));
    main_window_->getChild("b_return")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleReturnToMain,
            this));

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(main_window_);
}

void MenuState::displayMenuOptions()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    CEGUI::Window *sheet = win_mgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}

void MenuState::displayMenuMapEditor()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    CEGUI::Window *sheet = win_mgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}

void MenuState::displayMenuMultiplayer()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    CEGUI::Window *sheet = win_mgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}

void MenuState::displayMenuSingleplayer()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    main_window_ = win_mgr.loadLayoutFromFile("singleplayer.layout");

    main_window_->getChild("b_back")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleReturnToPlayGame,
            this));
    main_window_->getChild("b_start")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &MenuState::handleDisplayLoading,
            this));

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(main_window_);
}

void MenuState::displayMenuLoading()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    CEGUI::Window *sheet = win_mgr.createWindow("DefaultWindow", "MacrowarsLook/StaticImage");
    //sheet->setProperty("Image", "set:BackgroundImage image:full_image");
    //sheet->setProperty("FrameEnabled", "false");
    //sheet->setProperty("BackgroundEnabled", "false");
    sheet->addChild(win_mgr.loadLayoutFromFile("loading.layout"));
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
    changeAppState(findByName("GameState"));
}

void MenuState::createScene()
{
}

void MenuState::exit()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Leaving MenuState...");

    scene_manager_->destroyCamera(camera_);
    if(scene_manager_)
        OgreFramework::getSingletonPtr()->root_->destroySceneManager(scene_manager_);

    //OgreFramework::getSingletonPtr()->tray_manager_->clearAllTrays();
    //OgreFramework::getSingletonPtr()->tray_manager_->destroyAllWidgets();
    //OgreFramework::getSingletonPtr()->tray_manager_->setListener(0);
}

bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
    {
        quit_ = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

bool MenuState::mouseMoved(const OIS::MouseEvent &evt)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerMove(evt)) return true;
	OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel);
	OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
    return true;
}

bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerDown(evt, id)) return true;
	OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
    return true;
}

bool MenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerUp(evt, id)) return true;
	OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
    return true;
}

void MenuState::update(double timeSinceLastFrame)
{
    frame_event_.timeSinceLastFrame = timeSinceLastFrame;
    //OgreFramework::getSingletonPtr()->tray_manager_->frameRenderingQueued(frame_event_);

    if(quit_ == true)
    {
        shutdown();
        return;
    }
}

bool MenuState::onExitButton(const CEGUI::EventArgs &args)
{
	quit_ = true;
	return true;
}

bool MenuState::onEnterButton(const CEGUI::EventArgs &args)
{
	this->pushAppState(findByName("GameState"));
	return true;
}

bool MenuState::handlePlayButtonPressed(const CEGUI::EventArgs &e)
{
    displayMenuPlayGame();
    return true;
    //changeAppState(findByName("GameState"));
}

bool MenuState::handleQuitButtonPressed(const CEGUI::EventArgs &e)
{
    quit_ = true;
}

//void MenuState::buttonHit(OgreBites::Button *button)
//{
//    if(button->getName() == "ExitBtn")
//        quit_ = true;
//    else if(button->getName() == "EnterBtn")
//        changeAppState(findByName("GameState"));
//}
