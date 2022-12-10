#ifndef __FONT_TEXTURE_HELPER_HPP__
#define __FONT_TEXTURE_HELPER_HPP__

#include "log.hpp"

#include <iostream>
#include <fstream>

#include <OgreFontManager.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTexture.h>
#include <OgreUTFString.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>

#ifndef FONT_UNICODE
# define FONT_UNICODE //highly recommended to be used, otherwise you run into problems with non-ASCII font glyphs
#endif //FONT_UNICODE

// licensed under the do-whatever-you-want license ;)
// created by Thomas Fischer thomas{AT}thomasfischer{DOT}biz

Ogre::UTFString cache_name = "_cache";

// this code is slow, use it sparingly!
int generateFontTexture(Ogre::String font_name, bool load=false)
{
	Ogre::FontPtr font = Ogre::FontManager::getSingleton().getByName(font_name);
	if(font.isNull()) return 1;

	if(font->getType() != Ogre::FT_TRUETYPE) return 1;

	if(!font->isLoaded()) font->load();

	Ogre::String texname = font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();

	Ogre::TexturePtr fontTexture = Ogre::TextureManager::getSingleton().getByName(texname);
	Ogre::HardwarePixelBufferSharedPtr fontBuffer = fontTexture->getBuffer();

	if(fontBuffer.isNull()) return 1;

	// create a remporary buffer that holds the font
	size_t nBuffSize = fontBuffer->getSizeInBytes();
	Ogre::uint8* buffer = (Ogre::uint8*)calloc(nBuffSize, sizeof(Ogre::uint8));
	Ogre::PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(),fontBuffer->getDepth(), fontBuffer->getFormat(), buffer);
	fontBuffer->blitToMemory(fontPb);

	// create new render texture
	Ogre::String texNameManual = "FontTextureRTT_"+font->getName();
	Ogre::TexturePtr t = Ogre::TextureManager::getSingleton().createManual(texNameManual,"General",Ogre::TEX_TYPE_2D,(Ogre::uint)fontBuffer->getWidth(),(Ogre::uint)fontBuffer->getHeight(),(Ogre::uint)fontBuffer->getDepth(),1,fontBuffer->getFormat(), Ogre::TU_RENDERTARGET);
	Ogre::TexturePtr fontTextureRTT = Ogre::TextureManager::getSingleton().getByName(texNameManual);
	fontTextureRTT->getBuffer()->blitFromMemory(fontPb);

	// now render to file
	Ogre::RenderTexture* render_texture = fontTextureRTT->getBuffer()->getRenderTarget();
	render_texture->update();
	Ogre::String outImageName = font->getName()+ cache_name + ".png";
	render_texture->writeContentsToFile(outImageName);

	// free stuff
	free(buffer);
	Ogre::TextureManager::getSingleton().remove(texNameManual);


	// save texture font infos
	Ogre::UTFString out_text = font->getName()+cache_name+"\n{\n\ttype\timage\n\tsource\t"+outImageName+"\n";

	Ogre::vector<Ogre::Font::CodePointRange>::type ranges = font->getCodePointRangeList();
	for(Ogre::vector<Ogre::Font::CodePointRange>::type::iterator it = ranges.begin(); it != ranges.end(); it++)
	{
		// iterate over all known glyphs
		for(Ogre::uint32 i=it->first;i<it->second;i++)
		{
			Ogre::Font::GlyphInfo gi(0, Ogre::Font::UVRect(), 0);
			try
			{
				gi = font->getGlyphInfo(i);
			}
			catch(...)
			{
				// catch code point not found error
				continue;
			}

			// create character
#ifdef FONT_UNICODE
			wchar_t tmp[20];
			swprintf(tmp, 20, L"%c", i);
#else
			char tmp[20] = "";
			sprintf(tmp, "%c", i);
#endif
			out_text.append("\tglyph " + Ogre::UTFString(tmp) + " " + \
						TOSTRING(gi.uvRect.left)+" "+ \
						TOSTRING(gi.uvRect.top)+" "+ \
						TOSTRING(gi.uvRect.right)+" "+ \
						TOSTRING(gi.uvRect.bottom)+"\n");

		}
	}
	out_text.append("}\n");

	Ogre::String defFileName = font->getName() + cache_name + ".fontdef";
	std::ofstream f;
	f.open(defFileName.c_str());
	f << out_text;
	f.close();
	LOG("generated font cache for font "+font_name+" ("+outImageName+", "+defFileName+")");

	if(load)
	{
		// reload to find the .fontdef file
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		// load now
		Ogre::FontPtr cached_font = Ogre::FontManager::getSingleton().getByName(font_name+cache_name);
		if (cached_font.isNull()) return 1;
		cached_font->load();
		return 0;
	}
	return 0;
}


// if you want to use this, use instead of the original texture name, the texture name plus cache_name (see top)
// this function creates the font cache if its not existing
int fontCacheInit(Ogre::String font_name)
{
	Ogre::FontPtr font_org = Ogre::FontManager::getSingleton().getByName(font_name);
	Ogre::FontPtr font_cache = Ogre::FontManager::getSingleton().getByName(font_name + cache_name);

	// base font not even existing?
	if(font_org.isNull()) return 1;
	// using texture fonts is a bit stupid ...
	if(font_org->getType() != Ogre::FT_TRUETYPE) return 1;

	int res = 0;
	if(font_cache.isNull())
    {
		res = generateFontTexture(font_name);
    }

	if (font_org->isLoaded()) font_org->unload();
	return res;
}

int generateAllFontTextures()
{
	Ogre::ResourceManager::ResourceMapIterator itf2 = Ogre::FontManager::getSingleton().getResourceIterator();
	while (itf2.hasMoreElements())
		generateFontTexture(itf2.getNext()->getName(), true);

	LOG("all font textures generated");
	return 0;
}


void example_usage()
{
	// use only for debugging:
	// generateAllFontTextures()

	// example: i want to use the TTF font CyberbitEnglish in my application:
	fontCacheInit("CyberbitEnglish");

	// from now you can use font CyberbitEnglish_cache in your application :)
}

#endif
