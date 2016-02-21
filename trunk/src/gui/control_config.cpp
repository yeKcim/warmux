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
 * Widget to configure controls
 *****************************************************************************/

#include "game/config.h"
#include "gui/check_box.h"
#include "gui/horizontal_box.h"
#include "gui/label.h"
#include "gui/null_widget.h"
#ifndef ANDROID
# include "gui/picture_widget.h"
#endif
#include "gui/question.h"
#include "gui/select_box.h"
#include "interface/keyboard.h"

#include "control_config.h"

#define MIN_ACTION_WIDTH  310
#define MIN_KEY_WIDTH      50
#ifdef ANDROID
# define CHECKBOX_WIDTH     18
#else
# define KEY_CTRL_WIDTH     49
# define KEY_ALT_WIDTH      42
# define KEY_SHIFT_WIDTH    82
# define KEY_HEIGHT         37
#endif
#define SPACING_WIDTH       8
#define MODIFIERS_WIDTH    64

class ControlItem : public HBox
{
  ManMachineInterface::Key_t key_action;
  int   key_value;
  bool  read_only;

  Label *label_action, *label_key;
#ifdef ANDROID
  CheckBox *shift_box, *alt_box, *ctrl_box;
#else
  PictureWidget *shift_box, *alt_box, *ctrl_box;
#endif

public:
  // Ease parsing all other keys
  static std::vector<ControlItem*> *selves;

  ControlItem(int action, bool ro, uint height)
    : HBox(height, false, false, true /* use full height */)
    , key_action((ManMachineInterface::Key_t)action)
    , read_only(ro)
  {
    SetMargin(0);
    SetNoBorder();

    // Action name
    const Keyboard *kbd = Keyboard::GetConstInstance();
    label_action = new Label(kbd->GetHumanReadableActionName(key_action),
                             MIN_ACTION_WIDTH, (Font::font_size_t)14, Font::FONT_BOLD,
                             dark_gray_color, Text::ALIGN_RIGHT_CENTER);
    AddWidget(label_action);

    // First spacing
    AddWidget(new NullWidget(Point2i(SPACING_WIDTH, height)));

    // Modifiers
    int key_code = kbd->GetKeyAssociatedToAction(key_action);

#ifdef ANDROID
    ctrl_box  = new CheckBox("", CHECKBOX_WIDTH,
                             kbd->HasControlModifier(key_code));
    AddWidget(ctrl_box);

    alt_box   = new CheckBox("", CHECKBOX_WIDTH,
                             kbd->HasAltModifier(key_code));
    AddWidget(alt_box);

    shift_box = new CheckBox("", CHECKBOX_WIDTH,
                             kbd->HasShiftModifier(key_code));
    AddWidget(shift_box);
#else
    ctrl_box = new PictureWidget(Point2i(KEY_CTRL_WIDTH, KEY_HEIGHT),
                                 "menu/key_ctrl");
    if (!kbd->HasControlModifier(key_code)) {
      ctrl_box->Disable();
    }
    AddWidget(ctrl_box);

    alt_box = new PictureWidget(Point2i(KEY_ALT_WIDTH, KEY_HEIGHT),
                                "menu/key_alt");
    if (!kbd->HasAltModifier(key_code)) {
      alt_box->Disable();
    }
    AddWidget(alt_box);

    shift_box = new PictureWidget(Point2i(KEY_SHIFT_WIDTH, KEY_HEIGHT),
                                  "menu/key_shift");
    if (!kbd->HasShiftModifier(key_code)) {
      shift_box->Disable();
    }
    AddWidget(shift_box);
#endif

    // Second spacing
    AddWidget(new NullWidget(Point2i(SPACING_WIDTH, height)));

    // Actual key
    key_value = kbd->GetRawKeyCode(key_code);
    label_key = new Label((key_value) ? kbd->GetKeyNameFromKey(key_value) : _("None"),
                          MIN_KEY_WIDTH, Font::FONT_SMALL, Font::FONT_NORMAL,
                          c_black, Text::ALIGN_LEFT_CENTER);
    AddWidget(label_key);
  }

  virtual Widget* ClickUp(const Point2i &mousePosition, uint button)
  {
    WidgetList::ClickUp(mousePosition, button);
    return NULL;
  }

  virtual bool SendKey(const SDL_keysym & key)
  {
    SDLKey key_code = key.sym;

    if (read_only || key_code == SDLK_UNKNOWN)
      return false;

    // Ignore modifiers-only key presses
    if (Keyboard::IsModifier(key_code))
      return true;

    Keyboard *kbd = Keyboard::GetInstance();

    SDLMod mod_bits = SDL_GetModState();
    bool has_shift = mod_bits & KMOD_SHIFT;
    bool has_alt = mod_bits & KMOD_ALT;
    bool has_ctrl = mod_bits & KMOD_CTRL;
#ifdef MAEMO
    bool has_mode = mod_bits & KMOD_MODE;

    if (has_mode) {
      if (key_code == SDLK_LEFT) key_code = SDLK_UP;
      if (key_code == SDLK_RIGHT) key_code = SDLK_DOWN;
    }
#endif

#ifndef ANDROID
    ctrl_box->Disable();
    alt_box->Disable();
    shift_box->Disable();
#endif

    // Reset some configs if pure backspace is pressed
    if ((SDLK_BACKSPACE == key_code || SDLK_DELETE == key_code) &&
        !has_ctrl && !has_alt && !has_shift) {
      key_value = SDLK_UNKNOWN;
      label_key->SetText(_("None"));
#ifdef ANDROID
      ctrl_box->SetValue(false);
      alt_box->SetValue(false);
      shift_box->SetValue(false);
#endif

      // A simple NeedRedraw would reset the packing
      Pack();
      NeedRedrawing();
      return true;
    }

    // Check the current state of bindings, not the one in use
    for (std::vector<ControlItem*>::const_iterator it = selves->begin();
         it != selves->end();
         ++it) {
      const ControlItem *c = (*it);

      if (c!=this && c->key_value==key_code
#ifdef ANDROID
          && has_ctrl  == c->ctrl_box->GetValue()
          && has_alt   == c->alt_box->GetValue()
          && has_shift == c->shift_box->GetValue()) {
#else
          && has_ctrl  == c->ctrl_box->IsEnabled()
          && has_alt   == c->alt_box->IsEnabled()
          && has_shift == c->shift_box->IsEnabled()) {
#endif
        // A box different from this already has this setting
        Question question(Question::WARNING);
        question.Set(Format(_("This key has already been attributed to '%s'"),
                            kbd->GetHumanReadableActionName(c->key_action).c_str()),
                     true, 0);

        // React only to key press, not releases, as one key is being pressed now
        question.Ask(false);
        return true;
      }
    }

    key_value = key_code;
    label_key->SetText(kbd->GetKeyNameFromKey(key_value));

#ifdef ANDROID
    ctrl_box->SetValue(has_ctrl);
    alt_box->SetValue(has_alt);
    shift_box->SetValue(has_shift);
#else
    if (has_ctrl)
      ctrl_box->Enable();
    if (has_alt)
      alt_box->Enable();
    if (has_shift)
      shift_box->Enable();
#endif

    // A simple NeedRedraw would reset the packing
    Pack();
    NeedRedrawing();
    return true;
  }

  virtual void Pack()
  {
    // First this so that HBox::Pack does not reset label_key width back
    // to its minimal value
    label_key->SetSizeX(size.x - MIN_ACTION_WIDTH -
#ifdef ANDROID
                        2*SPACING_WIDTH - 3*CHECKBOX_WIDTH);
#else
                        2*SPACING_WIDTH - (KEY_CTRL_WIDTH + KEY_ALT_WIDTH + KEY_SHIFT_WIDTH));
#endif

    // Call first HBox::Pack to set positions
    HBox::Pack();

    // Set proper height now
    int height = size.y-2*border_size;
    for (wit it = widget_list.begin(); it != widget_list.end(); ++it)
      (*it)->SetSizeY(height);
  }

  void SaveAction(Keyboard *kbd)
  {
    if (!read_only) {
      kbd->ClearKeyAction(key_action);
      if (key_value != SDLK_UNKNOWN) {
        kbd->SaveKeyEvent(key_action, key_value,
#ifdef ANDROID
                          ctrl_box->GetValue(),
                          alt_box->GetValue(),
                          shift_box->GetValue());
#else
                          ctrl_box->IsEnabled(),
                          alt_box->IsEnabled(),
                          shift_box->IsEnabled());
#endif
      }
    }
  }
};

std::vector<ControlItem*>* ControlItem::selves = NULL;

class HeaderItem : public HBox
{
  Label *label_action, *label_key, *label_mods;

public:
  HeaderItem(uint height)
    : HBox(height, false, false, true)
  {
    SetMargin(0);
    SetNoBorder();

    // Action name
    label_action = new Label(_("Action"), MIN_ACTION_WIDTH,
                             Font::FONT_MEDIUM, Font::FONT_NORMAL,
                             c_black, Text::ALIGN_RIGHT_CENTER);
    AddWidget(label_action);

    // Modifiers
    label_mods = new Label(_("Modifiers"), MODIFIERS_WIDTH,
                           Font::FONT_MEDIUM, Font::FONT_NORMAL,
                           c_black, Text::ALIGN_CENTER);
    AddWidget(label_mods);

    // Actual key
    label_key = new Label(_("Key"), MIN_KEY_WIDTH,
                          Font::FONT_MEDIUM, Font::FONT_NORMAL,
                          c_black, Text::ALIGN_LEFT_CENTER);
    AddWidget(label_key);

    SetBackgroundColor(light_gray_color);
  }

  virtual void Pack()
  {
    // First this so that HBox::Pack does not reset label_key width back
    // to its minimal value
    label_key->SetSizeX(size.x - MIN_ACTION_WIDTH - MODIFIERS_WIDTH);

    // Call first HBox::Pack to set positions
    HBox::Pack();

    // Set proper height now
    int height = size.y-2*border_size;
    for (wit it = widget_list.begin(); it != widget_list.end(); ++it)
      (*it)->SetSizeY(height);
  }
};

ControlConfig::ControlConfig(const Point2i& size, bool readonly)
  : WidgetList(size)
  , read_only(readonly)
{
  header = new HeaderItem(32);
  AddWidget(header);

  box = new SelectBox(size, false, true /* full box width used*/);
  box->SetBorder(c_black, 0);
  for (int i=0; i<ManMachineInterface::KEY_NONE; i++) {
    ControlItem *item = new ControlItem(i, readonly, 32);
    items.push_back(item);
    box->AddWidget(item);
  }
  ControlItem::selves = &items;
  AddWidget(box);
}

void ControlConfig::SaveControlConfig() const
{
  if (read_only)
    return;

  Keyboard *kbd = Keyboard::GetInstance();
  for (uint i=0; i<items.size(); i++) {
    items[i]->SaveAction(kbd);
  }
}

void ControlConfig::Pack()
{
  //printf("Set widget at (%i,%i) as %ix%i\n", position.x, position.y, size.x, size.y);
  header->SetPosition(position);
  header->SetSizeX(size.x - 2*border_size);
  header->Pack();
  box->SetPosition(position.x + border_size,
                   position.y + header->GetSizeY() + border_size);
  box->SetSize(size.x - 2*border_size,
               size.y - header->GetSizeY() - 2*border_size);
  box->Pack();
}
