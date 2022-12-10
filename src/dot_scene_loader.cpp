#include "dot_scene_loader.hpp"
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>

#pragma warning(disable:4390)
#pragma warning(disable:4305)


DotSceneLoader::DotSceneLoader() : scene_manager_(0), terrain_group_(0)
{
    terrain_global_options_ = OGRE_NEW Ogre::TerrainGlobalOptions();
}


DotSceneLoader::~DotSceneLoader()
{
    if(terrain_group_)
    {
        OGRE_DELETE terrain_group_;
    }

    OGRE_DELETE terrain_global_options_;
}

void DotSceneLoader::parseDotScene(const Ogre::String &SceneName,
                                   const Ogre::String &groupName,
                                   Ogre::SceneManager *yourSceneMgr,
                                   Ogre::SceneNode *pAttachNode,
                                   const Ogre::String &sPrependNode)
{
    // set up shared object values
    group_name_ = groupName;
    scene_manager_ = yourSceneMgr;
    prepend_node_ = sPrependNode;
    staticObjects.clear();
    dynamicObjects.clear();

    rapidxml::xml_document<> XMLDoc;    // character type defaults to char

    rapidxml::xml_node<>* XMLRoot;

    Ogre::DataStreamPtr stream =
        Ogre::ResourceGroupManager::getSingleton().openResource(SceneName, groupName);
    char* scene = strdup(stream->getAsString().c_str());
    XMLDoc.parse<0>(scene);

    // Grab the scene node
    XMLRoot = XMLDoc.first_node("scene");

    // Validate the File
    if( getAttrib(XMLRoot, "formatVersion", "") == "")
    {
        Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error: Invalid .scene File. Missing <scene>");
        return;
    }

    // figure out where to attach any nodes we create
    attach_node_ = pAttachNode;
    if(!attach_node_)
        attach_node_ = scene_manager_->getRootSceneNode();

    // Process the scene
    processScene(XMLRoot);
}

void DotSceneLoader::processScene(rapidxml::xml_node<>* XMLRoot)
{
    // Process the scene parameters
    Ogre::String version = getAttrib(XMLRoot, "formatVersion", "unknown");

    Ogre::String message = "[DotSceneLoader] Parsing dotScene file with version " + version;
    if(XMLRoot->first_attribute("ID"))
    {
        message += ", id " + Ogre::String(XMLRoot->first_attribute("ID")->value());
    }
    if(XMLRoot->first_attribute("sceneManager"))
    {
        message += ", scene manager " + Ogre::String(XMLRoot->first_attribute("sceneManager")->value());
    }
    if(XMLRoot->first_attribute("minOgreVersion"))
    {
        message += ", min. Ogre version " + Ogre::String(XMLRoot->first_attribute("minOgreVersion")->value());
    }
    if(XMLRoot->first_attribute("author"))
    {
        message += ", author " + Ogre::String(XMLRoot->first_attribute("author")->value());
    }

    Ogre::LogManager::getSingleton().logMessage(message);

    rapidxml::xml_node<>* element;

    // Process environment (?)
    element = XMLRoot->first_node("environment");
    if(element)
    {
        processEnvironment(element);
    }

    // Process nodes (?)
    element = XMLRoot->first_node("nodes");
    if(element)
    {
        processNodes(element);
    }

    // Process externals (?)
    element = XMLRoot->first_node("externals");
    if(element)
    {
        processExternals(element);
    }

    // Process userDataReference (?)
    element = XMLRoot->first_node("userDataReference");
    if(element)
    {
        processUserDataReference(element);
    }

    // Process octree (?)
    element = XMLRoot->first_node("octree");
    if(element)
    {
        processOctree(element);
    }

    // Process light (?)
    //element = XMLRoot->first_node("light");
    //if(element)
    //   processLight(element);

    // Process camera (?)
    element = XMLRoot->first_node("camera");
    if(element)
    {
        processCamera(element);
    }

    // Process terrain (?)
    element = XMLRoot->first_node("terrain");
    if(element)
    {
        processTerrain(element);
    }
}

void DotSceneLoader::processNodes(rapidxml::xml_node<>* XMLNode)
{
    rapidxml::xml_node<>* element;

    // Process node (*)
    element = XMLNode->first_node("node");
    while(element)
    {
        processNode(element);
        element = element->next_sibling("node");
    }

    // Process position (?)
    element = XMLNode->first_node("position");
    if(element)
    {
        attach_node_->setPosition(parseVector3(element));
        attach_node_->setInitialState();
    }

    // Process rotation (?)
    element = XMLNode->first_node("rotation");
    if(element)
    {
        attach_node_->setOrientation(parseQuaternion(element));
        attach_node_->setInitialState();
    }

    // Process scale (?)
    element = XMLNode->first_node("scale");
    if(element)
    {
        attach_node_->setScale(parseVector3(element));
        attach_node_->setInitialState();
    }
}

void DotSceneLoader::processExternals(rapidxml::xml_node<>* XMLNode)
{
    //! @todo Implement this
}

void DotSceneLoader::processEnvironment(rapidxml::xml_node<>* XMLNode)
{
    rapidxml::xml_node<>* element;

    // Process camera (?)
    element = XMLNode->first_node("camera");
    if(element)
        processCamera(element);

    // Process fog (?)
    element = XMLNode->first_node("fog");
    if(element)
        processFog(element);

    // Process skyBox (?)
    element = XMLNode->first_node("skyBox");
    if(element)
        processSkyBox(element);

    // Process skyDome (?)
    element = XMLNode->first_node("skyDome");
    if(element)
        processSkyDome(element);

    // Process skyPlane (?)
    element = XMLNode->first_node("skyPlane");
    if(element)
        processSkyPlane(element);

    // Process clipping (?)
    element = XMLNode->first_node("clipping");
    if(element)
        processClipping(element);

    // Process colourAmbient (?)
    element = XMLNode->first_node("colourAmbient");
    if(element)
        scene_manager_->setAmbientLight(parseColour(element));

    // Process colourBackground (?)
    //! @todo Set the background colour of all viewports (RenderWindow has to be provided then)
    element = XMLNode->first_node("colourBackground");
    if(element)
        ;//scene_manager_->set(parseColour(element));

    // Process userDataReference (?)
    element = XMLNode->first_node("userDataReference");
    if(element)
        processUserDataReference(element);
}

void DotSceneLoader::processTerrain(rapidxml::xml_node<>* XMLNode)
{
    Ogre::Real worldSize = getAttribReal(XMLNode, "worldSize");
    int mapSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
    int rows = Ogre::StringConverter::parseInt(XMLNode->first_attribute("rows")->value());
    int columns = Ogre::StringConverter::parseInt(XMLNode->first_attribute("columns")->value());
    bool colourmapEnabled = getAttribBool(XMLNode, "colourmapEnabled");
    int colourMapTextureSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());

    Ogre::Vector3 lightdir(0, -0.3, 0.75);
    lightdir.normalise();
    Ogre::Light* l = scene_manager_->createLight("tstLight");
    l->setType(Ogre::Light::LT_DIRECTIONAL);
    l->setDirection(lightdir);
    l->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
    l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    scene_manager_->setAmbientLight(Ogre::ColourValue(0.6, 0.6, 0.6));

    terrain_global_options_->setMaxPixelError(1);
    terrain_global_options_->setCompositeMapDistance(2000);
    terrain_global_options_->setLightMapDirection(lightdir);
    terrain_global_options_->setCompositeMapAmbient(scene_manager_->getAmbientLight());
    terrain_global_options_->setCompositeMapDiffuse(l->getDiffuseColour());

    terrain_group_ = OGRE_NEW Ogre::TerrainGroup(scene_manager_, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize);
    terrain_group_->setOrigin(Ogre::Vector3::ZERO);

    rapidxml::xml_node<>* element;
    rapidxml::xml_node<>* pPageElement;

    // Process terrain pages (*)
    element = XMLNode->first_node("terrainPages");
    while(element)
    {
        pPageElement = element->first_node("terrainPage");
        while(pPageElement)
        {
            processTerrainPage(pPageElement);
            pPageElement = pPageElement->next_sibling("terrainPage");
        }
        element = element->next_sibling("terrainPages");
    }
    terrain_group_->loadAllTerrains(true);

    // process blendmaps
    element = XMLNode->first_node("terrainPages");
    while(element)
    {
        pPageElement = element->first_node("terrainPage");
        while(pPageElement)
        {
            processBlendmaps(pPageElement);
            pPageElement = pPageElement->next_sibling("terrainPage");
        }
        element = element->next_sibling("terrainPages");
    }
    terrain_group_->freeTemporaryResources();
    //mTerrain->setPosition(terrain_position_);
}

void DotSceneLoader::processTerrainPage(rapidxml::xml_node<>* XMLNode)
{
    Ogre::String name = getAttrib(XMLNode, "name");
    int pageX = Ogre::StringConverter::parseInt(XMLNode->first_attribute("pageX")->value());
    int pageY = Ogre::StringConverter::parseInt(XMLNode->first_attribute("pageY")->value());
    Ogre::Real worldSize = getAttribReal(XMLNode, "worldSize");
    int mapSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
    bool colourmapEnabled = getAttribBool(XMLNode, "colourmapEnabled");
    int colourmapTexturesize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourmapTexturesize")->value());
    int layerCount = Ogre::StringConverter::parseInt(XMLNode->first_attribute("layerCount")->value());

    Ogre::String filename = terrain_group_->generateFilename(pageX, pageY);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(terrain_group_->getResourceGroup(), filename))
    {
        terrain_group_->defineTerrain(pageX, pageY);
    }
    else
    {
        rapidxml::xml_node<>* element;

        element = XMLNode->first_node("position");
        if(element)
            terrain_position_ = parseVector3(element);

        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(name + Ogre::String(".ohm"), "General" );
        size_t size = stream.get()->size();
        if(size != mapSize * mapSize * 4)
        {
            OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Size of stream does not match terrainsize!", "TerrainPage" );
        }
        float* buffer = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_GEOMETRY);
        stream->read(buffer, size);

        Ogre::Terrain::ImportData& imp = terrain_group_->getDefaultImportSettings();
        imp.terrainSize = mapSize;
        imp.worldSize = worldSize;
        imp.inputFloat = buffer;
        imp.inputImage = 0;
        imp.deleteInputData = true;
        imp.minBatchSize = 33;
        imp.maxBatchSize = 65;

        imp.layerList.resize(layerCount);

        int count = 0;

        // Process layers (*)
        rapidxml::xml_node<>* pTerrainLayer;
        rapidxml::xml_node<>* pTerrainTextures;
        rapidxml::xml_node<>* pTerrainTexture;
        element = XMLNode->first_node("layers");
        while(element)
        {
            pTerrainLayer = element->first_node("layer");
            while(pTerrainLayer)
            {
                int worldSize = Ogre::StringConverter::parseInt(pTerrainLayer->first_attribute("worldSize")->value());
                pTerrainTextures = pTerrainLayer->first_node("textures");
                pTerrainTexture = pTerrainTextures->first_node("texture");
                while(pTerrainTexture)
                {
                    imp.layerList[count].textureNames.push_back(getAttrib(pTerrainTexture,"name",""));
                    imp.layerList[count].worldSize = (Ogre::Real)worldSize;
                    pTerrainTexture = pTerrainTexture->next_sibling("texture");
                }
                count++;
                // do stuff
                pTerrainLayer = pTerrainLayer->next_sibling("layer");
            }
            element = element->next_sibling("layers");
        }

        terrain_group_->defineTerrain(pageX, pageY, &imp);
    }
}

void DotSceneLoader::processBlendmaps(rapidxml::xml_node<>* XMLNode)
{
    int pageX = Ogre::StringConverter::parseInt(XMLNode->first_attribute("pageX")->value());
    int pageY = Ogre::StringConverter::parseInt(XMLNode->first_attribute("pageY")->value());

    Ogre::String filename = terrain_group_->generateFilename(pageX, pageY);
    // skip this is terrain page has been saved already
    if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(terrain_group_->getResourceGroup(), filename))
    {
        rapidxml::xml_node<>* element;

        // Process blendmaps (*)
        std::vector<Ogre::String> blendMaps;
        rapidxml::xml_node<>* pBlendmap;
        element = XMLNode->first_node("blendMaps");
        pBlendmap = element->first_node("blendMap");
        while(pBlendmap)
        {
            blendMaps.push_back(getAttrib(pBlendmap, "texture",""));
            pBlendmap = pBlendmap->next_sibling("blendMap");
        }

        for(int j = 1;j < terrain_group_->getTerrain(pageX, pageY)->getLayerCount();j++)
        {
            Ogre::TerrainLayerBlendMap *blendmap = terrain_group_->getTerrain(pageX, pageY)->getLayerBlendMap(j);
            Ogre::Image img;
            img.load(blendMaps[j-1],"General");
            int blendmapsize = terrain_group_->getTerrain(pageX, pageY)->getLayerBlendMapSize();
            if(img.getWidth() != blendmapsize)
                img.resize(blendmapsize, blendmapsize);

            float *ptr = blendmap->getBlendPointer();
            Ogre::uint8 *data = static_cast<Ogre::uint8*>(img.getPixelBox().data);

            for(int bp = 0;bp < blendmapsize * blendmapsize;bp++)
                ptr[bp] = static_cast<float>(data[bp]) / 255.0f;

            blendmap->dirty();
            blendmap->update();
        }
    }
}

void DotSceneLoader::processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    //! @todo Implement this
}

void DotSceneLoader::processOctree(rapidxml::xml_node<>* XMLNode)
{
    //! @todo Implement this
}

void DotSceneLoader::processLight(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    // Process attributes
    Ogre::String name = getAttrib(XMLNode, "name");
    Ogre::String id = getAttrib(XMLNode, "id");

    // Create the light
    Ogre::Light *pLight = scene_manager_->createLight(name);
    if(pParent)
        pParent->attachObject(pLight);

    Ogre::String sValue = getAttrib(XMLNode, "type");
    if(sValue == "point")
        pLight->setType(Ogre::Light::LT_POINT);
    else if(sValue == "directional")
        pLight->setType(Ogre::Light::LT_DIRECTIONAL);
    else if(sValue == "spot")
        pLight->setType(Ogre::Light::LT_SPOTLIGHT);
    else if(sValue == "radPoint")
        pLight->setType(Ogre::Light::LT_POINT);

    pLight->setVisible(getAttribBool(XMLNode, "visible", true));
    pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));

    rapidxml::xml_node<>* element;

    // Process position (?)
    element = XMLNode->first_node("position");
    if(element)
        pLight->setPosition(parseVector3(element));

    // Process normal (?)
    element = XMLNode->first_node("normal");
    if(element)
        pLight->setDirection(parseVector3(element));

    element = XMLNode->first_node("directionVector");
    if(element)
    {
        pLight->setDirection(parseVector3(element));
        light_direction_ = parseVector3(element);
    }

    // Process colourDiffuse (?)
    element = XMLNode->first_node("colourDiffuse");
    if(element)
        pLight->setDiffuseColour(parseColour(element));

    // Process colourSpecular (?)
    element = XMLNode->first_node("colourSpecular");
    if(element)
        pLight->setSpecularColour(parseColour(element));

    if(sValue != "directional")
    {
        // Process lightRange (?)
        element = XMLNode->first_node("lightRange");
        if(element)
            processLightRange(element, pLight);

        // Process lightAttenuation (?)
        element = XMLNode->first_node("lightAttenuation");
        if(element)
            processLightAttenuation(element, pLight);
    }
    // Process userDataReference (?)
    element = XMLNode->first_node("userDataReference");
    if(element)
        ;//processUserDataReference(element, pLight);
}

void DotSceneLoader::processCamera(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    // Process attributes
    Ogre::String name = getAttrib(XMLNode, "name");
    Ogre::String id = getAttrib(XMLNode, "id");
    Ogre::Real fov = getAttribReal(XMLNode, "fov", 45);
    Ogre::Real aspectRatio = getAttribReal(XMLNode, "aspectRatio", 1.3333);
    Ogre::String projectionType = getAttrib(XMLNode, "projectionType", "perspective");

    // Create the camera
    Ogre::Camera *pCamera = scene_manager_->createCamera(name);

    //TODO: make a flag or attribute indicating whether or not the camera should be attached to any parent node.
    //if(pParent)
    //   pParent->attachObject(pCamera);

    // Set the field-of-view
    //! @todo Is this always in degrees?
    //pCamera->setFOVy(Ogre::Degree(fov));

    // Set the aspect ratio
    //pCamera->setAspectRatio(aspectRatio);

    // Set the projection type
    if(projectionType == "perspective")
        pCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
    else if(projectionType == "orthographic")
        pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

    rapidxml::xml_node<>* element;

    // Process clipping (?)
    element = XMLNode->first_node("clipping");
    if(element)
    {
        Ogre::Real nearDist = getAttribReal(element, "near");
        pCamera->setNearClipDistance(nearDist);

        Ogre::Real farDist =  getAttribReal(element, "far");
        pCamera->setFarClipDistance(farDist);
    }

    // Process position (?)
    element = XMLNode->first_node("position");
    if(element)
        pCamera->setPosition(parseVector3(element));

    // Process rotation (?)
    element = XMLNode->first_node("rotation");
    if(element)
        pCamera->setOrientation(parseQuaternion(element));

    // Process normal (?)
    element = XMLNode->first_node("normal");
    if(element)
        ;//!< @todo What to do with this element?

    // Process lookTarget (?)
    element = XMLNode->first_node("lookTarget");
    if(element)
        ;//!< @todo Implement the camera look target

    // Process trackTarget (?)
    element = XMLNode->first_node("trackTarget");
    if(element)
        ;//!< @todo Implement the camera track target

    // Process userDataReference (?)
    element = XMLNode->first_node("userDataReference");
    if(element)
        ;//!< @todo Implement the camera user data reference

    // construct a scenenode is no parent
    if(!pParent)
    {
        Ogre::SceneNode* pNode = attach_node_->createChildSceneNode(name);
        pNode->setPosition(pCamera->getPosition());
        pNode->setOrientation(pCamera->getOrientation());
        pNode->scale(1,1,1);
    }
}

void DotSceneLoader::processNode(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    // Construct the node's name
    Ogre::String name = prepend_node_ + getAttrib(XMLNode, "name");

    // Create the scene node
    Ogre::SceneNode *pNode;
    if(name.empty())
    {
        // Let Ogre choose the name
        if(pParent)
            pNode = pParent->createChildSceneNode();
        else
            pNode = attach_node_->createChildSceneNode();
    }
    else
    {
        // Provide the name
        if(pParent)
            pNode = pParent->createChildSceneNode(name);
        else
            pNode = attach_node_->createChildSceneNode(name);
    }

    // Process other attributes
    Ogre::String id = getAttrib(XMLNode, "id");
    bool isTarget = getAttribBool(XMLNode, "isTarget");

    rapidxml::xml_node<>* element;

    // Process position (?)
    element = XMLNode->first_node("position");
    if(element)
    {
        pNode->setPosition(parseVector3(element));
        pNode->setInitialState();
    }

    // Process rotation (?)
    element = XMLNode->first_node("rotation");
    if(element)
    {
        pNode->setOrientation(parseQuaternion(element));
        pNode->setInitialState();
    }

    // Process scale (?)
    element = XMLNode->first_node("scale");
    if(element)
    {
        pNode->setScale(parseVector3(element));
        pNode->setInitialState();
    }

    // Process lookTarget (?)
    element = XMLNode->first_node("lookTarget");
    if(element)
        processLookTarget(element, pNode);

    // Process trackTarget (?)
    element = XMLNode->first_node("trackTarget");
    if(element)
        processTrackTarget(element, pNode);

    // Process node (*)
    element = XMLNode->first_node("node");
    while(element)
    {
        processNode(element, pNode);
        element = element->next_sibling("node");
    }

    // Process entity (*)
    element = XMLNode->first_node("entity");
    while(element)
    {
        processEntity(element, pNode);
        element = element->next_sibling("entity");
    }

    // Process light (*)
    //element = XMLNode->first_node("light");
    //while(element)
    //{
    //   processLight(element, pNode);
    //   element = element->next_sibling("light");
    //}

    // Process camera (*)
    element = XMLNode->first_node("camera");
    while(element)
    {
        processCamera(element, pNode);
        element = element->next_sibling("camera");
    }

    // Process particleSystem (*)
    element = XMLNode->first_node("particleSystem");
    while(element)
    {
        processParticleSystem(element, pNode);
        element = element->next_sibling("particleSystem");
    }

    // Process billboardSet (*)
    element = XMLNode->first_node("billboardSet");
    while(element)
    {
        processBillboardSet(element, pNode);
        element = element->next_sibling("billboardSet");
    }

    // Process plane (*)
    element = XMLNode->first_node("plane");
    while(element)
    {
        processPlane(element, pNode);
        element = element->next_sibling("plane");
    }

    // Process userDataReference (?)
    element = XMLNode->first_node("userDataReference");
    if(element)
        processUserDataReference(element, pNode);
}

void DotSceneLoader::processLookTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    //! @todo Is this correct? Cause I don't have a clue actually

    // Process attributes
    Ogre::String nodeName = getAttrib(XMLNode, "nodeName");

    Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
    Ogre::String sValue = getAttrib(XMLNode, "relativeTo");
    if(sValue == "local")
        relativeTo = Ogre::Node::TS_LOCAL;
    else if(sValue == "parent")
        relativeTo = Ogre::Node::TS_PARENT;
    else if(sValue == "world")
        relativeTo = Ogre::Node::TS_WORLD;

    rapidxml::xml_node<>* element;

    // Process position (?)
    Ogre::Vector3 position;
    element = XMLNode->first_node("position");
    if(element)
        position = parseVector3(element);

    // Process localDirection (?)
    Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;
    element = XMLNode->first_node("localDirection");
    if(element)
        localDirection = parseVector3(element);

    // Setup the look target
    try
    {
        if(!nodeName.empty())
        {
            Ogre::SceneNode *pLookNode = scene_manager_->getSceneNode(nodeName);
            position = pLookNode->_getDerivedPosition();
        }

        pParent->lookAt(position, relativeTo, localDirection);
    }
    catch(Ogre::Exception &/*e*/)
    {
        Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
    }
}

void DotSceneLoader::processTrackTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    // Process attributes
    Ogre::String nodeName = getAttrib(XMLNode, "nodeName");

    rapidxml::xml_node<>* element;

    // Process localDirection (?)
    Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;
    element = XMLNode->first_node("localDirection");
    if(element)
        localDirection = parseVector3(element);

    // Process offset (?)
    Ogre::Vector3 offset = Ogre::Vector3::ZERO;
    element = XMLNode->first_node("offset");
    if(element)
        offset = parseVector3(element);

    // Setup the track target
    try
    {
        Ogre::SceneNode *pTrackNode = scene_manager_->getSceneNode(nodeName);
        pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
    }
    catch(Ogre::Exception &/*e*/)
    {
        Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
    }
}

void DotSceneLoader::processEntity(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    // Process attributes
    Ogre::String name = getAttrib(XMLNode, "name");
    Ogre::String id = getAttrib(XMLNode, "id");
    Ogre::String meshFile = getAttrib(XMLNode, "meshFile");
    Ogre::String materialFile = getAttrib(XMLNode, "materialFile");
    bool isStatic = getAttribBool(XMLNode, "static", false);;
    bool castShadows = getAttribBool(XMLNode, "castShadows", true);

    // TEMP: Maintain a list of static and dynamic objects
    if(isStatic)
        staticObjects.push_back(name);
    else
        dynamicObjects.push_back(name);

    rapidxml::xml_node<>* element;

    // Process vertexBuffer (?)
    element = XMLNode->first_node("vertexBuffer");
    if(element)
        ;//processVertexBuffer(element);

    // Process indexBuffer (?)
    element = XMLNode->first_node("indexBuffer");
    if(element)
        ;//processIndexBuffer(element);

    // Create the entity
    Ogre::Entity *pEntity = 0;
    try
    {
        Ogre::MeshManager::getSingleton().load(meshFile, group_name_);
        pEntity = scene_manager_->createEntity(name, meshFile);
        pEntity->setCastShadows(castShadows);
        pParent->attachObject(pEntity);

        if(!materialFile.empty())
            pEntity->setMaterialName(materialFile);
    }
    catch(Ogre::Exception &/*e*/)
    {
        Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
    }

    // Process userDataReference (?)
    element = XMLNode->first_node("userDataReference");
    if(element)
        processUserDataReference(element, pEntity);


}

void DotSceneLoader::processParticleSystem(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    // Process attributes
    Ogre::String name = getAttrib(XMLNode, "name");
    Ogre::String id = getAttrib(XMLNode, "id");
    Ogre::String file = getAttrib(XMLNode, "file");

    // Create the particle system
    try
    {
        Ogre::ParticleSystem *pParticles = scene_manager_->createParticleSystem(name, file);
        pParent->attachObject(pParticles);
    }
    catch(Ogre::Exception &/*e*/)
    {
        Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
    }
}

void DotSceneLoader::processBillboardSet(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    //! @todo Implement this
}

void DotSceneLoader::processPlane(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
    //! @todo Implement this
}

void DotSceneLoader::processFog(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    Ogre::Real expDensity = getAttribReal(XMLNode, "density", 0.001);
    Ogre::Real linearStart = getAttribReal(XMLNode, "start", 0.0);
    Ogre::Real linearEnd = getAttribReal(XMLNode, "end", 1.0);

    Ogre::FogMode mode = Ogre::FOG_NONE;
    Ogre::String sMode = getAttrib(XMLNode, "mode");
    if(sMode == "none")
        mode = Ogre::FOG_NONE;
    else if(sMode == "exp")
        mode = Ogre::FOG_EXP;
    else if(sMode == "exp2")
        mode = Ogre::FOG_EXP2;
    else if(sMode == "linear")
        mode = Ogre::FOG_LINEAR;

    rapidxml::xml_node<>* element;

    // Process colourDiffuse (?)
    Ogre::ColourValue colourDiffuse = Ogre::ColourValue::White;
    element = XMLNode->first_node("colour");
    if(element)
        colourDiffuse = parseColour(element);

    // Setup the fog
    scene_manager_->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void DotSceneLoader::processSkyBox(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    Ogre::String material = getAttrib(XMLNode, "material", "BaseWhite");
    Ogre::Real distance = getAttribReal(XMLNode, "distance", 5000);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
    bool active = getAttribBool(XMLNode, "active", false);
    if(!active)
        return;

    rapidxml::xml_node<>* element;

    // Process rotation (?)
    Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
    element = XMLNode->first_node("rotation");
    if(element)
        rotation = parseQuaternion(element);

    // Setup the sky box
    scene_manager_->setSkyBox(true, material, distance, drawFirst, rotation, group_name_);
}

void DotSceneLoader::processSkyDome(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    Ogre::String material = XMLNode->first_attribute("material")->value();
    Ogre::Real curvature = getAttribReal(XMLNode, "curvature", 10);
    Ogre::Real tiling = getAttribReal(XMLNode, "tiling", 8);
    Ogre::Real distance = getAttribReal(XMLNode, "distance", 4000);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    rapidxml::xml_node<>* element;

    // Process rotation (?)
    Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
    element = XMLNode->first_node("rotation");
    if(element)
    {
        rotation = parseQuaternion(element);
    }

    // Setup the sky dome
    scene_manager_->setSkyDome(true, material, curvature, tiling, distance,
                               drawFirst, rotation, 16, 16, -1, group_name_);
}

void DotSceneLoader::processSkyPlane(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    Ogre::String material = getAttrib(XMLNode, "material");
    Ogre::Real planeX = getAttribReal(XMLNode, "planeX", 0);
    Ogre::Real planeY = getAttribReal(XMLNode, "planeY", -1);
    Ogre::Real planeZ = getAttribReal(XMLNode, "planeX", 0);
    Ogre::Real planeD = getAttribReal(XMLNode, "planeD", 5000);
    Ogre::Real scale = getAttribReal(XMLNode, "scale", 1000);
    Ogre::Real bow = getAttribReal(XMLNode, "bow", 0);
    Ogre::Real tiling = getAttribReal(XMLNode, "tiling", 10);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    // Setup the sky plane
    Ogre::Plane plane;
    plane.normal = Ogre::Vector3(planeX, planeY, planeZ);
    plane.d = planeD;
    scene_manager_->setSkyPlane(true, plane, material, scale, tiling, drawFirst,
                                bow, 1, 1, group_name_);
}

void DotSceneLoader::processClipping(rapidxml::xml_node<>* XMLNode)
{
    //! @todo Implement this

    // Process attributes
    Ogre::Real fNear = getAttribReal(XMLNode, "near", 0);
    Ogre::Real fFar = getAttribReal(XMLNode, "far", 1);
}

void DotSceneLoader::processLightRange(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight)
{
    // Process attributes
    Ogre::Real inner = getAttribReal(XMLNode, "inner");
    Ogre::Real outer = getAttribReal(XMLNode, "outer");
    Ogre::Real falloff = getAttribReal(XMLNode, "falloff", 1.0);

    // Setup the light range
    pLight->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
}

void DotSceneLoader::processLightAttenuation(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight)
{
    // Process attributes
    Ogre::Real range = getAttribReal(XMLNode, "range");
    Ogre::Real constant = getAttribReal(XMLNode, "constant");
    Ogre::Real linear = getAttribReal(XMLNode, "linear");
    Ogre::Real quadratic = getAttribReal(XMLNode, "quadratic");

    // Setup the light attenuation
    pLight->setAttenuation(range, constant, linear, quadratic);
}


Ogre::String DotSceneLoader::getAttrib(rapidxml::xml_node<>* XMLNode, const Ogre::String &attrib, const Ogre::String &defaultValue)
{
    if(XMLNode->first_attribute(attrib.c_str()))
        return XMLNode->first_attribute(attrib.c_str())->value();
    else
        return defaultValue;
}

Ogre::Real DotSceneLoader::getAttribReal(rapidxml::xml_node<>* XMLNode, const Ogre::String &attrib, Ogre::Real defaultValue)
{
    if(XMLNode->first_attribute(attrib.c_str()))
        return Ogre::StringConverter::parseReal(XMLNode->first_attribute(attrib.c_str())->value());
    else
        return defaultValue;
}

bool DotSceneLoader::getAttribBool(rapidxml::xml_node<>* XMLNode, const Ogre::String &attrib, bool defaultValue)
{
    if(!XMLNode->first_attribute(attrib.c_str()))
        return defaultValue;

    if(Ogre::String(XMLNode->first_attribute(attrib.c_str())->value()) == "true")
        return true;

    return false;
}

Ogre::Vector3 DotSceneLoader::parseVector3(rapidxml::xml_node<>* XMLNode)
{
    return Ogre::Vector3(
        Ogre::StringConverter::parseReal(XMLNode->first_attribute("x")->value()),
        Ogre::StringConverter::parseReal(XMLNode->first_attribute("y")->value()),
        Ogre::StringConverter::parseReal(XMLNode->first_attribute("z")->value())
        );
}

Ogre::Quaternion DotSceneLoader::parseQuaternion(rapidxml::xml_node<>* XMLNode)
{
    //! @todo Fix this crap!

    Ogre::Quaternion orientation;

    if(XMLNode->first_attribute("qx"))
    {
        orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
        orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
        orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
        orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
    }
    if(XMLNode->first_attribute("qw"))
    {
        orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
        orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
        orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
        orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
    }
    else if(XMLNode->first_attribute("axisX"))
    {
        Ogre::Vector3 axis;
        axis.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("axisX")->value());
        axis.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("axisY")->value());
        axis.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("axisZ")->value());
        Ogre::Real angle = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angle")->value());;
        orientation.FromAngleAxis(Ogre::Angle(angle), axis);
    }
    else if(XMLNode->first_attribute("angleX"))
    {
        Ogre::Vector3 axis;
        axis.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angleX")->value());
        axis.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angleY")->value());
        axis.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angleZ")->value());
        //orientation.FromAxes(&axis);
        //orientation.F
    }
    else if(XMLNode->first_attribute("x"))
    {
        orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("x")->value());
        orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("y")->value());
        orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("z")->value());
        orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("w")->value());
    }
    else if(XMLNode->first_attribute("w"))
    {
        orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("w")->value());
        orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("x")->value());
        orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("y")->value());
        orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("z")->value());
    }

    return orientation;
}

Ogre::ColourValue DotSceneLoader::parseColour(rapidxml::xml_node<>* XMLNode)
{
    return Ogre::ColourValue(
        Ogre::StringConverter::parseReal(XMLNode->first_attribute("r")->value()),
        Ogre::StringConverter::parseReal(XMLNode->first_attribute("g")->value()),
        Ogre::StringConverter::parseReal(XMLNode->first_attribute("b")->value()),
        XMLNode->first_attribute("a") != NULL ? Ogre::StringConverter::parseReal(XMLNode->first_attribute("a")->value()) : 1
        );
}

Ogre::String DotSceneLoader::getProperty(const Ogre::String &ndNm, const Ogre::String &prop)
{
    for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ )
    {
        if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop )
        {
            return nodeProperties[i].valueName;
        }
    }

    return "";
}

void DotSceneLoader::processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::Entity *pEntity)
{
    Ogre::String str = XMLNode->first_attribute("id")->value();
    pEntity->setUserAny(Ogre::Any(str));
}
