#ifndef __PLANET_HPP__
#define __PLANET_HPP__

#include <vector>
#include <map>
#include <OgreImage.h>

#include "origin_and_destination.hpp"
#include "waypoint.hpp"
#include "ship.hpp"

class Player;
class Waypoint;

class Planet : public OriginAndDestination
{
public:
    Planet(Ogre::SceneManager *scene_manager, int radius, const Ogre::Vector3 &pos, int speed);
    virtual ~Planet(void);

    void init(int radius, const Ogre::Vector3 &pos, int speed);
#if TODO
    virtual void draw(OBoy::Graphics *g, const Ogre::Vector3 camera);
    void draw(OBoy::Graphics *g, const Ogre::Vector3 &camera, const bool drawMinHp, const Player* player);
#endif

    float getDistance(const Ogre::Vector3);
    void update(const float dt, std::vector<Ship*>& ships);
    void capture(const Player* player);
    void changeMinHp(const int diffHp, const Player* player, std::vector<Ship*>& ships);
    virtual void handleShipCollision(Ship* ship);

    int releaseShips(std::vector<Ship*>& ships);
    void launchShip(std::vector<Ship*>& ships);

    virtual void setRadius(const int radius);
#if 0
    virtual void setImage(const ::std::string &id);
#endif
    inline void setSpeed(const int speed) { speed_ = speed; };

    inline bool isSelected() const { return selected_; };
    inline void setSelected(const bool selected) { selected_ = selected; };

    inline void setWaypoint(Waypoint* waypoint, const Player* player) { waypoints_[player] = waypoint; };

private:
#if TODO
    void _draw(OBoy::Graphics *g, const Ogre::Vector3 &pos);
    void _drawMinHp(OBoy::Graphics *g, const Ogre::Vector3 &pos, const Player* player);
    void _drawText(OBoy::Graphics *g, const Ogre::Vector3 &pos, const Player* player);
#endif
    void updateHp(int delta);
    void release();

protected:
    int speed_;
    Ogre::SceneNode *scene_node_;
    Ogre::Entity *entity_;
    Ogre::Image image_;

private:
    static const std::string filename_;
    Ogre::SceneManager *scene_manager_;
    int hp_;
    float rot_[3];
    float rot_vel_[3];
    std::map<const Player*, int> min_hp_;
    const Player* planet_owner_;
    float ttl_;
    bool captured_;
    //OBoy::Font *font_;
    //OBoy::Sound *exposion_sound_;
    std::map<const Player*, Waypoint*> waypoints_;
    bool selected_;
#if 0
    OBoy::Sphere* sphere_;
#endif
#if OBOY
    OBoy::TriStrip* min_hp_Strip;
    OBoy::TriStrip* current_hp_strip_;
#else
	Ogre::ManualObject *min_hp_object_;
	Ogre::SceneNode *min_hp_node_;
	Ogre::ManualObject *current_hp_object_;
	Ogre::SceneNode *current_hp_node_;
#endif
};

#endif
