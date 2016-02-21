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
 * Keyboard management.
 *****************************************************************************/

#include "game/game.h"
#include "interface/keyboard.h"
#include "network/chat.h"
#ifdef DEBUG
#include "network/randomsync.h"
#endif
#include "tool/xml_document.h"

#include <SDL_events.h>
#include <libxml/tree.h>

#define MODIFIER_OFFSET (SDLK_LAST + 1)
#define SHIFT_OFFSET (MODIFIER_OFFSET * SHIFT_BIT)
#define CONTROL_OFFSET (MODIFIER_OFFSET * CONTROL_BIT)
#define ALT_OFFSET (MODIFIER_OFFSET * ALT_BIT)

#if SDL_MINOR_VERSION != 2
#  define SDLK_LAST  SDL_NUM_SCANCODES
#endif


int  Keyboard::GetRawKeyCode(int key_code) const
{
  return key_code % MODIFIER_OFFSET;
}

bool Keyboard::HasShiftModifier(int key_code) const
{
  return (key_code/SHIFT_OFFSET)&1;
}

bool Keyboard::HasAltModifier(int key_code) const
{
  return (key_code/ALT_OFFSET)&1;
}

bool Keyboard::HasControlModifier(int key_code) const
{
  return (key_code/CONTROL_OFFSET)&1;
}

ManMachineInterface::Key_t
Keyboard::GetRegisteredAction(int raw_key_code, bool ctrl, bool alt, bool shift) const
{
  int key_code = raw_key_code + shift*SHIFT_OFFSET +
                 alt*ALT_OFFSET + ctrl*CONTROL_OFFSET;

  // If the map doesn't have such key, exit immediately
  std::map<int, std::vector<Key_t> >::const_iterator it = layout.find(key_code);
  if (it == layout.end())
    return KEY_NONE;

  // Must use at because [] has side effect of inserting a new key,
  // despite our previous verification
  return ((*it).second.size()) ?(*it).second[0] : KEY_NONE;
}

bool Keyboard::SaveKeyEvent(Key_t at, int raw_key_code,
                            bool ctrl, bool alt, bool shift)
{
  int key_code = raw_key_code + shift*SHIFT_OFFSET
               + alt*ALT_OFFSET + ctrl*CONTROL_OFFSET;
  SetKeyAction(key_code, at);
  return true;
}


Keyboard::Keyboard()
  : ManMachineInterface()
  , modifier_bits(0)
  , modifier_only_bits(0)
{
  //Disable repeated events when a key is kept down
  SDL_EnableKeyRepeat(0,0);
  SetDefaultConfig();

  // Registring SDL event
  RegisterEvent(SDL_KEYDOWN);
  RegisterEvent(SDL_KEYUP);
}

void Keyboard::SetDefaultConfig()
{
  SetKeyAction(SDLK_ESCAPE, ManMachineInterface::KEY_QUIT);
}

void Keyboard::SetConfig(const xmlNode *node)
{
  ASSERT(node);

  //Remove old key configuration
  ClearKeyBindings();

  xmlNodeArray list = XmlReader::GetNamedChildren(node, "bind");
  for (xmlNodeArray::iterator it = list.begin(); it != list.end(); ++it) {
    std::string key_name, action_name;
    bool shift = false, control = false, alt = false;

    //Extract XML config
    XmlReader::ReadStringAttr(*it, "key", key_name);
    XmlReader::ReadStringAttr(*it, "action", action_name);
    XmlReader::ReadBoolAttr(*it, "shift", shift);
    XmlReader::ReadBoolAttr(*it, "control", control);
    XmlReader::ReadBoolAttr(*it, "alt", alt);

    //Generate key and action value
    int key;
    Key_t action;

    // Set internal key
    key = GetKeyFromKeyName(key_name);
    // Reject config set for other values
    if (SDLK_BACKSPACE == key || SDLK_DELETE == key)
      continue;

    if (shift) {
      key += SHIFT_OFFSET;
    }
    if (control) {
      key += CONTROL_OFFSET;
    }
    if (alt) {
      key += ALT_OFFSET;
    }

    action = GetActionFromActionName(action_name);

    //Set association
    SetKeyAction(key, action);
  }
}

void Keyboard::SaveConfig(xmlNode *node) const
{
  xmlNode *keyboard_node = xmlAddChild(node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"keyboard"));
  std::map<int, std::vector<Key_t> >::const_iterator it;

  for (it = layout.begin(); it != layout.end(); it++) {
    const std::vector<Key_t>& actions = it->second;
    for (uint i = 0; i < actions.size(); i++) {
      int key = it->first;

      if (key == SDLK_UNKNOWN)
        continue;

      bool shift = false, control = false, alt = false;

      if (key > CONTROL_OFFSET) {
        key -= CONTROL_OFFSET;
        control = true;
      }

      if (key > ALT_OFFSET) {
        key -= ALT_OFFSET;
        alt = true;
      }

      if (key > SHIFT_OFFSET) {
        key -= SHIFT_OFFSET;
        shift = true;
      }

      //Generate node
      xmlNode *bind = xmlAddChild(keyboard_node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"bind"));
      xmlSetProp(bind, (const xmlChar*)"key", (const xmlChar*)GetKeyNameFromKey(key).c_str());
      xmlSetProp(bind, (const xmlChar*)"action", (const xmlChar*)GetActionNameFromAction(actions.at(i)).c_str());
      if (shift) xmlSetProp(bind, (const xmlChar*)"shift", (const xmlChar*)"true");
      if (control) xmlSetProp(bind, (const xmlChar*)"control", (const xmlChar*)"true");
      if (alt) xmlSetProp(bind, (const xmlChar*)"alt", (const xmlChar*)"true");
    }
  }
}

void Keyboard::HandleKeyComboEvent(int key_code, Key_Event_t event_type)
{
  MSG_DEBUG("keyboard", "%s %s%s%s%d",
            event_type == KEY_PRESSED ?"pressed":"released",
            ((key_code / MODIFIER_OFFSET) & CONTROL_BIT)?"[control] + ": "",
            ((key_code / MODIFIER_OFFSET) & ALT_BIT)?"[alt] + ": "",
            ((key_code / MODIFIER_OFFSET) & SHIFT_BIT)?"[shift] + ": "",
            key_code % MODIFIER_OFFSET);
  std::map<int, std::vector<Key_t> >::iterator it = layout.find(key_code);

  if (it == layout.end())
    return;


  const std::vector<Key_t>& keys = it->second;
  std::vector<Key_t>::const_iterator itv;

  for (itv = keys.begin(); itv != keys.end() ; itv++) {
    //While player writes, it cannot control the game but PAUSE.
    if (Game::GetInstance()->chatsession.CheckInput()) {
      switch (*itv) {
      case KEY_PAUSE:
        break;
      default:
        return;
      }
    }

    if (event_type == KEY_PRESSED) {
      HandleKeyPressed(*itv);
      return;
    }

    if (event_type == KEY_RELEASED) {
      HandleKeyReleased(*itv);
      return;
    }
  }
}

int Keyboard::GetModifierBits()
{
  SDLMod sdl_modifier_bits = SDL_GetModState();
  int result = 0;
  if (sdl_modifier_bits & KMOD_SHIFT)
    result |= SHIFT_BIT;

  if (sdl_modifier_bits & KMOD_ALT)
    result |= ALT_BIT;

  if (sdl_modifier_bits & KMOD_CTRL)
    result |= CONTROL_BIT;

  return result;
}

bool Keyboard::IsModifier(int raw_key_code)
{
  return raw_key_code>=SDLK_NUMLOCK && raw_key_code<=SDLK_COMPOSE;
}

bool Keyboard::HandleKeyEvent(const SDL_Event& evnt)
{
  // Not a registred event
  if (!IsRegistredEvent(evnt.type))
    return false;

  Key_Event_t event_type;
  switch(evnt.type) {
  case SDL_KEYDOWN:
    event_type = KEY_PRESSED;
    break;
  case SDL_KEYUP:
    event_type = KEY_RELEASED;
    break;
  default:
    return false;
  }

  //Handle input text for Chat session in Network game
  if (Game::GetInstance()->chatsession.CheckInput()) {
    if (event_type == KEY_PRESSED)
      Game::GetInstance()->chatsession.HandleKeyPressed(evnt);
    else if (event_type == KEY_RELEASED)
      Game::GetInstance()->chatsession.HandleKeyReleased(evnt);
    return false;
  }

  SDLKey basic_key_code = evnt.key.keysym.sym;

#ifdef DEBUG
  if (IsLOGGING("killsynchro")
      && basic_key_code == SDLK_k
      && event_type == KEY_RELEASED) {
    fprintf(stderr, "\n\nKILLING NETWORK SYNCHRONISATION!\n\n");
    RandomSync().SetSeed(0);
  }
#endif

  // Also ignore real key code of a modifier, fix bug #15238
  if (IsModifier(basic_key_code)) {
    int modifier_changed = modifier_only_bits ^ GetModifierBits();
    if (event_type == KEY_RELEASED) {
      for (std::set<SDLKey>::const_iterator it = pressed_keys.begin(); it != pressed_keys.end(); it++ ) {
        int key_code = *it + MODIFIER_OFFSET * modifier_changed;
        HandleKeyComboEvent(key_code, KEY_RELEASED);
      }
    } else if (modifier_only_bits && event_type == KEY_PRESSED) {
      for (std::set<SDLKey>::const_iterator it = pressed_keys.begin(); it != pressed_keys.end(); it++ ) {
        int key_code = *it + MODIFIER_OFFSET * modifier_only_bits;
        HandleKeyComboEvent(key_code, KEY_RELEASED);
      }
    }
    modifier_only_bits = GetModifierBits();
    return false;
  }
#ifdef MAEMO
  if (SDL_GetModState() & KMOD_MODE) {
    if (basic_key_code == SDLK_LEFT) basic_key_code = SDLK_UP;
    if (basic_key_code == SDLK_RIGHT) basic_key_code = SDLK_DOWN;
  }
#endif

  int key_code;
  int previous_modifier_bits = modifier_bits;
  modifier_bits = GetModifierBits();

  if (modifier_bits != previous_modifier_bits) {
    std::set<SDLKey>::const_iterator it = pressed_keys.find(basic_key_code);
    if (it !=  pressed_keys.end()) {
      key_code = basic_key_code + MODIFIER_OFFSET * previous_modifier_bits;
      HandleKeyComboEvent(key_code, KEY_RELEASED);
      if (key_code != basic_key_code)
        HandleKeyComboEvent(basic_key_code, KEY_RELEASED);
      pressed_keys.erase(basic_key_code);
      if (event_type == KEY_PRESSED) {
        key_code = basic_key_code + MODIFIER_OFFSET * modifier_bits;
        HandleKeyComboEvent(key_code, KEY_PRESSED);
        pressed_keys.insert(basic_key_code);
      }
      return true;
    }
  }

  if (event_type == KEY_PRESSED) {
    key_code = basic_key_code + (MODIFIER_OFFSET * modifier_bits);
    HandleKeyComboEvent(key_code, KEY_PRESSED);
    if (previous_modifier_bits ^ modifier_bits) {
      key_code = basic_key_code + (MODIFIER_OFFSET * previous_modifier_bits);
      HandleKeyComboEvent(key_code, KEY_RELEASED);
    }
    pressed_keys.insert(basic_key_code);
  } else {
    ASSERT(event_type == KEY_RELEASED);
    key_code = basic_key_code + (MODIFIER_OFFSET * previous_modifier_bits);
    HandleKeyComboEvent(key_code, KEY_RELEASED);
    if (key_code != basic_key_code)
      HandleKeyComboEvent(basic_key_code, KEY_RELEASED);
    pressed_keys.erase(basic_key_code);
  }
  return true;
}
