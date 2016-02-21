/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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

#include "tool/resource_manager.h"
#include <libxml/xmlreader.h>
#include <string>
#include <iostream>

#include "tool/xml_document.h"
#include "tool/string_tools.h"
#include "game/config.h"
#include "graphic/sprite.h"
#include "graphic/polygon_generator.h"
#include "map/random_map.h"
#include "interface/mouse_cursor.h"

ResourceManager::ProfileMap ResourceManager::profiles;

ResourceManager::ResourceManager() : base_path("")
{
}

ResourceManager::~ResourceManager()
{
  xmlCleanupParser();
  for (ProfileMap::iterator it = profiles.begin(); it != profiles.end(); ++it)
    delete it->second;
}

int ResourceManager::LoadInt(const Profile *profile, const std::string& resource_name) const
{
  int tmp = 0;
  const xmlNode* elem = GetElement(profile, "int", resource_name);
  if (!elem)
    Error("ResourceManager: can't find int resource \""+resource_name+"\" in profile "+profile->filename);
  if (!profile->doc->ReadIntAttr(elem, "value", tmp))
    Error("ResourceManager: int resource \""+resource_name+"\" has no value field in profile "+profile->filename);
  return tmp;
}

Double ResourceManager::LoadDouble(const Profile *profile, const std::string& resource_name) const
{
  Double tmp = ZERO;
  const xmlNode* elem = GetElement(profile, "Double", resource_name);
  if (!elem)
    Error("ResourceManager: can't find Double resource \""+resource_name+"\" in profile "+profile->filename);
  if (!profile->doc->ReadDoubleAttr(elem, "value", tmp))
    Error("ResourceManager: Double resource \""+resource_name+"\" has no value field in profile "+profile->filename);
  return tmp;
}

Color ResourceManager::LoadColor(const Profile *profile, const std::string& resource_name) const
{
  const xmlNode* elem = GetElement(profile, "color", resource_name);
  if (!elem)
    Error("ResourceManager: can't find color resource \""+resource_name+"\" in profile "+profile->filename);

  uint chanel_color[4];
  std::string tmp[4] = { "r", "g", "b", "a" };
  for (int i = 0; i < 4; i++) {
    if (!profile->doc->ReadUintAttr(elem, tmp[i], chanel_color[i]))
      Error("ResourceManager: color resource \""+resource_name+"\" has no "+tmp[i]+" field in profile "+profile->filename);
  }
  return Color(chanel_color[0], chanel_color[1], chanel_color[2], chanel_color[3]);
}

Point2i ResourceManager::LoadPoint2i(const Profile *profile, const std::string& resource_name) const
{
  const xmlNode* elem = GetElement(profile, "point", resource_name);
  if (!elem)
    Error("ResourceManager: can't find point resource \""+resource_name+"\" in profile "+profile->filename);

  uint point[2];
  std::string tmp[2] = { "x", "y" };
  for (int i = 0; i < 2; i++) {
    if (!profile->doc->ReadUintAttr(elem, tmp[i], point[i]))
      Error("ResourceManager: point resource \""+resource_name+"\" has no "+tmp[i]+" field in profile "+profile->filename);
  }
  return Point2i(point[0], point[1]);
}

Point2d ResourceManager::LoadPoint2d(const Profile *profile, const std::string& resource_name) const
{
  const xmlNode* elem = GetElement(profile, "point", resource_name);
  if (!elem)
    Error("ResourceManager: can't find point resource \""+resource_name+"\" in profile "+profile->filename);

  Double point[2];
  std::string tmp[2] = { "x", "y" };
  for (int i = 0; i < 2; i++) {
    if (!profile->doc->ReadDoubleAttr(elem, tmp[i], point[i]))
      Error("ResourceManager: point resource \""+resource_name+"\" has no "+tmp[i]+" field in profile "+profile->filename);
  }
  return Point2d(point[0], point[1]);
}

MouseCursor ResourceManager::LoadMouseCursor(const Profile *profile, const std::string& resource_name,
                                             Mouse::pointer_t _pointer_id) const
{
  const xmlNode* elem = GetElement(profile, "mouse_cursor", resource_name);
  if (!elem)
    Error("ResourceManager: can't find mouse cursor resource \""+resource_name+"\" in profile "+profile->filename);
  std::string filename;
  if (!profile->doc->ReadStringAttr(elem, "file", filename))
    Error("ResourceManager: mouse cursor resource \""+resource_name+"\" has no file field in profile "+profile->filename);

  uint point[2];
  std::string tmp[2] = { "x", "y" };
  for (int i = 0; i < 2; i++) {
    if (!profile->doc->ReadUintAttr(elem, tmp[i], point[i]))
      Error("ResourceManager: mouse cursor resource \""+resource_name+"\" has no "+tmp[i]+" field in profile "+profile->filename);
  }
  Point2i pos(point[0], point[1]);

  MouseCursor mouse_cursor(_pointer_id, profile->relative_path+filename, pos);
  return mouse_cursor;
}

Surface ResourceManager::LoadImage(const std::string& filename,
                                   bool alpha, bool set_colorkey, Uint32 colorkey) const
{
  Surface surface(filename.c_str());

  if (set_colorkey)
    surface.SetColorKey(SDL_SRCCOLORKEY, colorkey);

  return (alpha) ? surface.DisplayFormatAlpha() : surface.DisplayFormat();
}

Profile *ResourceManager::LoadXMLProfile(const std::string& xml_filename, bool is_absolute_path) const
{
  ProfileMap::iterator it = profiles.find(xml_filename);
  if (it != profiles.end() && it->second) {
    MSG_DEBUG("xml.cached_load", "Returning cached %s\n", xml_filename.c_str());
    it->second->ref_count++;
    return it->second;
  }

  MSG_DEBUG("xml.load", "Loading uncached %s\n", xml_filename.c_str());
  XmlReader *doc = new XmlReader;
  std::string filename, path;
  if (!is_absolute_path) {
    path = base_path;
    filename = path + xml_filename;
  } else {
    ASSERT(xml_filename.rfind(PATH_SEPARATOR) != xml_filename.npos);
    path = xml_filename.substr(0, xml_filename.rfind(PATH_SEPARATOR)+1);
    filename = xml_filename;
  }

   // Load the XML
  if (!doc->Load(filename)) {
    // TODO raise an "can't load file" exception
    delete doc;
    Error("ResourceManager: can't load profile "+filename);
    return NULL;
  }

  Profile *profile = new Profile(xml_filename);
  profile->doc = doc;
  profile->filename = xml_filename;
  profile->relative_path = path;
  profiles[xml_filename] = profile;
  return profile;
}

void ResourceManager::UnLoadXMLProfile(Profile *profile) const
{
  // Let's greedily cache all documents!
  --profile->ref_count;
  if (!profile->ref_count) {
    profiles.erase(profile->filename);
    delete profile;
  }
}

const xmlNode*  ResourceManager::GetElement(const Profile *profile, const std::string& resource_type,
                                            const std::string& resource_name) const
{
  const xmlNode* elem = profile->doc->Access(profile->doc->GetRoot(), resource_type, resource_name);

  if (!elem) {
    std::string r_name = resource_name;
    const xmlNode* cur_elem = profile->doc->GetRoot();

    while((r_name.find("/") != r_name.npos) && (cur_elem != NULL)) {
      cur_elem = profile->doc->Access(cur_elem, "section", r_name.substr(0, r_name.find("/")));
      r_name = r_name.substr(r_name.find("/") + 1, r_name.length());
    }
    if (cur_elem)
      elem = profile->doc->Access(cur_elem, resource_type, r_name);
  }
  return elem;
}

std::string ResourceManager::LoadImageFilename(const Profile *profile, const std::string& resource_name) const
{
  const xmlNode* elem = GetElement(profile, "surface", resource_name);
  if (!elem)
    Error("ResourceManager: can't find image resource \""+resource_name+"\" in profile "+profile->filename);

  std::string filename;
  if (!profile->doc->ReadStringAttr(elem, "file", filename))
    Error("ResourceManager: image resource \""+resource_name+"\" has no file field in profile "+profile->filename);

  return profile->relative_path+filename;
}

Surface ResourceManager::LoadImage(const Profile *profile, const std::string& resource_name, bool alpha) const
{
  std::string    filename = LoadImageFilename(profile, resource_name);
  Surface        image    = LoadImage(filename, alpha);
  const xmlNode *elem     = GetElement(profile, "surface", resource_name);
  std::string    str;

  if (XmlReader::ReadStringAttr(elem, "size", str)) {
    int x, y;
    Rectanglei source_rect(0,0, image.GetWidth(), image.GetHeight());

    if (sscanf(str.c_str(), "%i,%i", &x, &y) != 2)
      Error("ResourceManager: can't load image resource \""+resource_name+"\", malformed size attribute " + str);
    source_rect.SetSizeX(x);
    source_rect.SetSizeY(y);

    if (XmlReader::ReadStringAttr(elem, "pos", str)) {
      if (sscanf(str.c_str(), "%i,%i", &x, &y) != 2)
        Error("ResourceManager: can't load image resource \""+resource_name+"\", malformed position attribute " + str);

      source_rect.SetPositionX(x);
      source_rect.SetPositionY(y);
    }

    return image.Crop(source_rect);
  }
  else {
    return image;
  }

  // TODO load more properties in xml : alpha, colorkey....
  //      By now force alpha and no colorkey
}

Sprite *ResourceManager::LoadSprite(const Profile *profile, const std::string& resource_name) const
{
  const xmlNode* elem_sprite = GetElement(profile, "sprite", resource_name);
  if (!elem_sprite)
    Error("ResourceManager: can't find sprite resource \""+resource_name+"\" in profile "+profile->filename);

  return LoadSprite(elem_sprite, resource_name, profile->relative_path);
}

Sprite *ResourceManager::LoadSprite(const xmlNode* elem_sprite, const std::string& resource_name,
                                    const std::string& main_folder) const
{
  const xmlNode* elem_image = XmlReader::GetMarker(elem_sprite, "image");

  if (!elem_image)
    Error("ResourceManager: can't load (sprite) resource " + resource_name);

  std::string image_filename;
  if (!XmlReader::ReadStringAttr(elem_image, "file", image_filename))
    Error("ResourceManager: can't load (sprite) resource " + resource_name);

  // TODO load more properties in xml : alpha, colorkey....
  //      By now force alpha and no colorkey

  bool alpha = true;
  Sprite *sprite = NULL;

  const xmlNode* elem_grid = XmlReader::GetMarker(elem_image, "grid");

  if (!elem_grid) {
    ASSERT(resource_name != "barrel");
    // No grid element, Load the Sprite like a normal image
    Surface surface = LoadImage(main_folder+image_filename, alpha);
    sprite = new Sprite();
    sprite->Init(surface, surface.GetSize(), 1, 1);
  } else {
    Point2i frameSize, offset;
    int nb_frames_x = 1;
    int nb_frames_y = 1;
    std::string str;

    // size is required
    if (!XmlReader::ReadStringAttr(elem_grid, "size", str))
      Error("ResourceManager: can't load sprite resource \""+resource_name+"\", no attribute size");
    if (sscanf(str.c_str(), "%i,%i", &frameSize.x, &frameSize.y) != 2)
      Error("ResourceManager: can't load sprite resource \""+resource_name+"\", malformed size attribute " + str);

    //array is not required, default is 1,1
    if (XmlReader::ReadStringAttr(elem_grid, "array", str)) {
      if (sscanf(str.c_str(), "%i,%i", &nb_frames_x, &nb_frames_y) != 2)
        Error("ResourceManager: can't load (sprite) resource "+resource_name+"\", malformed array attribute " + str);
      if (nb_frames_x <= 0)
        nb_frames_x = 1;
      if (nb_frames_y <= 0)
        nb_frames_y = 1;
    }

    Surface surface = LoadImage(main_folder+image_filename, alpha);
    sprite = new Sprite();
    sprite->Init(surface, frameSize, nb_frames_x, nb_frames_y);
  }

  ASSERT(sprite != NULL);

  const xmlNode* elem = XmlReader::GetMarker(elem_sprite, "animation");
  if (elem != NULL) {
    std::string str;
    // Set the frame speed
    if (XmlReader::ReadStringAttr(elem, "speed", str))
      sprite->SetFrameSpeed(atoi(str.c_str()));

    if (XmlReader::ReadStringAttr(elem, "loop_mode", str)) {
      bool loop_value;
      if (str2bool(str, loop_value))
        sprite->animation.SetLoopMode(loop_value);
      else if (str == "pingpong")
        sprite->animation.SetPingPongMode(true);
      else
        Error("ResourceManager: unrecognized xml option loop_mode=\"" +str+ "\" in resource " + resource_name);
    }

    if (XmlReader::ReadStringAttr(elem, "loop_wait", str)) {
      sprite->animation.SetLoopWait(atoi(str.c_str()));
    }

    if (XmlReader::ReadStringAttr(elem, "loop_wait_random", str)) {
      sprite->animation.SetLoopWaitRandom(atoi(str.c_str()));
    }


  }
  return sprite;
}

std::string ResourceManager::GenerateMap(Profile *profile, InfoMap::Island_type generator,
                                         const int width, const int height) const
{
  RandomMap random_map(profile, width, height);
  random_map.Generate(generator);
  return random_map.SaveMap();
}













