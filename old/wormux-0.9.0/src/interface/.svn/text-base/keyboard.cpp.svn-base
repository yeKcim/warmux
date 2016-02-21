/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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

#include "interface/keyboard.h"
#include "game/game.h"
#include "network/chat.h"
#include <SDL_events.h>
#include <libxml/tree.h>

#define MODIFIER_OFFSET (SDLK_LAST + 1)
#define SHIFT_BIT 0x1
#define ALT_BIT 0x2
#define CONTROL_BIT 0x4
#define SHIFT_OFFSET (MODIFIER_OFFSET * SHIFT_BIT)
#define CONTROL_OFFSET (MODIFIER_OFFSET * CONTROL_BIT)
#define ALT_OFFSET (MODIFIER_OFFSET * ALT_BIT)


Keyboard::Keyboard() :
  ManMachineInterface(),
  modifier_bits(0)
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
  SetKeyAction(SDLK_ESCAPE,                  ManMachineInterface::KEY_QUIT);
}

void Keyboard::SetConfig(const xmlNode *node)
{
  ASSERT(node != NULL);

  //Remove old key configuration
  ClearKeyAction();

  xmlNodeArray list = XmlReader::GetNamedChildren(node, "bind");
  for (xmlNodeArray::iterator it = list.begin(); it != list.end(); ++it)
  {
    std::string key_name, action_name;
    bool shift, control, alt;
    shift = false;
    control = false;
    alt = false;

    //Extract XML config
    XmlReader::ReadStringAttr(*it, "key", key_name);
    XmlReader::ReadStringAttr(*it, "action", action_name);
    XmlReader::ReadBoolAttr(*it, "shift", shift);
    XmlReader::ReadBoolAttr(*it, "control", control);
    XmlReader::ReadBoolAttr(*it, "alt", alt);

    //Generate key and action value
    int key;
    Key_t action;

    key = GetKeyFromKeyName(key_name);
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

void Keyboard::SaveConfig( xmlNode *node) const
{
  xmlNode *keyboard_node = xmlAddChild(node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"keyboard"));
  std::map<int, std::vector<Key_t> >::const_iterator it;

  for (it = layout.begin(); it != layout.end(); it++)
  {
    const std::vector<Key_t> actions = it->second;
    for (uint i = 0; i < actions.size(); i++)
    {
      bool shift, control, alt;
      shift = false;
      control = false;
      alt = false;
      int key = it->first;

      if (key > CONTROL_OFFSET)
      {
        key -= CONTROL_OFFSET;
        control = true;
      }

      if (key > ALT_OFFSET)
      {
        key -= ALT_OFFSET;
        alt = true;
      }

      if (key > SHIFT_OFFSET)
      {
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

  if(it == layout.end())
    return;


  std::vector<Key_t> keys = it->second;
  std::vector<Key_t>::const_iterator itv;

  for(itv = keys.begin(); itv != keys.end() ; itv++)
  {
    //While player writes, it cannot control the game but PAUSE.
    if (Game::GetInstance()->chatsession.CheckInput()) {
      switch (*itv) {
      case KEY_PAUSE:
        break;
      default:
        return;
      }
    }

    if(event_type == KEY_PRESSED) {
      HandleKeyPressed(*itv);
      return;
    }

    if(event_type == KEY_RELEASED) {
      HandleKeyReleased(*itv);
      return;
    }
  }
}

static int GetModifierBitsFromSDL() {
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

void Keyboard::HandleKeyEvent(const SDL_Event& event)
{
  // Not a registred event
  if(!IsRegistredEvent(event.type))
    return;

  Key_Event_t event_type;
  switch(event.type)
    {
    case SDL_KEYDOWN:
      event_type = KEY_PRESSED;
      break;
    case SDL_KEYUP:
      event_type = KEY_RELEASED;
      break;
    default:
      return;
    }

  //Handle input text for Chat session in Network game
  if (Game::GetInstance()->chatsession.CheckInput()) {
    if (event_type == KEY_PRESSED)
      Game::GetInstance()->chatsession.HandleKeyPressed(event);
    else if (event_type == KEY_RELEASED)
      Game::GetInstance()->chatsession.HandleKeyReleased(event);
    return;
  }

  int previous_modifier_bits = modifier_bits;
  modifier_bits = GetModifierBitsFromSDL();
  SDLKey basic_key_code = event.key.keysym.sym;
  if (basic_key_code >= MODIFIER_OFFSET)
    return;
  int key_code;
  if (modifier_bits != previous_modifier_bits) {
    std::set<SDLKey>::iterator it;
    for (it = pressed_keys.begin();  it !=  pressed_keys.end(); it++) {
      int basic_key_code_it = *it;
      if (basic_key_code != basic_key_code_it) {
        key_code = basic_key_code_it + MODIFIER_OFFSET * previous_modifier_bits;
        HandleKeyComboEvent(key_code, KEY_RELEASED);
        key_code = basic_key_code_it + MODIFIER_OFFSET * modifier_bits;
        HandleKeyComboEvent(key_code, KEY_PRESSED);
      }
    }
  }
  if (event_type == KEY_PRESSED) {
    key_code = basic_key_code + (MODIFIER_OFFSET * modifier_bits);
    HandleKeyComboEvent(key_code, KEY_PRESSED);
    pressed_keys.insert(basic_key_code);
  } else {
    ASSERT(event_type == KEY_RELEASED);
    key_code = basic_key_code + (MODIFIER_OFFSET * previous_modifier_bits);
    HandleKeyComboEvent(key_code, KEY_RELEASED);
    pressed_keys.erase(basic_key_code);
  }
}
