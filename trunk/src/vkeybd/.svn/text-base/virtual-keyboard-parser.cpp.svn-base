/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 ******************************************************************************
 *
 * This file is derived from "ScummVM - Graphic Adventure Engine"
 * Original licence below:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <WARMUX_config.h>

#ifdef ENABLE_VKEYBD

#include <WARMUX_error.h>
#include "vkeybd/virtual-keyboard-parser.h"
#include "vkeybd/vkpolygon.h"
#include "include/constant.h"
#include "game/config.h"
#include "graphic/color.h"
#include "graphic/video.h"
#include "tool/resource_manager.h"

using namespace std;

namespace Common {

bool vparseIntegerKey(const char *key, int count, va_list args)
{
  char *parseEnd;
  int *num_ptr;

  while (count--) {
    while (isspace(static_cast<unsigned char> (*key)))
      key++;

    num_ptr = va_arg(args, int*);
    *num_ptr = strtol(key, &parseEnd, 10);

    key = parseEnd;

    while (isspace(static_cast<unsigned char> (*key)))
      key++;

    if (count && *key++ != ',')
      return false;
  }

  return (*key == 0);
}

bool parseIntegerKey(const string &key, int count, ...)
{
  bool result;
  va_list args;
  va_start(args, count);
  result = vparseIntegerKey(key.c_str(), count, args);
  va_end(args);
  return result;
}

/*std::list<std::string> stringTokenizer(const String str, const String sep) {
 std::list<std::string> tok;
 std::string strs = str;
 size_t pos = strs.find_first_of(sep);
 while (pos != std::string::npos) {
 String flag = strs.substr(0,pos);
 tok.insert(tok.end(), flag);
 strs.erase(0,pos+1);
 pos = strs.find_first_of(" ,");
 }
 return tok;
 }*/

VirtualKeyboardParser::VirtualKeyboardParser(VirtualKeyboard *kbd) :
  _keyboard(kbd)
{
}

VirtualKeyboardParser::~VirtualKeyboardParser()
{
}

void VirtualKeyboardParser::cleanup()
{
  _mode = 0;
  _kbdParsed = false;
  _initialModeName.clear();
  if (_parseMode == kParseFull) {
    // reset keyboard to remove existing config
    _keyboard->reset();
  }
}

bool VirtualKeyboardParser::parserCallback_keyboard(const xmlNode *node)
{
  if (_kbdParsed)
    Error("Only a single keyboard element is allowed");

  // if not full parse then we're done
  if (_parseMode == kParseCheckResolutions)
    return true;

  ReadStringAttr(node, "initial_mode", _initialModeName);
  string h;
  if (ReadStringAttr(node, "h_align", h)) {
    if (h == "left")
      _keyboard->_hAlignment = VirtualKeyboard::kAlignLeft;
    else if (h == "centre" || h == "center")
      _keyboard->_hAlignment = VirtualKeyboard::kAlignCenter;
    else if (h == "right")
      _keyboard->_hAlignment = VirtualKeyboard::kAlignRight;
  }
  string v;
  if (ReadStringAttr(node, "v_align", v)) {
    if (v == "top")
      _keyboard->_vAlignment = VirtualKeyboard::kAlignTop;
    else if (v == "middle" || v == "center")
      _keyboard->_vAlignment = VirtualKeyboard::kAlignMiddle;
    else if (v == "bottom")
      _keyboard->_vAlignment = VirtualKeyboard::kAlignBottom;
  }

  return true;
}

bool VirtualKeyboardParser::parserCallback_mode(const xmlNode *node)
{
  string name;
  ReadStringAttr(node, "name", name);

  if (_parseMode == kParseFull) {
    // if full parse then add new mode to keyboard
    if (_keyboard->_modes.count(name))
      Error("Mode '" + name + "' has already been defined");

    VirtualKeyboard::Mode mode;
    mode.name = name;
    _keyboard->_modes[name] = mode;
  }

  _mode = &(_keyboard->_modes[name]);
  if (name == _initialModeName)
    _keyboard->_initialMode = _mode;
  string resolutions;
  ReadStringAttr(node, "resolutions", resolutions);
  //StringTokenizer tok (resolutions, " ,");
  std::list<std::string> tok;
  size_t pos = resolutions.find_first_of(" ,");
  while (pos != std::string::npos) {
    string resolution = resolutions.substr(0, pos);
    tok.insert(tok.end(), resolution);
    resolutions.erase(0, pos + 1);
    pos = resolutions.find_first_of(" ,");
  }
  if (!resolutions.empty()) {
    string resolution = resolutions.substr(0, resolutions.size());
    tok.insert(tok.end(), resolution);
  }

  // select best resolution simply by minimising the difference between the
  // overlay size and the resolution dimensions.
  // TODO: improve this by giving preference to a resolution that is smaller
  // than the overlay res (so the keyboard can't be too big for the screen)
  std::list<std::string>::iterator it;
  int scrW = GetMainWindow().GetWidth();
  int scrH = GetMainWindow().GetHeight();
  int diff = INT_MAX;
  string newResolution;
  string res;
  for (it = tok.begin(); it != tok.end(); it++) {
    res = *it;
    int resW, resH;
    if (sscanf(res.c_str(), "%dx%d", &resW, &resH) != 2) {
      Error("Invalid resolution specification");
    } else {
      if (resW == scrW && resH == scrH) {
        newResolution = res;
        break;
      } else {
        int newDiff = abs(scrW - resW) + abs(scrH - resH);
        if (newDiff < diff) {
          diff = newDiff;
          newResolution = res;
        }
      }
    }
  }

  if (newResolution.empty())
    Error("No acceptable resolution was found\n");

  if (_parseMode == kParseCheckResolutions) {
    if (_mode->resolution == newResolution) {
      //node->ignore = true;
      return true;
    } else {
      // remove data relating to old resolution
      _mode->bitmapName.clear();
      //if (_mode->image) {
      //	_mode->image->Free();
      //	delete _mode->image;
      //	_mode->image = 0;
      //}
      _mode->imageMap.removeAllAreas();
      _mode->displayArea = Rectanglei();
    }
  }

  _mode->resolution = newResolution;
  printf("Virtual Keyboard resolution %s\n", newResolution.c_str());
  _layoutParsed = false;

  return true;
}

bool VirtualKeyboardParser::parserCallback_event(const xmlNode *node)
{
  // if just checking resolutions we're done
  if (_parseMode == kParseCheckResolutions)
    return true;
  string name;
  ReadStringAttr(node, "name", name);
  if (_mode->events.count(name))
    Error("Event '" + name + "' has already been defined");

  VirtualKeyboard::VKEvent *evt = new VirtualKeyboard::VKEvent();
  evt->name = name;
  string type;
  ReadStringAttr(node, "type", type);
  if (type == "key") {
    string code;
    string ascii;
    if (!ReadStringAttr(node, "code", code) | !ReadStringAttr(node, "ascii", ascii)) {
      delete evt;
      Error("Key event element must contain code and ascii attributes");
      return false;
    }
    evt->type = VirtualKeyboard::kVKEventKey;

    KeyState *ks = (KeyState*) malloc(sizeof(KeyState));
    ks->scancode = (KeyCode) atoi(code.c_str());
    ks->sym = (SDLKey) atoi(code.c_str());
    ks->unicode = atoi(ascii.c_str());
    ks->mod = (SDLMod) 0;
    string mods;
    if (ReadStringAttr(node, "modifiers", mods))
      ks->mod = (SDLMod) parseFlags(mods);
    evt->data = ks;

  } else if (type == "modifier") {
    string modifier;
    if (!ReadStringAttr(node, "modifiers", modifier)) {
      delete evt;
      Error("Key modifier element must contain modifier attributes");
    }

    evt->type = VirtualKeyboard::kVKEventModifier;
    int *flags = (int*) malloc(sizeof(int));
    if (!flags)
      Error("[VirtualKeyboardParser::parserCallback_event] Cannot allocate memory");

    *(flags) = parseFlags(modifier);
    evt->data = flags;

  } else if (type == "switch_mode") {
    string mode;
    if (!ReadStringAttr(node, "mode", mode)) {
      delete evt;
      Error("Switch mode event element must contain mode attribute");
    }

    evt->type = VirtualKeyboard::kVKEventSwitchMode;

    char *str = (char*) malloc(sizeof(char) * mode.size() + 1);
    if (!str)
      printf("[VirtualKeyboardParser::parserCallback_event] Cannot allocate memory");

    memcpy(str, mode.c_str(), sizeof(char) * mode.size());
    str[mode.size()] = 0;
    evt->data = str;
  } else if (type == "submit") {
    evt->type = VirtualKeyboard::kVKEventSubmit;
  } else if (type == "cancel") {
    evt->type = VirtualKeyboard::kVKEventCancel;
  } else if (type == "clear") {
    evt->type = VirtualKeyboard::kVKEventClear;
  } else if (type == "delete") {
    evt->type = VirtualKeyboard::kVKEventDelete;
  } else if (type == "move_left") {
    evt->type = VirtualKeyboard::kVKEventMoveLeft;
  } else if (type == "move_right") {
    evt->type = VirtualKeyboard::kVKEventMoveRight;
  } else {
    delete evt;
    Error("Event type '" + type + "' not known");
  }

  _mode->events[name] = evt;

  return true;
}

bool VirtualKeyboardParser::parserCallback_layout(const xmlNode *node)
{
  assert(!_mode->resolution.empty());
  string res;
  ReadStringAttr(node, "resolution", res);

  if (res != _mode->resolution) {
    //		node->ignore = true;
    return false;
  }

  ReadStringAttr(node, "bitmap", _mode->bitmapName);

  int r, g, b;
  string tcolor;
  if (ReadStringAttr(node, "transparent_color", tcolor)) {
    if (!parseIntegerKey(tcolor, 3, &r, &g, &b))
      Error("Could not parse color value");
  } else {
    // default to purple
    r = 255;
    g = 0;
    b = 255;
  }
  _mode->transparentColor = Color(r, g, b);// format.RGBToColor(r, g, b);

  _mode->image = ResourceManager::GetRef().LoadImage(Config::GetConstInstance()->GetDataDir()
      + "/vkeyb/" + "vkeybd_default" + "/" + _mode->bitmapName, false, false);
  _mode->image.SetColorKey(SDL_SRCCOLORKEY | SDL_RLEACCEL, _mode->image.MapColor(
      _mode->transparentColor));

  if (ReadStringAttr(node, "display_font_color", tcolor)) {
    if (!parseIntegerKey(tcolor, 3, &r, &g, &b))
      Error("Could not parse color value");
  } else {
    r = g = b = 0; // default to black
  }
  _mode->displayFontColor = Color(r, g, b);

  _layoutParsed = true;

  return true;
}

bool VirtualKeyboardParser::parserCallback_map(const xmlNode *node)
{
  return true;
}

bool VirtualKeyboardParser::parserCallback_area(const xmlNode *node)
{
  string shape;
  string target;
  string coords;

  ReadStringAttr(node, "shape", shape);
  ReadStringAttr(node, "target", target);
  ReadStringAttr(node, "coords", coords);

  if (target == "display_area") {
    if (shape != "rect")
      Error("display_area must be a rect area");
    _mode->displayArea = Rectanglei();
    return parseRect(_mode->displayArea, coords);
  } else if (shape == "rect") {
    Polygon *poly = _mode->imageMap.createArea(target);
    return parseRectAsPolygon(*poly, coords);
    //} else if (!shape.compare("poly")) {
    //	Polygon *poly = _mode->imageMap.createArea(target);
    //	return parsePolygon(*poly, coords);
  }
  Error("Area shape '" + shape + "' not known");
  return false;
}

int VirtualKeyboardParser::parseFlags(const string& flags)
{
  if (flags.empty())
    return 0;

  //	StringTokenizer tok(flags, ", ");
  std::string tflags = flags;
  std::list<std::string> tok;
  size_t pos = tflags.find_first_of(" ,");
  while (pos != std::string::npos) {
    string flag = tflags.substr(0, pos);
    tok.insert(tok.end(), flag);
    tflags.erase(0, pos + 1);
    pos = tflags.find_first_of(" ,");
  }
  if (!tflags.empty()) {
    string flag = tflags.substr(0, tflags.size());
    tok.insert(tok.end(), flag);
  }

  int val = KMOD_NONE;
  for (string fl = tok.front(); !tok.empty(); fl = tok.front()) {
    if (fl == "ctrl" || fl == "control")
      val |= KMOD_CTRL;
    else if (fl == "alt")
      val |= KMOD_ALT;
    else if (fl == "shift")
      val |= KMOD_SHIFT;
    tok.pop_front();
  }
  return val;
}

bool VirtualKeyboardParser::parseRect(Rectanglei &rect, const string& coords)
{
  int x1, y1, x2, y2;
  if (!parseIntegerKey(coords, 4, &x1, &y1, &x2, &y2))
    Error("Invalid coords for rect area");

  rect = Rectanglei(x1, y1, x2 - x1, y2 - y1);
  //	if (!rect.isValidRect())
  //		return parserError("Rect area is not a valid rectangle");
  return true;
}
/*
 bool VirtualKeyboardParser::parsePolygon(Polygon &poly, const String& coords) {
 StringTokenizer tok(coords, ", ");
 for (String st = tok.nextToken(); !st.empty(); st = tok.nextToken()) {
 int x, y;
 if (sscanf(st.c_str(), "%d", &x) != 1)
 return parserError("Invalid coords for polygon area");
 st = tok.nextToken();
 if (sscanf(st.c_str(), "%d", &y) != 1)
 return parserError("Invalid coords for polygon area");
 poly.addPoint(x, y);
 }
 if (poly.getPointCount() < 3)
 return parserError("Invalid coords for polygon area");

 return true;
 }
 */
bool VirtualKeyboardParser::parseRectAsPolygon(Polygon &poly, const string& coords)
{
  Rectanglei rect;
  if (!parseRect(rect, coords))
    return false;
  poly.addPoint(rect.GetLeft(), rect.GetTop());
  poly.addPoint(rect.GetRight(), rect.GetTop());
  poly.addPoint(rect.GetRight(), rect.GetBottom());
  poly.addPoint(rect.GetLeft(), rect.GetBottom());
  return true;
}

bool VirtualKeyboardParser::parse()
{
  cleanup();

  parserCallback_keyboard(GetMarker(GetRoot(), "keyboard"));

  xmlNodeArray mode_nodes = GetNamedChildren(GetMarker(GetRoot(), "keyboard"), "mode");
  xmlNodeArray::const_iterator mode_it = mode_nodes.begin();
  for (; mode_it != mode_nodes.end(); ++mode_it) {
    parserCallback_mode(*mode_it);
    xmlNodeArray layout_nodes = GetNamedChildren(*mode_it, "layout");
    xmlNodeArray::const_iterator layout_it = layout_nodes.begin();
    for (; layout_it != layout_nodes.end(); ++layout_it) {
      if (parserCallback_layout(*layout_it)) {
        xmlNodeArray nodes = GetNamedChildren(GetMarker(*layout_it, "map"), "area");
        xmlNodeArray::const_iterator it = nodes.begin();
        for (; it != nodes.end(); ++it) {
          parserCallback_area(*it);
        }
      }
    }

    xmlNodeArray event_nodes = GetNamedChildren(*mode_it, "event");
    xmlNodeArray::const_iterator event_it = event_nodes.begin();
    for (; event_it != event_nodes.end(); ++event_it) {
      parserCallback_event(*event_it);
    }
  }
  _kbdParsed = true;
  return true;
}

} // End of namespace GUI

#endif // #ifdef ENABLE_VKEYBD
