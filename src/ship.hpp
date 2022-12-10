#ifndef __SHIP_HPP__
#define __SHIP_HPP__

#include <vector>

#include "body.hpp"
#include "traveller.hpp"
#include "satellite.hpp"

class Player;

typedef struct
{
    bool active; // active (yes/no)
    Ogre::ColourValue life; // particle life
    char fade; // fade speed
    Ogre::ColourValue color; // color
    Ogre::Vector3 pos; // position
    Ogre::Vector3 dir; // direction
#if 0
    OBoy::LineStrip *shape;
#endif
}
Particle; // particles Structure

class Ship : public Traveller, public Satellite
{
public:
    Ship(OriginAndDestination *origin, OriginAndDestination *destination,
         const Player* player, Ogre::Vector3 position,
         Ogre::SceneManager *scene_manager);
    virtual ~Ship(void);

    // draw the ship
#if TODO
    virtual void draw(OBoy::Graphics *g, const Ogre::Vector3 camera);
#endif

    // update the ship
    void update(const float dt);

    // handle collision between two ships
    virtual void handleShipCollision(Ship* ship);

    // move the ship to its destination or orbit around a body
    virtual void move(const float speed);
    void moveAsTraveller(const float speed);
    void moveAsSatellite(const float speed);

    // get and set the ship state
    inline int getShipState() const { return ship_state_; };
    inline void setShipState(const int shipState) { ship_state_ = shipState; };

    // get and set the ship state
    inline const Player* getShipOwner() const { return ship_owner_; };

private:
#if TODO
    void _draw(OBoy::Graphics *g, Ogre::Vector3 &pos);
#endif

public:
    enum ShipState
    {
        SHIP_DEAD,
        SHIP_REMOVE,
        SHIP_ALIVE
    };

private:
    typedef void (Ship::*moveFunc)(const float);

    Ogre::SceneManager *scene_manager_;
    ::std::vector<moveFunc> move_callbacks_;
    int ship_state_;
    int num_particles_;
    Particle* particles_;
    const Player* ship_owner_;
#if OBOY
    OBoy::LineStrip *triangle_;
#else
	Ogre::ManualObject *ship_object_;
	Ogre::SceneNode *ship_node_;
#endif
};

#endif
