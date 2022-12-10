#ifndef __WAYPOINT_HPP__
#define __WAYPOINT_HPP__

#include <map>

#include "orbitable.hpp"
#include "origin_and_destination.hpp"
#include "ship.hpp"
#include "planet.hpp"
//#include "font_texture_helper.hpp"

class Planet;

class Waypoint : public OriginAndDestination, public Orbitable
{
public:
    Waypoint(Ogre::SceneManager *scene_manager);
    explicit Waypoint(Ogre::Vector3);
    virtual ~Waypoint(void);

#if TODO
    void draw(OBoy::Graphics *g, const Ogre::Vector3 camera);
    void drawProbas(OBoy::Graphics *g, const Ogre::Vector3 camera);
#endif

    virtual bool isTouched(const Ogre::Vector3);
    bool isRingTouched(const Ogre::Vector3);
    bool isArrowTouched(const Ogre::Vector3 pos, Waypoint* &parent);

    void addProbaArrow(Waypoint*);
    void removeProbaArrow(Waypoint*);
    void changeProbaArrow(int, Waypoint*);
    void updateProbasArrow(int, Waypoint*);

    void changeProbaRing(const int proba);

    void addParent(Waypoint*);
    void addSibling(Waypoint*);
    void addSibling(Waypoint*, const int proba);

    void addParents(std::vector<Waypoint*>&);
    void addSiblings(std::vector<Waypoint*>&);

    void removeParent(Waypoint*);
    void removeSibling(Waypoint*);

    void removeAllParents();
    void removeAllSiblings();

    void mergeWaypoint(Waypoint*);

    virtual void handleShipCollision(Ship*);

    void setDefaultRadius();
    virtual void setRadius(const int radius);

    Waypoint* getShipDestination();

    inline std::vector<Waypoint*>& getParents() { return parents_; };
    inline std::vector<Waypoint*>& getSiblings() { return siblings_; };

    inline Planet* getPlanet() { return planet_; };
    inline void setPlanet(Planet* planet) { planet_ = planet; };

    inline int getProbaRing() const { return proba_ring_; };
    inline int getProbaArrow(Waypoint* sibling) { return probas_[sibling]; };

    inline bool isSelected() const { return selected_; };
    inline void setSelected(const bool selected) { selected_ = selected; };

    inline bool isSelectedAsParent() const { return selected_as_parent_; };
    inline void setSelectedAsParent(const bool selected) { selected_as_parent_ = selected; };

    inline bool getDrawRing() const { return draw_ring_; };
    inline void setDrawRing(const bool draw) { draw_ring_ = draw; };

private:
#if TODO
    void drawArrow(OBoy::Graphics *g, const float length, const Ogre::ColourValue color);
#endif
#if 0
    void createArrow(Waypoint* sibling);
    void deleteArrow(Waypoint* sibling);
    void updateArrow(Waypoint* sibling, const float length, const Ogre::ColourValue color);
#endif
private:
    Ogre::SceneManager *scene_manager_;
    std::map<Waypoint*, int> probas_;
    int proba_ring_;
    bool draw_ring_;
    bool selected_;
    bool selected_as_parent_;
    std::vector<Waypoint*> parents_;
    std::vector<Waypoint*> siblings_;
    Planet* planet_;
    //OBoy::Font *font_;
    //std::map<Waypoint*, OBoy::TriStrip*> arrow_tail_tri_strips_;
    //std::map<Waypoint*, OBoy::TriStrip*> arrow_head_tri_strips_;
    //std::map<Waypoint*, OBoy::LineStrip*> arrow_line_strips_;
    //OBoy::TriStrip* ring_strip_;
	Ogre::ManualObject *ring_object_;
	Ogre::SceneNode *ring_node_;
};

#endif
