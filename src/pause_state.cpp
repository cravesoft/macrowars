#include "pause_state.hpp"

using namespace Ogre;

PauseState::PauseState()
    : quit_(false)
    , question_active_(false)
{
    frame_event_ = Ogre::FrameEvent();
}

void PauseState::enter()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Entering PauseState...");

    scene_manager_ = OgreFramework::getSingletonPtr()->root_->createSceneManager(ST_GENERIC, "PauseSceneMgr");
    scene_manager_->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    //scene_manager_->addRenderQueueListener(OgreFramework::getSingletonPtr()->overlay_system_);

    camera_ = scene_manager_->createCamera("PauseCam");
    camera_->setPosition(Vector3(0, 25, -50));
    camera_->lookAt(Vector3(0, 0, 0));
    camera_->setNearClipDistance(1);

    camera_->setAspectRatio(Real(OgreFramework::getSingletonPtr()->viewport_->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->viewport_->getActualHeight()));

    OgreFramework::getSingletonPtr()->viewport_->setCamera(camera_);

    //OgreFramework::getSingletonPtr()->tray_manager_->destroyAllWidgets();
    //OgreFramework::getSingletonPtr()->tray_manager_->showCursor();
    //OgreFramework::getSingletonPtr()->tray_manager_->createButton(OgreBites::TL_CENTER, "BackToGameBtn", "Return to GameState", 250);
    //OgreFramework::getSingletonPtr()->tray_manager_->createButton(OgreBites::TL_CENTER, "BackToMenuBtn", "Return to Menu", 250);
    //OgreFramework::getSingletonPtr()->tray_manager_->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit AdvancedOgreFramework", 250);
    //OgreFramework::getSingletonPtr()->tray_manager_->createLabel(OgreBites::TL_TOP, "PauseLbl", "Pause mode", 250);

    quit_ = false;

    createScene();
}

void PauseState::createScene()
{
}

void PauseState::exit()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Leaving PauseState...");

    scene_manager_->destroyCamera(camera_);
    if(scene_manager_)
        OgreFramework::getSingletonPtr()->root_->destroySceneManager(scene_manager_);

    //OgreFramework::getSingletonPtr()->tray_manager_->clearAllTrays();
    //OgreFramework::getSingletonPtr()->tray_manager_->destroyAllWidgets();
    //OgreFramework::getSingletonPtr()->tray_manager_->setListener(0);
}

bool PauseState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE) && !question_active_)
    {
        quit_ = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

    return true;
}

bool PauseState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

    return true;
}

bool PauseState::mouseMoved(const OIS::MouseEvent &evt)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerMove(evt)) return true;
    return true;
}

bool PauseState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerDown(evt, id)) return true;
    return true;
}

bool PauseState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerUp(evt, id)) return true;
    return true;
}

void PauseState::update(double timeSinceLastFrame)
{
    frame_event_.timeSinceLastFrame = timeSinceLastFrame;
    //OgreFramework::getSingletonPtr()->tray_manager_->frameRenderingQueued(frame_event_);

    if(quit_ == true)
    {
        popAppState();
        return;
    }
}

//void PauseState::buttonHit(OgreBites::Button *button)
//{
//    if(button->getName() == "ExitBtn")
//    {
//        OgreFramework::getSingletonPtr()->tray_manager_->showYesNoDialog("Sure?", "Really leave?");
//        question_active_ = true;
//    }
//    else if(button->getName() == "BackToGameBtn")
//        quit_ = true;
//    else if(button->getName() == "BackToMenuBtn")
//        popAllAndPushAppState(findByName("MenuState"));
//}

void PauseState::yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit)
{
    if(yesHit == true)
        shutdown();
    //else
        //OgreFramework::getSingletonPtr()->tray_manager_->closeDialog();

    question_active_ = false;
}
