#include <OgreMaterialManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTechnique.h>
#include <OgreEntity.h>

#include "log.hpp"
#include "planet.hpp"
#include "player.hpp"

#define PLANET_DEFAULT_RADIUS 30
#define PLANET_DEFAULT_SPEED 5
#define MAX_HP 1000

const std::string Planet::filename_ = "planet.png";

Planet::Planet(Ogre::SceneManager *scene_manager, int radius,
               const Ogre::Vector3 &pos, int speed)
    : scene_manager_(scene_manager)
{
    init(radius, pos, speed);
}

Planet::~Planet(void)
{
#if 0
    delete sphere_;
#endif
#if OBOY
    delete min_hp_Strip;
    delete current_hp_strip_;
#else
	if(current_hp_node_ != NULL)
	{
		//scene_manager_->getRootSceneNode()->removeAndDestroyChild(current_hp_node_);
		delete current_hp_node_;
		current_hp_node_ = NULL;
	}

	if(current_hp_object_ != NULL)
	{
		scene_manager_->destroyManualObject(current_hp_object_);
		current_hp_object_ = NULL;
	}
	if(min_hp_node_ != NULL)
	{
		//scene_manager_->getRootSceneNode()->removeAndDestroyChild(min_hp_node_);
		delete min_hp_node_;
		min_hp_node_ = NULL;
	}

	if(min_hp_object_ != NULL)
	{
		scene_manager_->destroyManualObject(min_hp_object_);
		min_hp_object_ = NULL;
	}
#endif
}

void Planet::init(int radius, const Ogre::Vector3 &pos, int speed)
{
    color_ = Ogre::ColourValue::White;
    radius_ = radius;
    speed_ = speed;
    pos_ = pos;
    captured_ = false;
    selected_ = false;
    ttl_ = 0;
    hp_ = 0;
    entity_ = scene_manager_->createEntity(Ogre::SceneManager::PT_SPHERE);
    scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
    scene_node_->attachObject(entity_);
    scene_node_->setPosition(pos_);
    float scale_factor = (float)radius_ / 100;
    scene_node_->setScale(Ogre::Vector3(scale_factor, scale_factor, scale_factor));
    //image_ = OBoy::Environment::getImage("IMAGE_PLANET");
    try
    {
        image_.load(filename_,
                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    }
    catch(Ogre::Exception &e)
    {
        LOG("Error loading blendmap: " + filename_ + " : " + e.getFullDescription());
        return;
    }
    for(int i = 0; i < 3; i++)
    {
        rot_[i] = rand() % 360;
        if(!(rand()%2))
        {
            rot_vel_[i] = rand() % 100 / 1000;
        }
        else
        {
            rot_vel_[i] = -rand() % 100 / 1000;
        }
    }
#if TODO
    OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
    font_ = rm->getFont("FONT_MAIN");
    exposion_sound_ = rm->getSound("SOUND_EXPLOSION");
#endif
#if 0
    sphere_ = OBoy::Environment::instance()->createSphere((float)radius_, 20, 20);
    sphere_->build();
#endif
#if OBOY
    // graphics:
    min_hp_Strip = OBoy::Environment::instance()->createTriStrip(722);
    min_hp_Strip->setColor(Ogre::ColourValue::Red & 0x40ffffff);
    current_hp_strip_ = OBoy::Environment::instance()->createTriStrip(722);
    current_hp_strip_->setColor(Ogre::ColourValue::Red & 0x40ffffff);
#else
	min_hp_object_ =  scene_manager_->createManualObject();
	min_hp_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();

    Ogre::MaterialPtr min_hp_material = Ogre::MaterialManager::getSingleton().create("MinHpMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	min_hp_material->setReceiveShadows(false);
	min_hp_material->getTechnique(0)->setLightingEnabled(true);
	min_hp_material->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
	min_hp_material->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
	min_hp_material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);

	min_hp_node_->attachObject(min_hp_object_);

	current_hp_object_ =  scene_manager_->createManualObject();
	current_hp_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();

	Ogre::MaterialPtr current_hp_material = Ogre::MaterialManager::getSingleton().create("CurrentHpMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	current_hp_material->setReceiveShadows(false);
	current_hp_material->getTechnique(0)->setLightingEnabled(true);
	current_hp_material->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
	current_hp_material->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
	current_hp_material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);

	current_hp_node_->attachObject(current_hp_object_);
#endif
    updateHp(10);
}
#if 0
void Planet::setImage(const ::std::string &id)
{
    OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
    image_ = rm->getImage(id);
    sphere_->setTexture(image_);
    //rm->saveResource((OBoy::Resource*)image_, "image");
}
#endif

void Planet::setRadius(const int radius)
{
    Body::setRadius(radius);
#if 0
    delete sphere_;
    sphere_ = OBoy::Environment::instance()->createSphere((float)radius_, 20, 20);
    sphere_->build();
#endif
}

#if TODO
void Planet::draw(OBoy::Graphics *g, Ogre::Vector3 camera)
{
}

void Planet::draw(OBoy::Graphics *g, const Ogre::Vector3 &camera, const bool drawMinHp, const Player* player)
{
    int w = OBoy::Environment::screenWidth();
    int h = OBoy::Environment::screenHeight();
    Ogre::Vector3 pos = pos_ - camera;

    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    //g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    _draw(g,pos);
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);

    if(drawMinHp)
    {
        //g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
        g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
        _drawMinHp(g, pos, player);
        g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
        if(waypoints_[player] == NULL || !waypoints_[player]->isSelectedAsParent())
        {
          _drawText(g, pos, player);
        }
    }
}

void Planet::_draw(OBoy::Graphics *g, const Ogre::Vector3 &pos)
{
    if(image_==NULL)
    {
        return;
    }
    g->pushTransform();
    g->setColorizationEnabled(true);
    g->setColor(color_);
#if 0
#ifdef OBOY_PLATFORM_WIN32
    g->rotate(rot_[0], rot_[1], rot_[2]);
    g->translate(pos.x(),pos.y());
#else
    g->translate(pos.x(),pos.y());
    g->rotate(rot_[0], rot_[1], rot_[2]);
#endif
#else
    g->translate(pos.x(),pos.y());
#endif
    g->drawImage(image_);
    //g->drawSphere(sphere_);
    g->setColorizationEnabled(false);
    g->popTransform();
}

void Planet::_drawMinHp(OBoy::Graphics *g, const Ogre::Vector3 &pos, const Player* player)
{
    g->pushTransform();
    g->translate(pos.x(),pos.y());
    if(min_hp_[player] > 0)
    {
        const float outerRadius = 1.2f*radius_;
        g->setColor(Ogre::ColourValue(Ogre::ColourValue::Red));
        g->drawCircle(0, 0, outerRadius+min_hp_[player], 1);
        // draw the minimum Hp:
        g->drawTriStrip(min_hp_Strip);
        if(player->getColor() == color_)
        {
            // draw the current Hp:
            g->drawTriStrip(current_hp_strip_);
        }
    }
    g->popTransform();
}

void Planet::_drawText(OBoy::Graphics *g, const Ogre::Vector3 &pos, const Player* player)
{
    if(planet_owner_ == player || planet_owner_ == NULL || selected_)
    {
        // draw stats:
        float str2scale = 0.8f;
        OBoy::UString stats;
        if(!selected_)
        {
            stats = OBoy::UString::format("%d", hp_);
        }
        else if(planet_owner_ == player || planet_owner_ == NULL)
        {
            stats = OBoy::UString::format("%d/%d", hp_, min_hp_[player]);
        }
        else
        {
            stats = OBoy::UString::format("?/%d", min_hp_[player]);
        }
        float x = pos.x() - (font_->getStringWidth(stats)*str2scale) / 2.0f;
        g->setColorizationEnabled(true);
        g->setColor(Ogre::ColourValue::White);
        g->pushTransform();
        g->translate(x, pos.y());
        //g->setZ(radius_+1);
        font_->drawString(g,stats,str2scale);
        g->popTransform();
        g->setColorizationEnabled(false);
    }
}
#endif

float Planet::getDistance(const Ogre::Vector3 pos)
{
    return abs((pos_ - pos).length() - radius_);
}

void Planet::capture(const Player* player)
{
    color_ = player->getColor();
    planet_owner_ = player;
    captured_ = true;
    ttl_ = 0;
#if 0
    sphere_->setColor(color_);
    sphere_->build();
#endif
}

void Planet::release()
{
    color_ = Ogre::ColourValue::White;
    captured_ = false;
    ttl_ = 0;
#if 0
    sphere_->setColor(color_);
    sphere_->build();
#endif
}

void Planet::changeMinHp(int diffHp, const Player* player, std::vector<Ship*>& ships)
{
    min_hp_[player] += diffHp;
    if(min_hp_[player] < 0)
    {
        min_hp_[player] = 0;
    }
    else if(min_hp_[player] > MAX_HP)
    {
        min_hp_[player] = MAX_HP;
    }
    if(hp_ > min_hp_[player] && color_ == player->getColor())
    {
      releaseShips(ships);
    }
    // graphics
    const float outerRadius = 1.2f*radius_;
    float angle = 0;
    int i = 0;
#if TODO
    while(angle <= 360)
    {
        min_hp_Strip->setVertPos(i, outerRadius*cos(deg2rad(angle)),
                                 outerRadius*sin(deg2rad(angle)));
        min_hp_Strip->setVertPos(++i, (outerRadius+min_hp_[player])*cos(deg2rad(angle)),
                                 (outerRadius+min_hp_[player])*sin(deg2rad(angle)));
        ++i;
        ++angle;
    }
    min_hp_Strip->build();
#endif
}

void Planet::updateHp(int delta)
{
    hp_ += delta;
    // graphics
    const float outerRadius = 1.2f*radius_;
    float angle = 0;
    int i = 0;
#if TODO
    while(angle <= 360)
    {
        current_hp_strip_->setVertPos(i, outerRadius*cos(deg2rad(angle)),
                                      outerRadius*sin(deg2rad(angle)));
        current_hp_strip_->setVertPos(++i, (outerRadius+hp_)*cos(deg2rad(angle)),
                                      (outerRadius+hp_)*sin(deg2rad(angle)));
        ++i;
        ++angle;
    }
    current_hp_strip_->build();
#endif
}

int Planet::releaseShips(std::vector<Ship*>& ships)
{
    int num_ships_released = 0;
    if(waypoints_[planet_owner_] != NULL &&
       waypoints_[planet_owner_]->getSiblings().size() > 0)
    {
        for(int i = 0; i < hp_ - min_hp_[planet_owner_]; i++)
        {
            if(rand()%100 > waypoints_[planet_owner_]->getProbaRing() ||
               hp_ >= min_hp_[planet_owner_])
            {
                Ship* ship = new Ship(waypoints_[planet_owner_],
                                      waypoints_[planet_owner_]->getShipDestination(),
                                      planet_owner_, pos_, scene_manager_);
                ships.push_back(ship);
                num_ships_released ++;
            }
        }
    }
    updateHp(-num_ships_released);
    return num_ships_released;
}

void Planet::launchShip(std::vector<Ship*>& ships)
{
    if(waypoints_[planet_owner_] != NULL &&
       waypoints_[planet_owner_]->getSiblings().size() > 0 &&
       (rand()%100 > waypoints_[planet_owner_]->getProbaRing() ||
        hp_ >= min_hp_[planet_owner_]))
    {
        Ship* ship = new Ship(waypoints_[planet_owner_],
                              waypoints_[planet_owner_]->getShipDestination(),
                              planet_owner_, pos_, scene_manager_);
        ships.push_back(ship);
        return;
    }
    updateHp(+1);
}

void Planet::update(float dt, std::vector<Ship*>& ships)
{
    if(captured_)
    {
        ttl_ += dt;
        if(ttl_ > speed_)
        {
            // create a ship and release it
            launchShip(ships);
            ttl_ = 0;
        }
    }
    for(int i = 0; i < 3; i++)
    {
        rot_[i] += rot_vel_[i]*dt;
        if(rot_[i] >= 360 && rot_vel_[i] > 0)
        {
            rot_[i] = 0;
        }
        else if(rot_[i] == 0 && rot_vel_[i] < 0)
        {
            rot_[i] = 360;
        }
    }
}

void Planet::handleShipCollision(Ship* ship)
{
    if(planet_owner_ != ship->getShipOwner()) // impact
    {
        ship->setShipState(Ship::SHIP_DEAD);
        // play a sound:
        //OBoy::Environment::instance()->getSoundPlayer()->playSound(exposion_sound_);
        if (hp_ > 0)
        {
            updateHp(-1);
            if (hp_ == 0)
            {
                release();
            }
        }
        else
        {
            capture(ship->getShipOwner());
        }
    }
    else
    {
        Waypoint* waypoint = waypoints_[ship->getShipOwner()];
        if (waypoint != NULL && waypoint->getSiblings().size() > 0 && (rand()%100 > waypoint->getProbaRing() || hp_ >= min_hp_[planet_owner_])) // choose a new destination
        {
            ship->setOriginAndDestination(waypoint, waypoint->getShipDestination());
        }
        else // increase planet energy
        {
            updateHp(+1);
            ship->setShipState(Ship::SHIP_REMOVE);
        }
    }
}
