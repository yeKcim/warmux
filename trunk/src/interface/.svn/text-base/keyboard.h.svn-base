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
 * Keyboard management
 *****************************************************************************/

#ifndef KEYBOARD_H
#define KEYBOARD_H
//-----------------------------------------------------------------------------
#include <set>
#include "SDL_keysym.h"

#include <WARMUX_singleton.h>

#include "interface/man_machine_interface.h"
//-----------------------------------------------------------------------------

typedef struct _xmlNode xmlNode;

#define SHIFT_BIT   0x1
#define ALT_BIT     0x2
#define CONTROL_BIT 0x4

class Keyboard : public Singleton<Keyboard>, public ManMachineInterface
{
private:
  int modifier_bits;
  int modifier_only_bits;
  std::set<SDLKey> pressed_keys;
  void HandleKeyComboEvent(int key_code, Key_Event_t event_type);
protected:
  friend class Singleton<Keyboard>;
  Keyboard();
  void SetDefaultConfig();

public:
  bool HandleKeyEvent(const SDL_Event& evnt);
  void SetConfig(const xmlNode *node);
  void SaveConfig(xmlNode *node) const;

  int  GetRawKeyCode(int keycode) const;
  bool HasControlModifier(int key_code) const;
  bool HasAltModifier(int key_code) const;
  bool HasShiftModifier(int key_code) const;
  ManMachineInterface::Key_t
    GetRegisteredAction(int raw_key_code, bool control, bool alt, bool shift) const;
  bool SaveKeyEvent(Key_t at, int raw_key_code,
                    bool control, bool alt, bool shift);
  static bool IsModifier(int raw_key_code);
  static int  GetModifierBits();
};

//-----------------------------------------------------------------------------
#endif /* KEYBOARD_H */
