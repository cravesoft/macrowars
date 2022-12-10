#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "waypoint.hpp"
#include "core.hpp"

enum PlayerId
{
    HUMAN,
    COMPUTER
};

enum PlayerStrategy
{
    DEFENSIVE,
    INTERMEDIATE,
    AGGRESSIVE
};

class Player
{
public:
    Player(Ogre::SceneManager *scene_manager);
    ~Player(void);

    // draw all waypoints and draw the probas of the selected parent
#if TODO
    void drawWaypoints(OBoy::Graphics *g, const Ogre::Vector3 camera);
#endif

    // create a new waypoint and make it the selected waypoint
    bool createWaypoint(Ogre::Vector3 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships);

    // find a waypoint in the area defined by the mouse position and choose it as the selected waypoint
    bool pickWaypoint(const Ogre::Vector3 position);

    // update the proba of a ring with the mouse
    bool updateProbaRing(const Ogre::Vector3 position);

    // update the proba of an arrow with the mouse
    bool updateProbaArrow(const Ogre::Vector3 position);

    // check if an arrow is touched
    bool isArrowTouched(const Ogre::Vector3 position);

    // check if a ring is touched
    bool isRingTouched(const Ogre::Vector3 position);

    // move a waypoint with the mouse
    bool moveWaypoint(const Ogre::Vector3 position, std::vector<Planet*> &planets);

    // find a waypoint in the zone defined by the mouse position and delete it
    void deleteWaypoint(const Ogre::Vector3 position);

    // release the selected waypoint
    void releaseWaypoint(const Ogre::Vector3 position, std::vector<Planet*> &planets, std::vector<Ship*>& ships);

    // release the selected arrow
    void releaseArrow();

    // release the selected ring
    void releaseRing();

    // update the player
    void update(const float dt);

    // update the strategy of the player (only for COMPUTER)
    void updateStrategy();

    // get and set the core planet of the player
    inline Core* getCore() const { return player_core_; };
    inline void setCore(Core *core) { player_core_ = core; };

    // get and set the color of the player
    inline Ogre::ColourValue getColor() const { return color_; };
    inline void setColor(const Ogre::ColourValue color) { color_ = color; };

    // get and set the id of the player (HUMAN or COMPUTER)
    inline int getPlayerId() const { return player_id_; };
    inline void setPlayerId(const int id) { player_id_ = id; };

    // get and set the strategy of the player (only for COMPUTER)
    inline int getPlayerStrategy() const { return player_strategy_; };
    inline void setPlayerStrategy(const int type) { player_strategy_ = type; };

private:
    // delete waypoints that do not have any parent or sibling
    void cleanWaypoints();

private:
    Ogre::SceneManager *scene_manager_;
    Ogre::ColourValue color_;
    int player_id_;
    int player_strategy_;
    Core* player_core_;
    Ogre::Vector3 original_mouse_pos_;
    std::vector<Waypoint*> waypoints_;
};

#endif
