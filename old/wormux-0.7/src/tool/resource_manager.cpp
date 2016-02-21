/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Resource Manager: Load resources (images/sprites) suitable for SDL
 *                    Load directly or from refernece in xml resource profile
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de) 
 *             Initial version
 * 
 * TODO:       Keep reference to resources, better exceptions
 *****************************************************************************/

#include "resource_manager.h"
#include <string>
#include <iostream>
#include "error.h"
#include "xml_document.h"
#include "../game/config.h"
#include "../graphic/sprite.h"

Profile::Profile()
{
   doc = NULL;
}

Profile::~Profile()
{
  if (doc != NULL) delete doc;
}


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::AddDataPath( std::string base_path)
{
   this->base_path = base_path;
}

Surface ResourceManager::LoadImage( const std::string filename, 
        bool alpha, bool set_colorkey, Uint32 colorkey)
{
  Surface pre_surface = Surface( filename.c_str() );
  Surface end_surface;
  
  if( set_colorkey ) 
    end_surface.SetColorKey( SDL_SRCCOLORKEY, colorkey);

  if( !alpha )
    end_surface = pre_surface.DisplayFormat();
  else
    end_surface = pre_surface.DisplayFormatAlpha();
   
  return end_surface;
}

Profile *ResourceManager::LoadXMLProfile( const std::string xml_filename, bool relative_path)
{
   LitDocXml *doc = new LitDocXml;
   std::string filename, path;
   if (!relative_path) {
       path = base_path;
       filename = path + xml_filename;
   } else {
       assert ( xml_filename.rfind(PATH_SEPARATOR) != xml_filename.npos );
       path = xml_filename.substr(0, xml_filename.rfind(PATH_SEPARATOR)+1);
       filename = xml_filename;
   }
   
   // Load the XML
   if ( !doc->Charge(filename) )
     {
	// TODO raise an "can't load file" exception
	Error("ResourceManager: can't load profile "+filename);
	return NULL;
     }
  
   Profile *profile = new Profile; 
   profile->doc = doc;
   profile->filename = xml_filename;
   profile->relative_path = path;
   return profile;
}

void ResourceManager::UnLoadXMLProfile( Profile *profile)
{
   delete profile;
}

xmlpp::Element * ResourceManager::GetElement( const Profile *profile, const std::string resource_type, const std::string resource_name)
{
   xmlpp::Element *elem = profile->doc->Access ( profile->doc->racine(),
						 resource_type,
						 resource_name);

   if ( elem == NULL )
     {
	std::string r_name = resource_name;
	xmlpp::Element *cur_elem = profile->doc->racine();

	while ( ( r_name.find("/") != r_name.npos ) && (cur_elem != NULL)) 
	  {
	     cur_elem = profile->doc->Access ( cur_elem, "section", r_name.substr(0, r_name.find("/")));
	     r_name = r_name.substr( r_name.find("/")+1, r_name.length());
	  }
	
	if ( cur_elem)
	     elem = profile->doc->Access ( cur_elem, resource_type, r_name);
	
     }
   
   return elem;   
}

Surface ResourceManager::LoadImage( const Profile *profile, const std::string resource_name)
{     
  xmlpp::Element *elem = GetElement ( profile, "surface", resource_name);
  if ( elem == NULL)
	Error("ResourceManager: can't find image resource \""+resource_name+"\" in profile "+profile->filename);
  
  std::string filename; 
  if ( ! profile->doc->LitAttrString( elem, "file", filename) )
	Error("ResourceManager: image resource \""+resource_name+"\" has no file field in profile "+profile->filename);
   
 // TODO load more properties in xml : alpha, colorkey....
 //      By now force alpha and no colorkey
   
 bool alpha = true;
   
 return LoadImage( profile->relative_path+filename, alpha);
}

Sprite *ResourceManager::LoadSprite( const Profile *profile, const std::string resource_name)
{
  xmlpp::Element *elem_sprite = GetElement( profile, "sprite", resource_name);
  if ( elem_sprite == NULL)
    Error("ResourceManager: can't find sprite resource \""+resource_name+"\" in profile "+profile->filename);;

  xmlpp::Element *elem_image = profile->doc->AccesBalise ( elem_sprite, "image");

  if ( elem_image == NULL )
    Error("ResourceManager: can't load (sprite) resource "+resource_name);

  std::string image_filename; 
  if ( ! profile->doc->LitAttrString( elem_image, "file", image_filename) )
    Error("ResourceManager: can't load (sprite) resource "+resource_name);

  // TODO load more properties in xml : alpha, colorkey....
  //      By now force alpha and no colorkey

  bool alpha = true;
  Sprite *sprite = NULL;
   
  xmlpp::Element *elem_grid = profile->doc->AccesBalise ( elem_image, "grid");

  if ( elem_grid == NULL )
  {
    // No grid element, Load the Sprite like a normal image
    Surface surface = LoadImage( profile->relative_path+image_filename, alpha);
    sprite = new Sprite();
    sprite->Init(surface, surface.GetSize(), 1, 1);
  }
  else
  {	
	Point2i frameSize;
    int nb_frames_x = 0;
    int nb_frames_y = 0;
    std::string size;

    if ( ! profile->doc->LitAttrString( elem_grid, "size", size) )
      Error("ResourceManager: can't load sprite resource \""+resource_name+"\" has no attribute size");
    
	if ( size.find(",") != size.npos)
    {
      frameSize.x = atoi( (size.substr(0,size.find(","))).c_str());
      frameSize.y = atoi( (size.substr(size.find(",")+1,size.length())).c_str());
    }
    else
      Error("ResourceManager: can't load sprite resource \""+resource_name+"\" has malformed size attribute");
	
    std::string array;
    if ( ! profile->doc->LitAttrString( elem_grid, "array", array) )
      Error("ResourceManager: can't load sprite resource \""+resource_name+"\" has no attribute array");
	
	if ( array.find(",") != array.npos)
    {
      nb_frames_x = atoi( (array.substr(0,array.find(","))).c_str());
      if ( nb_frames_x <= 0 )
      nb_frames_x = 1;
	    nb_frames_y = atoi( (array.substr(array.find(",")+1,array.length()-array.find(",")-1)).c_str());;
      if ( nb_frames_y <= 0 )
      nb_frames_y = 1;
    }
    else
      Error("ResourceManager: can't load (sprite) resource "+resource_name);

	Surface surface = LoadImage( profile->relative_path+image_filename, alpha);
    sprite = new Sprite();
    sprite->Init( surface, frameSize, nb_frames_x, nb_frames_y);
  }

  assert(sprite!=NULL);

  xmlpp::Element *elem = profile->doc->AccesBalise ( elem_sprite, "frame");
  if ( elem != NULL )
  {
	  std::string fs_str;
    int fs=0;
    if ( profile->doc->LitAttrString( elem, "speed", fs_str) )
	    fs = atoi( fs_str.c_str());
    sprite->SetFrameSpeed(fs);
  }

  elem = profile->doc->AccesBalise ( elem_sprite, "animation");
  if ( elem != NULL )
  {
	  std::string pp_str;
    if ( profile->doc->LitAttrString( elem, "pingpong", pp_str) )
    if (pp_str == "yes")
      sprite->animation.SetPingPongMode(true);
  }

  elem = profile->doc->AccesBalise ( elem_sprite, "animation");
  if ( elem != NULL )
  {
	  std::string pp_str;
    if ( profile->doc->LitAttrString( elem, "loop_mode", pp_str) )
    if (pp_str == "no")
      sprite->animation.SetLoopMode(false);
  }

  return sprite;
}


ResourceManager resource_manager;

