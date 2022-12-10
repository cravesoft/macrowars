#include <OgreSceneNode.h>
#include <OgreTechnique.h>
#include <OgreResourceGroupManager.h>
#include <OgreManualObject.h>
#include <iostream>

#include "game_state.hpp"

#include "asteroid.hpp"
#include "planet.hpp"
#include "player.hpp"

using namespace Ogre;

#define SHIP_ROT_SPEED 200.0f
#define SHIP_ACCELERATION 30.0f
#define SHIP_MAX_SPEED 250.0f
#define SHIP_DAMPENING_FACTOR 1.0f
#define BULLET_SPEED 400.0f
#define INVULNERABILITY_PERIOD 2.5f

#define NUM_STARS 1000
#define MAP_WIDTH 3000
#define MAP_HEIGHT 2500
#define NUM_PLAYERS 1
#define NUM_PLANETS 30
#define NUM_ASTEROIDS 100

#define OSQRT2 (1.41421356f)

#define PROJECTION_Z_NEAR 1
#define PROJECTION_Z_FAR 1000

#define CAM_Z -500

GameState::GameState()
    : move_speed_(5.0f)
    , rot_speed_(0.3f)
    , left_mouse_down_(false)
    , right_mouse_down_(false)
    , quit_(false)
    , settings_mode_(false)
    , game_over_(false)
    , world_size_(MAP_WIDTH, MAP_HEIGHT, 0)
    , main_player_(NULL)
    , load_complete_(false)
    , grid_node_(NULL)
{
    key_pressed_[OIS::KC_UP] = false;
    key_pressed_[OIS::KC_DOWN] = false;
    key_pressed_[OIS::KC_LEFT] = false;
    key_pressed_[OIS::KC_RIGHT] = false;
}

void GameState::enter()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Entering GameState...");

    scene_manager_ = OgreFramework::getSingletonPtr()->root_->createSceneManager(ST_GENERIC, "GameSceneMgr");
    scene_manager_->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    scene_manager_->addRenderQueueListener(OgreFramework::getSingletonPtr()->overlay_system_);

    ray_scene_query_ = scene_manager_->createRayQuery(Ray());
    ray_scene_query_->setQueryMask(OGRE_HEAD_MASK);

    camera_ = scene_manager_->createCamera("GameCamera");
    int w2 = OgreFramework::getSingletonPtr()->viewport_->getActualWidth() >> 1;
    int h2 = OgreFramework::getSingletonPtr()->viewport_->getActualWidth() >> 1;
    camera_->setNearClipDistance(PROJECTION_Z_NEAR);
    camera_->setFarClipDistance(PROJECTION_Z_FAR);
    camera_->setProjectionType(PT_ORTHOGRAPHIC);
    camera_->setOrthoWindow(OgreFramework::getSingletonPtr()->viewport_->getActualWidth(),
                            OgreFramework::getSingletonPtr()->viewport_->getActualHeight());
    camera_->setPosition(Vector3(w2, h2, -CAM_Z));
    camera_->lookAt(Vector3(w2, h2, 0));

    OgreFramework::getSingletonPtr()->viewport_->setCamera(camera_);
    current_node_ = 0;

    buildGUI();

    loadRandomMap(NUM_PLAYERS, NUM_PLANETS, NUM_ASTEROIDS);

    createBackground();

#if 0
    for(int i= planets_.size() - 1; i >= 0; i--)
    {
        std::ostringstream path;
        path << "res/planet_" << i;
        OBoy::CoherentNoise::createSphereHeightMap(path.str(), "planet", 128, 64);
        std::ostringstream id;
        id << "IMAGE_PLANET_" << i;
        rm->addResource(id.str(), "image", path.str(), "textures");
    }

    rm->loadResourceGroup("textures");

    for(int i = planets_.size() - 1; i >= 0; i--)
    {
        std::ostringstream id;
        id << "IMAGE_PLANET_" << i;
        planets_[i]->setImage(id.str());
    }
#endif
    main_player_ = players_.front();
    Ogre::Vector3 camera_pos = main_player_->getCore()->getPosition();
    camera_->setPosition(camera_pos.x, camera_pos.y, -CAM_Z);
    game_speed_ = 1.0f;
    move_ = Ogre::Vector3();
    draw_ships_ = true;
    draw_waypoints_ = true;
    draw_min_hp_ = true;
    planet_selected_ = false;

    createScene();
}

bool GameState::pause()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Pausing GameState...");

    OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().setRootWindow(0);
//    OgreFramework::getSingletonPtr()->gui_renderer_->setTargetSceneManager(0);

    saved_game_speed_ = game_speed_;
    game_speed_ = 0;

    return true;
}

void GameState::resume()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Resuming GameState...");

    buildGUI();

    game_speed_ = saved_game_speed_;

    OgreFramework::getSingletonPtr()->viewport_->setCamera(camera_);

//    OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().setRootWindow(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME"));
    quit_ = false;
}

void GameState::exit()
{
    OgreFramework::getSingletonPtr()->log_->logMessage("Leaving GameState...");

    OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().setRootWindow(0);

    scene_manager_->destroyCamera(camera_);
    scene_manager_->destroyQuery(ray_scene_query_);
    if(scene_manager_)
    {
        OgreFramework::getSingletonPtr()->root_->destroySceneManager(scene_manager_);
    }

    game_over_ = true;

    while(ships_.size()>0)
    {
        delete *(ships_.begin());
        ships_.erase(ships_.begin());
    }

    while(players_.size()>0)
    {
        delete *(players_.begin());
        players_.erase(players_.begin());
    }

    while(planets_.size()>0)
    {
        delete *(planets_.begin());
        planets_.erase(planets_.begin());
    }

    while(cores_.size()>0)
    {
        cores_.erase(cores_.begin());
    }

    while(asteroids_.size()>0)
    {
        delete *(asteroids_.begin());
        asteroids_.erase(asteroids_.begin());
    }

    deleteBackground();

    main_player_ = NULL;
}

void GameState::loadRandomMap(int num_players, int num_planets, int num_asteroids)
{
    Ogre::Vector3 pos = Ogre::Vector3(0, 0, 0);
    bool found = false;
    int radius;
    int speed;
    float distance_between_two_cores = sqrt(world_size_.x * world_size_.x +
                                            world_size_.y * world_size_.y) /
                                        (num_players + 1);
    int min_distance_between_planets = 140;
    int min_distance_from_border = 60;
    const Ogre::ColourValue playerColors[] =
    {
        Ogre::ColourValue(1.0, 0.0, 0.0),
        Ogre::ColourValue(0.0, 1.0, 0.0),
        Ogre::ColourValue(0.0, 0.0, 1.0),
        Ogre::ColourValue(1.0, 1.0, 0.0),
        Ogre::ColourValue(1.0, 0.0, 1.0),
        Ogre::ColourValue(0.0, 1.0, 1.0)
    };
    for(int i = 0; i < num_players; i++)
    {
        Player* player = new Player(scene_manager_);
        players_.push_back(player);
        player->setColor(playerColors[i]);
        found = false;
        while(!found)
        {
            pos.x = rand() % ((int)world_size_.x - 36 - min_distance_from_border);
            pos.y = rand() % ((int)world_size_.y - 36 - min_distance_from_border);
            found = true;
            for(int i = 0 ; i < cores_.size(); i++)
            {
                if(cores_[i]->getDistance(pos) < distance_between_two_cores)
                {
                    found = false;
                    break;
                }
            }
        }
        Core* core = new Core(scene_manager_, 36, pos, 5);
        core->capture(player);
        player->setCore(core);
        cores_.push_back(core);
        planets_.push_back(core);
    }

    for(int i = 0; i < num_planets; i++)
    {
        int proba = rand() % 100;
        if(proba < 50)
        {
            speed = 8;
            radius = 20;
        }
        else if(proba < 80)
        {
            speed = 7;
            radius = 25;
        }
        else
        {
            speed = 6;
            radius = 30;
        }
        found = false;
        while(!found)
        {
            pos.x = rand() % ((int)world_size_.x - radius - min_distance_from_border);
            pos.y = rand() % ((int)world_size_.y - radius - min_distance_from_border);
            found = true;
            for(int i = 0 ; i < planets_.size(); i++)
            {
                if(planets_[i]->getDistance(pos) < min_distance_between_planets)
                {
                    found = false;
                    break;
                }
            }
        }
        std::cout << pos << std::endl;
        planets_.push_back(new Planet(scene_manager_, radius, pos, speed));
    }
    for(int i = 0; i < num_asteroids; i++)
    {
        int proba = rand() % 100;
        if(proba < 50)
        {
            radius = 15;
        }
        else if(proba < 80)
        {
            radius = 20;
        }
        else
        {
            radius = 25;
        }
        found = false;
        while(!found)
        {
            pos.x = rand() % ((int)world_size_.x - 16 - min_distance_from_border);
            pos.y = rand() % ((int)world_size_.y - 16 - min_distance_from_border);
            found = true;
            for(int i = 0 ; i < planets_.size(); i++)
            {
                if(planets_[i]->getDistance(pos) < min_distance_between_planets)
                {
                    found = false;
                    break;
                }
            }
        }
        asteroids_.push_back(new Asteroid(scene_manager_, 16, pos));
    }
}

void GameState::createBackground()
{
    int num_grid_cols = 16;
    int num_grid_rows = 16;
    int xinc = world_size_.x / num_grid_cols;
    int yinc = world_size_.y / num_grid_rows;
    num_grid_cols--;
    num_grid_rows--;
    int vert_index=0;

    grid_object_ =  scene_manager_->createManualObject("GridObject");
    grid_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode("GridNode");

    MaterialPtr grid_material = Ogre::MaterialManager::getSingleton().create("GridMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    grid_material->setReceiveShadows(false);
    grid_material->getTechnique(0)->setLightingEnabled(true);
    grid_material->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
    grid_material->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
    grid_material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);

    grid_object_->begin("GridMaterial", RenderOperation::OT_LINE_LIST);

    // draw grid:
    for(int i = 0; i < num_grid_cols; i++)
    {
        grid_object_->position((i+1)*xinc, 0, 0);
        grid_object_->position((i+1)*xinc, world_size_.y, 0);
    }
    for(int i = 0; i < num_grid_rows; i++)
    {
        grid_object_->position(0, (i+1)*yinc, 0);
        grid_object_->position(world_size_.x, (i+1)*yinc, 0);
    }

    // draw border:
    grid_object_->position(0, 0, 0);
    grid_object_->position(world_size_.x, 0, 0);

    grid_object_->position(world_size_.x, 0, 0);
    grid_object_->position(world_size_.x, world_size_.y, 0);

    grid_object_->position(world_size_.x, world_size_.y, 0);
    grid_object_->position(0, world_size_.y, 0);

    grid_object_->position(0, world_size_.y, 0);
    grid_object_->position(0, 0, 0);

    grid_object_->end();

    grid_node_->attachObject(grid_object_);
}

void GameState::deleteBackground()
{
    if(grid_node_ != NULL)
    {
        scene_manager_->getRootSceneNode()->removeAndDestroyChild("GridNode");
        grid_node_ = NULL;
    }

    if(grid_object_ != NULL)
    {
        scene_manager_->destroyManualObject("GridObject");
        grid_object_ = NULL;
    }
}

void GameState::createScene()
{
    scene_manager_->createLight("Light")->setPosition(75,75,75);

    DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
    pDotSceneLoader->parseDotScene("CubeScene.xml", "General", scene_manager_, scene_manager_->getRootSceneNode());
    delete pDotSceneLoader;

    scene_manager_->getEntity("Cube01")->setQueryFlags(CUBE_MASK);
    scene_manager_->getEntity("Cube02")->setQueryFlags(CUBE_MASK);
    scene_manager_->getEntity("Cube03")->setQueryFlags(CUBE_MASK);

    head_entity_ = scene_manager_->createEntity("OgreHeadEntity", "ogrehead.mesh");
    head_entity_->setQueryFlags(OGRE_HEAD_MASK);
    head_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode("OgreHeadNode");
    head_node_->attachObject(head_entity_);
    head_node_->setPosition(Vector3(0, 0, -25));

    head_mat_ = head_entity_->getSubEntity(1)->getMaterial();
    head_mat_high_ = head_mat_->clone("OgreHeadMatHigh");
    head_mat_high_->getTechnique(0)->getPass(0)->setAmbient(1, 0, 0);
    head_mat_high_->getTechnique(0)->getPass(0)->setDiffuse(1, 0, 0, 0);
}

#if TODO
#ifdef OBOY_PLATFORM_LINUX
Ogre::Vector3 getGLPos(Ogre::Vector3 pos)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winx, winy, winz;
    GLdouble posx, posy, posz;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    winx = static_cast<GLfloat>(pos.x());
    winy = static_cast<GLfloat>(viewport[3] - pos.y());
    glReadPixels(pos.x(), static_cast<GLint>(winy), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);
    gluUnProject(winx, winy, winz, modelview, projection, viewport, &posx, &posy, &posz);
    return Ogre::Vector3(static_cast<float>(posx), static_cast<float>(posy));
}
#endif
#endif

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    key_pressed_[keyEventRef.key] = true;
    if(settings_mode_ == true)
    {
#if 0
        if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_S))
        {
            OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->tray_manager_->getWidget("DisplayModeSelMenu");
            if(pMenu->getSelectionIndex() + 1 < (int)pMenu->getNumItems())
                pMenu->selectItem(pMenu->getSelectionIndex() + 1);
        }

        if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_W))
        {
            OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->tray_manager_->getWidget("DisplayModeSelMenu");
            if(pMenu->getSelectionIndex() - 1 >= 0)
                pMenu->selectItem(pMenu->getSelectionIndex() - 1);
        }
#endif

        CEGUI::GUIContext& context = OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext();
        // to tell CEGUI that a key was pressed, we inject the scancode
        context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
        // as for the character it's a litte more complicated. we'll use for translated unicode value.
        context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_ESCAPE))
    {
        //pushAppState(findByName("PauseState"));
        quit_ = true;
        return true;
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_TAB))
    {
        settings_mode_ = !settings_mode_;

        if(settings_mode_)
            setBufferedMode();
        else
            setUnbufferedMode();
        return true;
    }

    if(settings_mode_ && OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_RETURN) ||
        OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_NUMPADENTER))
    {
        CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)chat_window_->getChild("ChatInputBox");
        CEGUI::MultiLineEditbox *pChatContentBox = (CEGUI::MultiLineEditbox*)chat_window_->getChild("ChatContentBox");
        pChatContentBox->setText(pChatContentBox->getText() + pChatInputBox->getText() + "\n");
        pChatInputBox->setText("");
        //pChatContentBox->setCaratIndex(pChatContentBox->getText().size());
        //pChatContentBox->ensureCaratIsVisible();
    }

    if(!settings_mode_ || (settings_mode_ &&
       !OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_O)))
    {
        OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    }

    return true;
}

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    // like before we inject the scancode directly
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);

    key_pressed_[keyEventRef.key] = false;
    switch(keyEventRef.key)
    {
        case OIS::KC_P:
        {
            game_speed_ = game_speed_ > 0 ? 0 : 1.0f;
            break;
        }
        case OIS::KC_I:
        {
            game_speed_ = 1.0;
            break;
        }
        case OIS::KC_O:
        {
            game_speed_ = 5.0;
            break;
        }
        case OIS::KC_F:
        {
            draw_min_hp_ = !draw_min_hp_;
            break;
        }
        case OIS::KC_D:
        {
            draw_waypoints_ = !draw_waypoints_;
            break;
        }
        case OIS::KC_E:
        {
            draw_ships_ = !draw_ships_;
            draw_waypoints_ = draw_ships_;
            draw_min_hp_ = true;
            break;
        }
        case OIS::KC_H:
        {
            float w = OgreFramework::getSingletonPtr()->viewport_->getActualWidth() / 2.0f;
            float h = OgreFramework::getSingletonPtr()->viewport_->getActualHeight() / 2.0f;
            Ogre::Vector3 camera_pos = main_player_->getCore()->getPosition();
            camera_->setPosition(camera_pos.x, camera_pos.y, -CAM_Z);
            break;
        }
        case OIS::KC_SPACE: // when spacebar is hit change main player
        {
            for(int i = 0 ; i < (int)players_.size() ; i++)
            {
                if(main_player_->getColor() == players_[i]->getColor())
                {
                    i++;
                    if(i == players_.size())
                    {
                        main_player_ = players_.front();
                    }
                    else
                    {
                        main_player_ = players_[i];
                    }
                    break;
                }
            }
            break;
        }
        default:
            ;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel))
    {
        return true;
    }
    if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel))
    {
        return true;
    }

    if(game_over_)
    {
        return true;
    }

    Ogre::Vector3 mouse_pos = Ogre::Vector3(camera_->getPosition().x,
                                            camera_->getPosition().y, 0);

    if(button_pressed_[OIS::MB_Left] || button_pressed_[OIS::MB_Right])
    {
        if(button_pressed_[OIS::MB_Left] && planet_selected_)
        {
            for(int i = 0 ; i < planets_.size(); i++)
            {
                if(planets_[i]->isSelected())
                {
                    planets_[i]->changeMinHp(
                        evt.state.Y.rel,
                        main_player_, ships_);
                    break;
                }
            }
        }
        else if(waypoint_selected_ == true)
        {
            main_player_->moveWaypoint(mouse_pos, planets_);
        }
        else if(button_pressed_[OIS::MB_Right] && arrow_selected_ == true)
        {
            main_player_->updateProbaArrow(mouse_pos);
        }
        else if(button_pressed_[OIS::MB_Right] && ring_selected_ == true)
        {
            main_player_->updateProbaRing(mouse_pos);
        }
    }

#if 0
    if(right_mouse_down_)
    {
        camera_->yaw(Degree(evt.state.X.rel * -0.1f));
        camera_->pitch(Degree(evt.state.Y.rel * -0.1f));
    }
#endif

    return true;
}

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    //if(OgreFramework::getSingletonPtr()->tray_manager_->injectPointerDown(evt, id)) return true;

    switch(id)
    {
        case OIS::MB_Left:
            if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton))
            {
                return true;
            }
            onLeftPressed(evt);
            left_mouse_down_ = true;
            break;
        case OIS::MB_Middle:
            if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonDown(CEGUI::MiddleButton))
            {
                return true;
            }
            onMiddlePressed(evt);
            break;
        case OIS::MB_Right:
            if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonDown(CEGUI::RightButton))
            {
                return true;
            }
            onRightPressed(evt);
            right_mouse_down_ = true;
            break;
    }

    return true;
}

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    Ogre::Vector3 mouse_pos = Ogre::Vector3(evt.state.X.rel, evt.state.Y.rel, 0)
                              + Ogre::Vector3(camera_->getPosition().x,
                                              camera_->getPosition().y, 0);

    switch(id)
    {
        case OIS::MB_Left:
            left_mouse_down_ = false;
            if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton))
            {
                return true;
            }
            if(button_pressed_[OIS::MB_Left] == true)
            {
                if(waypoint_selected_)
                {
                    main_player_->releaseWaypoint(mouse_pos, planets_, ships_);
                    waypoint_selected_ = false;
                }
                if(planet_selected_)
                {
                    for(int i=0 ; i<(int)planets_.size() ; i++)
                    {
                        if(planets_[i]->isSelected())
                        {
                            planets_[i]->setSelected(false);
                        }
                        planet_selected_ = false;
                    }
                }
                button_pressed_[OIS::MB_Left] = false;
            }
            break;
        case OIS::MB_Middle:
            if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonUp(CEGUI::MiddleButton))
            {
                return true;
            }
            button_pressed_[OIS::MB_Middle] = false;
            break;
        case OIS::MB_Right:
            right_mouse_down_ = false;
            if(OgreFramework::getSingletonPtr()->gui_system_->getDefaultGUIContext().injectMouseButtonUp(CEGUI::RightButton))
            {
                return true;
            }
            if(button_pressed_[OIS::MB_Right] == true)
            {
                if(waypoint_selected_)
                {
                    main_player_->releaseWaypoint(mouse_pos, planets_, ships_);
                }
                if(arrow_selected_ == true)
                {
                    main_player_->releaseArrow();
                }
                if(ring_selected_ == true)
                {
                    main_player_->releaseRing();
                }
                waypoint_selected_ = false;
                arrow_selected_ = false;
                ring_selected_ = false;
                button_pressed_[OIS::MB_Right] = false;
            }
            break;
    }
    return true;
}

void GameState::onLeftPressed(const OIS::MouseEvent &evt)
{
#if 0
    if(current_node_)
    {
        current_node_->showBoundingBox(false);
        current_entity_->getSubEntity(1)->setMaterial(head_mat_);
    }

    CEGUI::Vector3f mouse_pos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
    Ogre::Ray mouseRay = camera_->getCameraToViewportRay(mouse_pos.d_x/float(evt.state.width), mouse_pos.d_y/float(evt.state.height));
    //Ogre::Ray mouseRay = camera_->getCameraToViewportRay(OgreFramework::getSingletonPtr()->mouse_->getMouseState().X.abs / float(evt.state.width),
    //    OgreFramework::getSingletonPtr()->mouse_->getMouseState().Y.abs / float(evt.state.height));
    ray_scene_query_->setRay(mouseRay);
    ray_scene_query_->setSortByDistance(true);

    Ogre::RaySceneQueryResult &result = ray_scene_query_->execute();
    Ogre::RaySceneQueryResult::iterator itr;

    for(itr = result.begin(); itr != result.end(); itr++)
    {
        if(itr->movable)
        {
            OgreFramework::getSingletonPtr()->log_->logMessage("MovableName: " + itr->movable->getName());
            current_node_ = scene_manager_->getEntity(itr->movable->getName())->getParentSceneNode();
            OgreFramework::getSingletonPtr()->log_->logMessage("ObjName " + current_node_->getName());
            current_node_->showBoundingBox(true);
            current_entity_ = scene_manager_->getEntity(itr->movable->getName());
            current_entity_->getSubEntity(1)->setMaterial(head_mat_high_);
            break;
        }
    }
#endif

#ifdef OBOY_PLATFORM_WIN32 /* Direct3D */
    Ogre::Vector3 mouse_pos = Ogre::Vector3(
        evt.state.X.abs + camera_->getPosition().x,
        evt.state.Y.abs + camera_->getPosition().y);
#else /* OpenGL */
#if TODO
    Ogre::Vector3 mouse_pos = getGLPos(mouse->getPosition()) + camera_;
#else
    Ogre::Vector3 mouse_pos = Ogre::Vector3(
        evt.state.X.abs + camera_->getPosition().x,
        evt.state.Y.abs + camera_->getPosition().y,
        0);
#endif
#endif

    if(button_pressed_[OIS::MB_Left] == false)
    {
        for(int i = 0 ; i < planets_.size(); i++)
        {
            if(planets_[i]->isTouched(mouse_pos))
            {
                planet_selected_ = true;
                planets_[i]->setSelected(true);
                break;
            }
        }
        if(draw_waypoints_ && !planet_selected_)
        {
            waypoint_selected_ = main_player_->pickWaypoint(mouse_pos);
        }
        button_pressed_[OIS::MB_Left] = true;
    }
}

void GameState::onMiddlePressed(const OIS::MouseEvent &evt)
{
#ifdef OBOY_PLATFORM_WIN32 /* Direct3D */
    Ogre::Vector3 mouse_pos = Ogre::Vector3(
        evt.state.X.abs + camera_->getPosition().x,
        evt.state.Y.abs + camera_->getPosition().y);
#else /* OpenGL */
#if TODO
    Ogre::Vector3 mouse_pos = getGLPos(mouse->getPosition()) + camera_;
#else
    Ogre::Vector3 mouse_pos = Ogre::Vector3(
        evt.state.X.abs + camera_->getPosition().x,
        evt.state.Y.abs + camera_->getPosition().y,
        0);
#endif
#endif

    if (button_pressed_[OIS::MB_Middle] == false)
    {
        if(draw_waypoints_ && button_pressed_[OIS::MB_Middle] == false)
        {
            main_player_->deleteWaypoint(mouse_pos);
        }
        button_pressed_[OIS::MB_Middle] = true;
    }
}

void GameState::onRightPressed(const OIS::MouseEvent &evt)
{
#ifdef OBOY_PLATFORM_WIN32 /* Direct3D */
    Ogre::Vector3 mouse_pos = Ogre::Vector3(
        evt.state.X.abs + camera_->getPosition().x,
        evt.state.Y.abs + camera_->getPosition().y);
#else /* OpenGL */
#if TODO
    Ogre::Vector3 mouse_pos = getGLPos(mouse->getPosition()) + camera_;
#else
    Ogre::Vector3 mouse_pos = Ogre::Vector3(
        evt.state.X.abs + camera_->getPosition().x,
        evt.state.Y.abs + camera_->getPosition().y,
        0);
#endif
#endif

    if(button_pressed_[OIS::MB_Right] == false)
    {
        if(draw_waypoints_)
        {
            arrow_selected_ = main_player_->isArrowTouched(mouse_pos);
            if(!arrow_selected_)
            {
                ring_selected_ = main_player_->isRingTouched(mouse_pos);
                if(!ring_selected_)
                {
                    waypoint_selected_ = main_player_->createWaypoint(mouse_pos, planets_, ships_);
                }
            }
        }
        button_pressed_[OIS::MB_Right] = true;
    }
}

bool GameState::onExitButtonGame(const CEGUI::EventArgs &args)
{
    quit_ = true;
    return true;
}

void GameState::moveCamera()
{
    camera_->moveRelative(translate_vector_);
}

void GameState::getInput()
{
    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_Q))
    {
        translate_vector_.x = -move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_D))
    {
        translate_vector_.x = move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_Z))
    {
        translate_vector_.y = move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_S))
    {
        translate_vector_.y = -move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_U))
    {
        translate_vector_.z = -move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->keyboard_->isKeyDown(OIS::KC_J))
    {
        translate_vector_.z = move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->mouse_->getMouseState().X.abs < 10)
    {
        translate_vector_.x = -move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->mouse_->getMouseState().X.abs >
       OgreFramework::getSingletonPtr()->viewport_->getActualWidth() - 10)
    {
        translate_vector_.x = move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->mouse_->getMouseState().Y.abs < 10)
    {
        translate_vector_.y = move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->mouse_->getMouseState().Y.abs >
       OgreFramework::getSingletonPtr()->viewport_->getActualHeight() - 10)
    {
        translate_vector_.y = -move_scale_;
    }

    if(OgreFramework::getSingletonPtr()->mouse_->getMouseState().buttonDown(OIS::MB_Left) &&
       !planet_selected_ && !waypoint_selected_)
    {
        translate_vector_.x = -OgreFramework::getSingletonPtr()->mouse_->getMouseState().X.rel;
        translate_vector_.y = OgreFramework::getSingletonPtr()->mouse_->getMouseState().Y.rel;
    }
}

void GameState::update(double timeSinceLastFrame)
{
    if(quit_ == true)
    {
        popAppState();
        return;
    }

    move_scale_ = move_speed_ * timeSinceLastFrame;

    translate_vector_ = Vector3::ZERO;

    getInput();
    moveCamera();

    // Update the speed
    if(game_speed_ <= 0)
    {
        return;
    }
    timeSinceLastFrame *= game_speed_;

    // Update players
    for(int i = players_.size() - 1 ; i >= 0; i--)
    {
        players_[i]->update(timeSinceLastFrame);
        if(players_[i]->getPlayerId() == COMPUTER)
        {
            players_[i]->updateStrategy();
        }
    }

    // Update planets
    for(int i = planets_.size() - 1; i >= 0; i--)
    {
        planets_[i]->update(timeSinceLastFrame, ships_);
    }

    // Update ships
    for(int i = ships_.size() - 1; i >= 0 ; i--)
    {
        ships_[i]->update(timeSinceLastFrame);
        if(ships_[i]->getShipState() == Ship::SHIP_ALIVE)
        {
            if(!ships_[i]->isOrbiting() && ships_[i]->hasArrivedAtDestination())
            {
                ships_[i]->getDestination()->handleShipCollision(ships_[i]);
                break;
            }

            // collision with other ships:
            for(int j = 0 ; j < i ; ++j)
            {
                if(ships_[j]->getColor() != ships_[i]->getColor() &&
                   ships_[j]->getShipState() == Ship::SHIP_ALIVE &&
                   ships_[i]->isTouched(ships_[j]->getPosition()))
                {
                    ships_[i]->handleShipCollision(ships_[j]);
                    // play a sound:
                    //OBoy::Environment::instance()->getSoundPlayer()->playSound(explosion_sound_);
                    break;
                }
            }
            // collision with asteroids:
            for(int j = 0 ; j < asteroids_.size(); j++)
            {
                if(asteroids_[j]->isTouched(ships_[i]->getPosition()))
                {
                    asteroids_[j]->handleShipCollision(ships_[i]);
                    // play a sound:
                    //OBoy::Environment::instance()->getSoundPlayer()->playSound(explosion_sound_);
                    break;
                }
            }
        }
        else
        {
            if(ships_[i]->getShipState() == Ship::SHIP_REMOVE)
            {
                // destroy dead ships:
                delete *(ships_.begin()+i);
                ships_.erase(ships_.begin()+i);
            }
        }
    }
}

void GameState::buildGUI()
{
    OgreFramework::getSingletonPtr()->keyboard_->setEventCallback(this);
    OgreFramework::getSingletonPtr()->mouse_->setEventCallback(this);

//    OgreFramework::getSingletonPtr()->gui_renderer_->setTargetSceneManager(scene_manager_);

    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    //main_window_ = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("menu_title.layout");
    ////chat_window_ = CEGUI::WindowManager::getSingleton().getWindow("ChatWnd");
    //CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(main_window_);

#if 0
    CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)main_window_->getChild("ExitButton_Game");
    pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onExitButtonGame, this));

    left_mouse_down_ = right_mouse_down_ = false;
    quit_ = false;
    settings_mode_ = false;

    setUnbufferedMode();
#endif
}

void GameState::setBufferedMode()
{
    CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)main_window_->getChild("ModeCaption");
    pModeCaption->setText("Buffered Input Mode");

    CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)chat_window_->getChild("ChatInputBox");
    pChatInputBox->setText("");
    pChatInputBox->activate();
    pChatInputBox->captureInput();

    CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)main_window_->getChild("ControlsPanel");
    pControlsPanel->setText("[Tab] - To switch between input modes\n\nAll keys to write in the chat box.\n\nPress [Enter] or [Return] to send message.\n\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

void GameState::setUnbufferedMode()
{
    CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)main_window_->getChild("ModeCaption");
    pModeCaption->setText("Unuffered Input Mode");

    CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)main_window_->getChild("ControlsPanel");
    pControlsPanel->setText("[Tab] - To switch between input modes\n\n[W] - Forward\n[S] - Backwards\n[A] - Left\n[D] - Right\n\nPress [Shift] to move faster\n\n[O] - Toggle Overlays\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//void GameState::iteselected_(OgreBites::SelectMenu* menu)
//{
//    switch(menu->getSelectionIndex())
//    {
//    case 0:
//        camera_->setPolygonMode(Ogre::PM_SOLID);break;
//    case 1:
//        camera_->setPolygonMode(Ogre::PM_WIREFRAME);break;
//    case 2:
//        camera_->setPolygonMode(Ogre::PM_POINTS);break;
//    }
//}

void GameState::displayMenuPlayerInterface()
{
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    main_window_ = win_mgr.loadLayoutFromFile("player_interface.layout");

    main_window_->getChild("b_menu")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &GameState::handleDisplayGameMenu,
            this));
    main_window_->subscribeEvent(
        CEGUI::Window::EventKeyDown,
        CEGUI::Event::Subscriber(&GameState::handleOnGameKey, this));

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(main_window_);
}

void GameState::displayMenuGameMenu()
{
#if 0
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    win_mgr.destroyAllWindows();
    main_window_ = win_mgr.loadLayoutFromFile("player_interface.layout");

    main_window_->getChild("b_back")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &GameState::handleReturnToPlayGame,
            this));
    main_window_->getChild("b_start")->subscribeEvent(
        CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(
            &GameState::handleDisplayLoading,
            this));

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(main_window_);
#endif
}

bool GameState::handleOnGameKey(const CEGUI::EventArgs& e)
{
    //if(static_cast<const CEGUI::KeyEventArgs&>(e).scancode == OBoy::Keyboard::KEY_ESCAPE)
    //{
    //    handleDisplayGameMenu(e);
    //    return true;
    //}
    return false;
}

bool GameState::handleDisplayGameMenu(const CEGUI::EventArgs& e)
{
    //game_->pauseGame();
    displayMenuGameMenu();
    return true;
}
