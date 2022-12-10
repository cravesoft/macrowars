#include <algorithm>
#include <functional>
#include <assert.h>

#include <OgreMaterialManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include "waypoint.hpp"

#define ARROW_HALF_WIDTH 8
#define ARROW_HALF_HEAD_WIDTH 16
#define ARROW_HEAD_HEIGHT 32
#define WAYPOINT_DEFAULT_RADIUS 32

Waypoint::Waypoint(Ogre::SceneManager *scene_manager)
    : planet_(NULL)
    , scene_manager_(scene_manager)
    , draw_ring_(true)
    , selected_(false)
    , selected_as_parent_(false)
    , proba_ring_(50)
    , ring_node_(NULL)
    , ring_object_(NULL)
{
    pos_ = Ogre::Vector3();
    color_ = Ogre::ColourValue::White;
#if TODO
    OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
    font_ = rm->getFont("FONT_MAIN");
    ring_strip_ = OBoy::Environment::instance()->createTriStrip(722);
#endif
    setRadius(WAYPOINT_DEFAULT_RADIUS);
}

Waypoint::~Waypoint(void)
{
	if(ring_node_ != NULL)
	{
		//scene_manager_->getRootSceneNode()->removeAndDestroyChild("RingNode");
        delete ring_node_;
		ring_node_ = NULL;
	}

	if(ring_object_ != NULL)
	{
		scene_manager_->destroyManualObject(ring_object_);
		ring_object_ = NULL;
	}
}

void Waypoint::setRadius(const int radius)
{
    Body::setRadius(radius);
    //graphics:
    const float outerRadius = 1.2f*radius_;
#if TODO
    ring_strip_->setColor((color_.argb & 0x00ffffff) | ((proba_ring_*0xff/100) << 24) );
    float angle = 0;
    int i = 0;
    while(angle <= 360)
    {
        ring_strip_->setVertPos(i, radius_ * cos(Ogre::Math::DegreesToRadians(angle)), radius_*sin(Ogre::Math::DegreesToRadians(angle)));
        ring_strip_->setVertPos(++i, outerRadius * cos(Ogre::Math::DegreesToRadians(angle)), outerRadius*sin(Ogre::Math::DegreesToRadians(angle)));
        ++i;
        ++angle;
    }
    ring_strip_->build();
#endif
}

#if 0
void Waypoint::createArrow(Waypoint* sibling)
{
    arrow_tail_tri_strips_[sibling] = OBoy::Environment::instance()->createTriStrip(4);
    arrow_head_tri_strips_[sibling] = OBoy::Environment::instance()->createTriStrip(4);
    arrow_line_strips_[sibling] = OBoy::Environment::instance()->createLineStrip(7);
}

void Waypoint::deleteArrow(Waypoint* sibling)
{
    delete(arrow_tail_tri_strips_[sibling]);
    delete(arrow_head_tri_strips_[sibling]);
    delete(arrow_line_strips_[sibling]);
}

void Waypoint::updateArrow(Waypoint* sibling, const float length, const OBoy::Color color)
{
    arrow_tail_tri_strips_[sibling]->setColor(color);
    arrow_tail_tri_strips_[sibling]->setVertPos(0, length, ARROW_HALF_WIDTH);
    arrow_tail_tri_strips_[sibling]->setVertPos(1, 0, 0);
    arrow_tail_tri_strips_[sibling]->setVertPos(2, length, 0);
    arrow_tail_tri_strips_[sibling]->setVertPos(3, length, -ARROW_HALF_WIDTH);
    arrow_tail_tri_strips_[sibling]->build();

    arrow_head_tri_strips_[sibling]->setColor(color);
    arrow_head_tri_strips_[sibling]->setVertPos(0, length, ARROW_HALF_HEAD_WIDTH);
    arrow_head_tri_strips_[sibling]->setVertPos(1, length, 0);
    arrow_head_tri_strips_[sibling]->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
    arrow_head_tri_strips_[sibling]->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
    arrow_head_tri_strips_[sibling]->build();

    arrow_line_strips_[sibling]->setColor(color_);
    arrow_line_strips_[sibling]->setVertPos(0, length, ARROW_HALF_WIDTH);
    arrow_line_strips_[sibling]->setVertPos(1, length, ARROW_HALF_HEAD_WIDTH);
    arrow_line_strips_[sibling]->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
    arrow_line_strips_[sibling]->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
    arrow_line_strips_[sibling]->setVertPos(4, length, -ARROW_HALF_WIDTH);
    arrow_line_strips_[sibling]->setVertPos(5, 0, 0);
    arrow_line_strips_[sibling]->setVertPos(6, length, ARROW_HALF_WIDTH);
    arrow_line_strips_[sibling]->build();
}
#endif

#if TODO
void Waypoint::drawArrow(OBoy::Graphics *g, const float length, const Ogre::ColourValue color)
{
    OBoy::TriStrip* arrowTailTriStrip = OBoy::Environment::instance()->createTriStrip(4);
    arrowTailTriStrip->setColor(color);
    arrowTailTriStrip->setVertPos(0, length, ARROW_HALF_WIDTH);
    arrowTailTriStrip->setVertPos(1, 0, 0);
    arrowTailTriStrip->setVertPos(2, length, 0);
    arrowTailTriStrip->setVertPos(3, length, -ARROW_HALF_WIDTH);
    arrowTailTriStrip->build();
    g->drawTriStrip(arrowTailTriStrip);
    delete arrowTailTriStrip;

    OBoy::TriStrip* arrowHeadTriStrip = OBoy::Environment::instance()->createTriStrip(4);
    arrowHeadTriStrip->setColor(color);
    arrowHeadTriStrip->setVertPos(0, length, ARROW_HALF_HEAD_WIDTH);
    arrowHeadTriStrip->setVertPos(1, length, 0);
    arrowHeadTriStrip->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
    arrowHeadTriStrip->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
    arrowHeadTriStrip->build();
    g->drawTriStrip(arrowHeadTriStrip);
    delete arrowHeadTriStrip;

    OBoy::LineStrip* arrowLineStrip = OBoy::Environment::instance()->createLineStrip(7);
    arrowLineStrip->setColor(color_);
    arrowLineStrip->setVertPos(0, length, ARROW_HALF_WIDTH);
    arrowLineStrip->setVertPos(1, length, ARROW_HALF_HEAD_WIDTH);
    arrowLineStrip->setVertPos(2, length + ARROW_HEAD_HEIGHT, 0);
    arrowLineStrip->setVertPos(3, length, -ARROW_HALF_HEAD_WIDTH);
    arrowLineStrip->setVertPos(4, length, -ARROW_HALF_WIDTH);
    arrowLineStrip->setVertPos(5, 0, 0);
    arrowLineStrip->setVertPos(6, length, ARROW_HALF_WIDTH);
    arrowLineStrip->build();
    g->drawLineStrip(arrowLineStrip);
    delete arrowLineStrip;
}
#endif

void Waypoint::setDefaultRadius()
{
    radius_ = WAYPOINT_DEFAULT_RADIUS;
}

#if TODO
void Waypoint::draw(OBoy::Graphics *g, const Ogre::Vector3 camera)
{
    int w = OBoy::Environment::screenWidth();
    int h = OBoy::Environment::screenHeight();
    Ogre::Vector3 pos = pos_ - camera;
    const float outerRadius = 1.2f*radius_;

    if(draw_ring_)
    {
        g->pushTransform();
        g->setColor(color_);
        g->translate(pos.x, pos.y);
        // draw the ring:
        g->drawCircle(0, 0, (float)radius_, 1);
        g->drawCircle(0, 0, outerRadius, 1);
        // draw the inside of the ring:
        g->drawTriStrip(ring_strip_);
        g->popTransform();
    }
    for(int i = 0 ; i < siblings_.size(); i++)
    {
        if(siblings_[i]->getPosition() != pos_)
        {
            // compute the angle between the sibling and its parent:
            Ogre::Vector3 direction = (siblings_[i]->getPosition() - pos_).normalize();
            float angle = atan2(direction.y, direction.x);
            // compute the length of the arrow
            float length = (siblings_[i]->getPosition() - pos_).length() - ARROW_HEAD_HEIGHT;
            g->pushTransform();
            g->setColor(color_);
            if(siblings_[i]->getDrawRing())
            {
                length -= 1.2f*siblings_[i]->getRadius();
            }
#ifdef OBOY_PLATFORM_WIN32
            if(draw_ring_)
            {
                g->translate(outerRadius, 0);
                length -= outerRadius;
            }
            g->rotate(Ogre::Math::RadiansToDegrees(-angle));
            g->translate(pos.x, pos.y);
#else
            g->translate(pos.x, pos.y);
            g->rotate(Ogre::Math::RadiansToDegrees(-angle));
            if(draw_ring_)
            {
                g->translate(outerRadius, 0);
                length -= outerRadius;
            }
#endif
            //g->drawTriStrip(arrow_tail_tri_strips_[siblings_[i]]);
            //g->drawTriStrip(arrow_head_tri_strips_[siblings_[i]]);
            //g->drawLineStrip(arrow_line_strips_[siblings_[i]]);
            drawArrow(g, length, (color_.argb & 0x00ffffff) | ((probas_[siblings_[i]]*0xff/100) << 24) );
            g->popTransform();
        }
    }
}

void Waypoint::drawProbas(OBoy::Graphics *g, const Ogre::Vector3 camera)
{
    int w = OBoy::Environment::screenWidth();
    int h = OBoy::Environment::screenHeight();
    Ogre::Vector3 pos = pos_ - camera;

    OBoy::UString stats;
    float str2scale = 0.8f;
    g->setColor(Ogre::ColourValue::White);
    stats = OBoy::UString::format("%d%%", proba_ring_);
    float x = pos.x - (font_->getStringWidth(stats)*str2scale) / 2.0f;
    g->pushTransform();
    g->translate(x, pos.y);
      font_->drawString(g,stats,str2scale);
    g->popTransform();

    const float outerRadius = 1.2f*radius_;
    for(int i = 0 ; i < siblings_.size(); i++)
    {
        stats = OBoy::UString::format("%d%%", probas_[siblings_[i]]);
        Ogre::Vector3 position;
        Ogre::Vector3 direction = (siblings_[i]->getPosition() - pos_).normalize();
        float angle = atan2(direction.y, direction.x);
        position.x = (cos(angle)*((siblings_[i]->getPosition() - pos_).length() - outerRadius - 1.2f*siblings_[i]->getRadius()))/2 + pos.x + cos(angle)*outerRadius;
        position.y = (sin(angle)*((siblings_[i]->getPosition() - pos_).length() - outerRadius - 1.2f*siblings_[i]->getRadius()))/2 + pos.y + sin(angle)*outerRadius;
        g->pushTransform();
        g->translate(position.x, position.y);
        font_->drawString(g,stats,str2scale);
        g->popTransform();
    }
}
#endif

bool Waypoint::isTouched(const Ogre::Vector3 pos)
{
    if((pos_ - pos).length() < 1.2f*radius_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Waypoint::isRingTouched(const Ogre::Vector3 pos)
{
    float distance = (pos_ - pos).length();
    if(distance > radius_ && distance < 1.2f * radius_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Waypoint::isArrowTouched(const Ogre::Vector3 pos, Waypoint* &parent)
{
    for(int i = 0 ; i < parents_.size(); i++)
    {
        Ogre::Vector3 direction = (parents_[i]->getPosition() - pos_).normalisedCopy();
        float angle = Ogre::Math::RadiansToDegrees(atan2(direction.y, direction.x));
        const float outerRadius = 1.2f*radius_;

        Ogre::Vector3 ul = Ogre::Vector3();
        ul.x = cos(Ogre::Math::DegreesToRadians(angle)) * outerRadius -
               sin(Ogre::Math::DegreesToRadians(angle)) *
               (-ARROW_HALF_HEAD_WIDTH) + pos_.x;
        ul.y = sin(Ogre::Math::DegreesToRadians(angle)) * outerRadius +
               cos(Ogre::Math::DegreesToRadians(angle)) *
               (-ARROW_HALF_HEAD_WIDTH) + pos_.y;

        Ogre::Vector3 ur = Ogre::Vector3();
        ur.x = cos(Ogre::Math::DegreesToRadians(angle))*outerRadius -
               sin(Ogre::Math::DegreesToRadians(angle))*ARROW_HALF_HEAD_WIDTH +
               pos_.x;
        ur.y = sin(Ogre::Math::DegreesToRadians(angle))*outerRadius +
               cos(Ogre::Math::DegreesToRadians(angle))*ARROW_HALF_HEAD_WIDTH +
               pos_.y;

        Ogre::Vector3 lr = Ogre::Vector3();
        lr.x = cos(Ogre::Math::DegreesToRadians(angle)) *
               ((parents_[i]->getPosition() - pos_).length() -
                1.2f*parents_[i]->getRadius()) -
               sin(Ogre::Math::DegreesToRadians(angle)) * ARROW_HALF_HEAD_WIDTH +
               pos_.x;
        lr.y = sin(Ogre::Math::DegreesToRadians(angle)) *
               ((parents_[i]->getPosition() - pos_).length() -
                1.2f*parents_[i]->getRadius()) +
               cos(Ogre::Math::DegreesToRadians(angle))*ARROW_HALF_HEAD_WIDTH +
               pos_.y;

        Ogre::Vector3 ll = Ogre::Vector3();
        ll.x = cos(Ogre::Math::DegreesToRadians(angle)) *
               ((parents_[i]->getPosition() - pos_).length() -
                1.2f*parents_[i]->getRadius()) -
               sin(Ogre::Math::DegreesToRadians(angle)) *
               (-ARROW_HALF_HEAD_WIDTH) + pos_.x;
        ll.y = sin(Ogre::Math::DegreesToRadians(angle)) *
               ((parents_[i]->getPosition() - pos_).length() -
                1.2f*parents_[i]->getRadius()) +
               cos(Ogre::Math::DegreesToRadians(angle)) *
               (-ARROW_HALF_HEAD_WIDTH) + pos_.y;

        Ogre::Vector3 axis1 = ur-ul;
        Ogre::Vector3 axis2 = ur-lr;

        float ur1 = (axis1 * (ur.dotProduct(axis1) /
                     (axis1.length() * axis1.length()))).dotProduct(axis1);
        float ul1 = (axis1 * (ul.dotProduct(axis1) /
                     (axis1.length() * axis1.length()))).dotProduct(axis1);
        float ur2 = (axis2 * (ur.dotProduct(axis2) / (axis2.length() *
                     axis2.length()))).dotProduct(axis2);
        float lr2 = (axis2*(lr.dotProduct(axis2)/(axis2.length()*axis2.length()))).dotProduct(axis2);
        float pos1 = (axis1*(pos.dotProduct(axis1)/(axis1.length()*axis1.length()))).dotProduct(axis1);
        float pos2 = (axis2*(pos.dotProduct(axis2)/(axis2.length()*axis2.length()))).dotProduct(axis2);

        if(pos1 < std::max(ur1, ul1) && pos1 > std::min(ur1, ul1) && pos2 < std::max(ur2, lr2) && pos2 > std::min(ur2, lr2))
        {
            parent = parents_[i];
            return true;
        }
    }
    return false;
}

void Waypoint::updateProbasArrow(int proba, Waypoint* sibling)
{
    if(siblings_.size() <= 1)
    {
        return;
    }

    int size = siblings_.size();
    int minProba = 1;
    int maxProba = 100 - probas_[sibling] - minProba*(size - 2);
    int sum = probas_[sibling];
    int inc_proba = -10/(size - 1);
    std::vector<Waypoint*> probasAlreadyUpdated;
    Waypoint* current_waypoint = NULL;
    for(int i = 0; i < size - 2; i++)
    {
        int current_proba = 999;
        for(int j=0 ; j<(int)siblings_.size() ; j++)
        {
            if(probas_[siblings_[j]] < current_proba && siblings_[j] != sibling)
            {
                bool alreadyUpdated = false;
                for(int k=0 ; k<(int)probasAlreadyUpdated.size() ; k++)
                {
                    if(probasAlreadyUpdated[k] == siblings_[j])
                    {
                        alreadyUpdated = true;
                        break;
                    }
                }
                if(!alreadyUpdated)
                {
                    current_proba = probas_[siblings_[j]];
                    current_waypoint = siblings_[j];
                }
            }
        }
        probasAlreadyUpdated.push_back(current_waypoint);
        inc_proba = proba/(size - 1);
        if(probas_[current_waypoint] - inc_proba > maxProba)
        {
            proba += maxProba - probas_[current_waypoint];
            probas_[current_waypoint] = maxProba;
        }
        else if(probas_[current_waypoint] - inc_proba < minProba)
        {
            proba -= probas_[current_waypoint] - minProba;
            probas_[current_waypoint] = minProba;
        }
        else
        {
            probas_[current_waypoint] -= inc_proba;
        }
        sum += probas_[current_waypoint];
    }
    current_waypoint = NULL;
    for(int j=0 ; j<(int)siblings_.size() ; j++)
    {
        if(siblings_[j] != sibling)
        {
            bool alreadyUpdated = false;
            for(int k=0 ; k<(int)probasAlreadyUpdated.size() ; k++)
            {
                if(probasAlreadyUpdated[k] == siblings_[j])
                {
                    alreadyUpdated = true;
                    break;
                }
            }
            if(!alreadyUpdated)
            {
                current_waypoint = siblings_[j];
                break;
            }
        }
    }
    probas_[current_waypoint] = 100 - sum;
}

void Waypoint::removeProbaArrow(Waypoint* sibling)
{
    int proba = probas_[sibling];
    probas_[sibling] = 0;
    updateProbasArrow(-proba, sibling);
}

void Waypoint::addProbaArrow(Waypoint* sibling)
{
    int size = siblings_.size();
    probas_[sibling] = 100/size;
    updateProbasArrow(probas_[sibling], sibling);
}

void Waypoint::changeProbaArrow(int proba, Waypoint* sibling)
{
    int size = siblings_.size();
    if(size > 1)
    {
        int minProba = 1;
        int maxProba = 100 - minProba*(size - 1);
        if(probas_[sibling] + proba > maxProba)
        {
            proba = maxProba - probas_[sibling];
        }
        else if(probas_[sibling] + proba < minProba)
        {
            proba = minProba - probas_[sibling];
        }
        probas_[sibling] += proba;
        updateProbasArrow(proba, sibling);
    }
}

void Waypoint::changeProbaRing(const int proba)
{
    proba_ring_ += proba;
    if(proba_ring_ > 100)
    {
        proba_ring_ = 100;
    }
    else if(proba_ring_ < 0)
    {
        proba_ring_ = 0;
    }
#if TODO
    //graphics:
    ring_strip_->setColor((color_.argb & 0x00ffffff) | ((proba_ring_*0xff/100) << 24));
    ring_strip_->build();
#endif
}

void Waypoint::addParent(Waypoint* parent)
{
    if(this == parent || find(parents_.begin(),parents_.end(),parent) != parents_.end())
    {
        return;
    }
    parents_.push_back(parent);
}

void Waypoint::addSibling(Waypoint* sibling)
{
    if(this == sibling || find(siblings_.begin(),siblings_.end(),sibling) != siblings_.end())
    {
        return;
    }
    siblings_.push_back(sibling);
    addProbaArrow(sibling);
}

void Waypoint::addSibling(Waypoint* sibling, const int proba)
{
    if(this == sibling || find(siblings_.begin(),siblings_.end(),sibling) != siblings_.end())
    {
        return;
    }
    siblings_.push_back(sibling);
    probas_[sibling] = proba;
    updateProbasArrow(probas_[sibling], sibling);
}

void Waypoint::addParents(std::vector<Waypoint*> &parents)
{
    for_each(parents.begin(), parents.end(), std::bind1st(std::mem_fun(&Waypoint::addParent),this));
}

void Waypoint::addSiblings(std::vector<Waypoint*> &siblings)
{
    for_each(siblings.begin(), siblings.end(), std::bind1st(std::mem_fun(&Waypoint::addSibling),this));
}

void Waypoint::removeParent(Waypoint* parent)
{
    assert(find(parents_.begin(),parents_.end(),parent) != parents_.end());
    parents_.erase(find(parents_.begin(),parents_.end(),parent));
}

void Waypoint::removeSibling(Waypoint* sibling)
{
    removeProbaArrow(sibling);
    assert(find(siblings_.begin(),siblings_.end(),sibling) != siblings_.end());
    siblings_.erase(find(siblings_.begin(),siblings_.end(),sibling));
}

void Waypoint::removeAllSiblings()
{
    siblings_.erase(siblings_.begin(), siblings_.end());
}

void Waypoint::removeAllParents()
{
    parents_.erase(parents_.begin(), parents_.end());
}

Waypoint* Waypoint::getShipDestination()
{
    assert (siblings_.size() > 0);
    int index_dest = rand()%100;
    int sum = 0;
    for(int i=0 ; i<(int)siblings_.size() ; ++i)
    {
        sum += probas_[siblings_[i]];
        if(index_dest <= sum)
        {
            return siblings_[i];
        }
    }
    return siblings_[0];
}

void Waypoint::mergeWaypoint(Waypoint* waypoint)
{
    this->addParents(waypoint->getParents());
    this->addSiblings(waypoint->getSiblings());
    std::vector<Waypoint*> parents = waypoint->getParents();
    for(int i=0 ; i<(int)parents.size() ; i++)
    {
        int proba = parents[i]->getProbaArrow(waypoint);
        parents[i]->removeSibling(waypoint);
        parents[i]->addSibling(this, proba);
    }
    std::vector<Waypoint*> siblings = waypoint->getSiblings();
    for(int i=0 ; i<(int)siblings.size() ; i++)
    {
        siblings[i]->removeParent(waypoint);
        siblings[i]->addParent(this);
    }
    waypoint->removeAllParents();
    waypoint->removeAllSiblings();

    std::vector<Traveller*> leavingTravellers = waypoint->getLeavingTravellers();
    for(int i=0 ; i<(int)leavingTravellers.size() ; i++)
    {
        leavingTravellers[i]->setOrigin(this);
    }

    std::vector<Traveller*> arrivingTravellers = waypoint->getArrivingTravellers();
    for(int i=0 ; i<(int)arrivingTravellers.size() ; i++)
    {
        arrivingTravellers[i]->setDestination(this);
    }
}

void Waypoint::handleShipCollision(Ship* ship)
{
    if(planet_ != NULL)
    {
        ship->setOriginAndDestination(this, planet_);
    }
    else
    {
        if(siblings_.size() > 0)
        {
            ship->setOriginAndDestination(this, getShipDestination());
        }
        else
        {
          ship->startOrbiting(this, radius_ + RADIUS_STEP_SIZE);
        }
    }
}
