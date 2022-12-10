void Macro::load()
{
    // load the common resource group:
    OBoy::ResourceManager *rm = OBoy::Environment::instance()->getResourceManager();
    rm->parseResourceFile("res/resources.xml",NULL);
    rm->loadResourceGroup("common");
    rm->createResourceGroup("textures");
}

void Macro::loadComplete()
{
    // start listening to the keyboard:
    OBoy::Environment::instance()->getKeyboard(0)->addListener(this);

    // start listening to the mouse:
    OBoy::Environment::instance()->getMouse(0)->addListener(this);

    // set the load complete flag (this will trigger
    // the start of the game in the update method):
    load_complete_ = true;

    // fetch whatever resources we need:
    font_ = rm->getFont("FONT_MAIN");
    boom_sound_ = rm->getSound("SOUND_BOOM");
    fire_sound_ = rm->getSound("SOUND_FIRE");
    thrust_sound_ = rm->getSound("SOUND_THRUST");
    explosion_sound_ = rm->getSound("SOUND_EXPLOSION");
}

void Macro::drawBackground(OBoy::Graphics *g, OBoyLib::Vector2 camera)
{
    g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
    g->setColor(OBoyLib::Color::LightPink);
    g->drawRect(0, 0, static_cast<int>(world_size_.x()), static_cast<int>(world_size_.y()));
#if 0
  g->setLineWidth(3.0f);
  g->pushTransform();
        g->translate(-camera.x(), -camera.y());
    g->drawLines(grid_);
    g->popTransform();
  g->setLineWidth(1.0f);
#endif
  g->setDrawMode(OBoy::Graphics::DRAWMODE_NORMAL);
}

void Macro::drawMenu(OBoy::Graphics *g)
{
  if (load_complete_)
    {
        /*OBoy::UString str1("MACROWARS");
        OBoy::UString str2("press ENTER to start");
        float str2scale = 0.8f;
        float x1 = (OBoy::Environment::screenWidth() - font_->getStringWidth(str1)) / 2.0f;
        float x2 = (OBoy::Environment::screenWidth() - font_->getStringWidth(str2)*str2scale) / 2.0f;
        g->pushTransform();
            g->translate(x1, OBoy::Environment::screenHeight()/2.0f - 50.0f);
            font_->drawString(g,str1);
        g->popTransform();
        g->pushTransform();
            g->translate(x2, OBoy::Environment::screenHeight()/2.0f + 50.0f);
            font_->drawString(g,str2,str2scale);
        g->popTransform();*/
    }

  // draw the gui:
  gui_system_->draw();
}

void Macro::drawGame(OBoy::Graphics *g)
{
  // draw the background:
  drawBackground(g, camera_);

    // draw the asteroids:
    for (int i=0 ; i<(int)asteroids_.size() ; i++)
    {
        asteroids_[i]->draw(g, camera_);
    }

    // draw the planets:
    for (int i=0 ; i<(int)planets_.size() ; i++)
    {
        planets_[i]->draw(g, camera_, draw_min_hp_, main_player_);
    }

    // draw the ships:
  if (draw_ships_)
  {
      for (int i=0 ; i<(int)ships_.size() ; i++)
      {
          ships_[i]->draw(g, camera_);
      }
  }

  // draw the waypoints:
  if (main_player_ != NULL && draw_waypoints_)
    main_player_->drawWaypoints(g, camera_);

  // draw the gui:
  gui_system_->draw();
}

void Macro::mouseWheel(OBoy::Mouse *mouse, int wheelDelta)
{
  //mZoomFactor += wheelDelta;
}
