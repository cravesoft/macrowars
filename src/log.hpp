#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <OgreLogManager.h>

// some shortcuts for us
#define LOG(x)       Ogre::LogManager::getSingleton().logMessage(x)
#define LOGSAFE(x)   do { if (Ogre::LogManager::getSingletonPtr()) Ogre::LogManager::getSingleton().logMessage(x);  } while(0) // use this if you log whenever its unsure if Ogre was started properly beforehand
#define TOSTRING(x)  Ogre::StringConverter::toString(x)
#define TOUTFSTRING(x)  ANSI_TO_UTF(Ogre::StringConverter::toString(x))
#define PARSEINT(x)  Ogre::StringConverter::parseInt(x)
#define PARSEREAL(x) Ogre::StringConverter::parseReal(x)

#endif
