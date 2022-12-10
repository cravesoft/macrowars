#include <OgreSceneManager.h>
#include <OgreMaterialManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreTechnique.h>

#include "asteroid.hpp"
#include "log.hpp"

const std::string Asteroid::filename_ = "asteroid.png";

Asteroid::Asteroid(Ogre::SceneManager *scene_manager, int radius, const Ogre::Vector3 &pos)
    : scene_manager_(scene_manager)
{
    pos_ = pos;
    radius_ = radius;
    color_ = Ogre::ColourValue::White;
    entity_ = scene_manager_->createEntity(Ogre::SceneManager::PT_SPHERE);
    scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();

    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("AsteroidMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    material->setReceiveShadows(false);
    material->getTechnique(0)->setLightingEnabled(true);
    material->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
    material->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
    material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);

    entity_->setMaterial(material);
    scene_node_->attachObject(entity_);
    scene_node_->setPosition(pos_);
    float scale_factor = (float)radius_ / 100;
    scene_node_->setScale(Ogre::Vector3(scale_factor, scale_factor, scale_factor));
    //image_ = OBoy::Environment::getImage("IMAGE_ASTEROID");
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
}

Asteroid::~Asteroid(void)
{
}

#if TODO
void Asteroid::draw(OBoy::Graphics *g, const Ogre::Vector2 camera)
{
    int w = OBoy::Environment::screenWidth();
    int h = OBoy::Environment::screenHeight();
    Ogre::Vector2 pos = pos_ - camera;

    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    _draw(g,pos);
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}

void Asteroid::_draw(OBoy::Graphics *g, Ogre::Vector2 &pos)
{
    if (image_==NULL)
    {
        return;
    }

    g->pushTransform();
    //g->rotate(rot_);
    g->setColorizationEnabled(true);
    g->setColor(color_);
    g->translate(pos.x(),pos.y());
    g->drawImage(image_);
    g->setColorizationEnabled(false);
    g->popTransform();
}
#endif

void Asteroid::update()
{
}

void Asteroid::handleShipCollision(Ship* ship)
{
    ship->setShipState(Ship::SHIP_DEAD);
}
