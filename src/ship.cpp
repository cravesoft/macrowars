#include <OgreMaterialManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTechnique.h>

#include "log.hpp"
#include "ship.hpp"
#include "player.hpp"

#define SHIP_WIDTH 4

#define callMemberFunction(object,ptrToMember) ((object)->*(ptrToMember))

Ship::Ship(OriginAndDestination *origin, OriginAndDestination *destination,
           const Player* player, Ogre::Vector3 position,
           Ogre::SceneManager *scene_manager)
    : Traveller(origin, destination)
    , Satellite()
    , scene_manager_(scene_manager)
    , ship_owner_(player)
    , ship_state_(SHIP_ALIVE)
    , num_particles_(10)
    , ship_object_(NULL)
    , ship_node_(NULL)
{
    speed_ = .1f;
    angle_ = 0;
    radius_ = SHIP_WIDTH;
    color_ = player->getColor();
    pos_ = position;
    particles_ = new Particle[num_particles_];
    move_callbacks_.push_back(&Ship::moveAsTraveller);
    move_callbacks_.push_back(&Ship::moveAsSatellite);
    for(int i = 0; i < num_particles_; ++i)
    {
        particles_[i].active = true;
        particles_[i].life = Ogre::ColourValue::Black;
        //particles_[i].fade = (float)(rand()%100)/1000.0f+0.003f;
        particles_[i].fade = 0x01;
        particles_[i].color = Ogre::ColourValue::White;
        particles_[i].pos.x = 0;
        particles_[i].pos.y = 0;
        particles_[i].dir.x = (float)((rand()%50)-26.0f)*10.0f;
        particles_[i].dir.y = (float)((rand()%50)-26.0f)*10.0f;
#if 0
        particles_[i].shape = OBoy::Environment::instance()->createLineStrip(5);
        particles_[i].shape->setVertPos(0, -2, -2);
        particles_[i].shape->setVertPos(1, 4, -2);
        particles_[i].shape->setVertPos(2, 4, 4);
        particles_[i].shape->setVertPos(3, -2, 4);
        particles_[i].shape->setVertPos(4, -2, -2);
        particles_[i].shape->setColor(particles_[i].color);
        particles_[i].shape->build();
#endif
    }
#if OBOY
    triangle_ = OBoy::Environment::instance()->createLineStrip(4);
    triangle_->setVertPos(0, 0, 5);
    triangle_->setVertPos(1, 12, 0);
    triangle_->setVertPos(2, 0, -5);
    triangle_->setVertPos(3, 0, 5);
    triangle_->setColor(color_);
    triangle_->build();
#else
	// load 3d line for mouse picking
	ship_object_ =  scene_manager_->createManualObject();
	ship_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();

	Ogre::MaterialPtr ship_material = Ogre::MaterialManager::getSingleton().create("ShipMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	ship_material->setReceiveShadows(false);
	ship_material->getTechnique(0)->setLightingEnabled(true);
	ship_material->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
	ship_material->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
	ship_material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);

	ship_object_->begin("ShipMaterial", Ogre::RenderOperation::OT_LINE_STRIP);
	ship_object_->position(0, 5, 0);
	ship_object_->position(12, 0, 0);
	ship_object_->position(0, -5, 0);
	ship_object_->position(0, 5, 0);
	ship_object_->end();
	ship_node_->attachObject(ship_object_);
    //ship_node_->setPosition(pos_);
	//ship_node_->setVisible(true);
#endif
}

Ship::~Ship(void)
{
#if OBOY
    delete triangle_;
#else
	if(ship_node_ != NULL)
	{
		//scene_manager_->getRootSceneNode()->removeAndDestroyChild(ship_node_);
        delete ship_node_;
		ship_node_ = NULL;
	}

	if(ship_object_ != NULL)
	{
		scene_manager_->destroyManualObject(ship_object_);
		ship_object_ = NULL;
	}
#endif

    delete[] particles_;
}

#if TODO
void Ship::draw(OBoy::Graphics *g, const Ogre::Vector3 camera)
{
    int w = OBoy::Environment::screenWidth();
    int h = OBoy::Environment::screenHeight();
    Ogre::Vector3 pos = pos_ - camera;

    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    _draw(g,pos);
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}

void Ship::_draw(OBoy::Graphics *g, Ogre::Vector3 &pos)
{
    if(ship_state_ > 0)
    {
        g->pushTransform();
        //g->setColor(color_);
        g->rotate(-angle_);
        g->translate(pos.x(), pos.y());
        g->drawLineStrip(triangle_);
        g->popTransform();
    }
    else
    {
#if 1
        for (int i = 0; i < num_particles_; ++i)
        {
            if (particles_[i].active)
            {
                g->pushTransform();
                g->setColor(particles_[i].color.argb | particles_[i].life.argb);
                g->translate(pos.x() + particles_[i].pos.x(), pos.y() + particles_[i].pos.y());
                g->drawRect(-2, -2, 4, 4);
#if 0
                g->drawLineStrip(particles_[i].shape);
#endif
                g->popTransform();
            }
        }
#endif
    }
}
#endif

void Ship::handleShipCollision(Ship* ship)
{
    this->setShipState(SHIP_DEAD);
    ship->setShipState(SHIP_DEAD);
}

void Ship::moveAsSatellite(float speed)
{
    Satellite::move(speed);
}

void Ship::moveAsTraveller(float speed)
{
    Traveller::move(speed);
}

void Ship::move(float speed)
{
    callMemberFunction(this, move_callbacks_[orbiting_])(speed);
}

void Ship::update(const float dt)
{
    speed_ = dt*0.5f;

    if(ship_state_ > 0)
    {
        move(speed_);
    }
    else if(ship_state_ == SHIP_DEAD)
    {
        ship_state_ = SHIP_REMOVE;
        float slowdown = 2.0f;
        for(int i = 0; i < num_particles_; ++i)
        {
            if (particles_[i].active)
            {
                ship_state_ = SHIP_DEAD;
                particles_[i].pos += particles_[i].dir/(slowdown*1000);
                particles_[i].dir -= accel_/(slowdown*10);
#if TODO
                particles_[i].life.argb -= particles_[i].fade << 24;
#endif
#if 0
                particles_[i].shape->setColor(particles_[i].color | particles_[i].life);
                particles_[i].shape->build();
#endif
#if TODO
                if(particles_[i].life.argb <= 0xffffff00)
                {
                    particles_[i].active = false;
                }
#endif
            }
        }
    }
}
