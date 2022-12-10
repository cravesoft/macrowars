#ifndef __GAME_STATE_HPP__
#define __GAME_STATE_HPP__

#include "app_state.hpp"
#include "dot_scene_loader.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

enum QueryFlags
{
    OGRE_HEAD_MASK  = 1<<0,
    CUBE_MASK       = 1<<1
};

class Asteroid;
class Planet;
class Core;
class Ship;
class Player;

class GameState : public AppState
{
public:
    GameState();

    DECLARE_APPSTATE_CLASS(GameState)

    void enter();
    void createScene();
    void exit();
    bool pause();
    void resume();

    void moveCamera();
    void getInput();
    void buildGUI();

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &arg);
    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    void onLeftPressed(const OIS::MouseEvent &evt);
    void onMiddlePressed(const OIS::MouseEvent &evt);
    void onRightPressed(const OIS::MouseEvent &evt);
    bool onExitButtonGame(const CEGUI::EventArgs &args);
    //void iteselected_(OgreBites::SelectMenu* menu);

    void update(double timeSinceLastFrame);

    void setBufferedMode();
    void setUnbufferedMode();

private:
    void displayMenuPlayerInterface();
    void displayMenuGameMenu();

    void loadRandomMap(int num_players, int num_planets, int num_asteroids);
    void createBackground();
    void deleteBackground();

    bool handleOnGameKey(const CEGUI::EventArgs& e);
    bool handleDisplayGameMenu(const CEGUI::EventArgs&);
    //bool handleDisplayLoading(const CEGUI::EventArgs&) { displayMenuLoading(); return true; }

    Ogre::SceneNode*            head_node_;
    Ogre::Entity*               head_entity_;
    Ogre::MaterialPtr           head_mat_;
    Ogre::MaterialPtr           head_mat_high_;

    bool                        quit_;

    Ogre::Vector3               translate_vector_;
    Ogre::Real                  move_speed_;
    Ogre::Degree                rot_speed_;
    float                       move_scale_;
    Ogre::Degree                rot_scale_;

    Ogre::RaySceneQuery*        ray_scene_query_;
    Ogre::SceneNode*            current_node_;
    Ogre::Entity*               current_entity_;
    bool                        left_mouse_down_;
    bool                        right_mouse_down_;
    bool                        settings_mode_;

    CEGUI::Window*              main_window_;
    CEGUI::Window*              chat_window_;

    bool                        game_over_;
    Ogre::Vector3               world_size_;
    std::vector<Asteroid*>      asteroids_;
    std::vector<Planet*>        planets_;
    std::vector<Core*>          cores_;
    std::vector<Ship*>          ships_;
    std::vector<Player*>        players_;
    //OBoy::Lines                *grid_;
	Ogre::ManualObject         *grid_object_;
	Ogre::SceneNode            *grid_node_;
    float                       game_speed_;
    float                       saved_game_speed_;
    Player                     *main_player_;
    bool                        draw_min_hp_;
    bool                        draw_waypoints_;
    bool                        draw_ships_;
    bool                        planet_selected_;
    bool                        arrow_selected_;
    bool                        ring_selected_;
    bool                        waypoint_selected_;
    std::map<OIS::KeyCode, bool> key_pressed_;
    std::map<OIS::MouseButtonID, bool> button_pressed_;
    Ogre::Vector3 move_;
    bool load_complete_;
#if TODO
    OBoy::Sound *boom_sound_;
    OBoy::Sound *explosion_sound_;
    OBoy::Sound *fire_sound_;
    OBoy::Sound *thrust_sound_;
    OBoy::Font *font_;
#endif
};

#endif
