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
 * Liste de choix.
 *****************************************************************************/

#ifndef LIST_BOX_H
#define LIST_BOX_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../include/base.h"
#include "widget.h"
#include "button.h"
#include <string>
#include <vector>
//-----------------------------------------------------------------------------

typedef struct s_list_box_item_t{
    std::string label;
    std::string value;
  } list_box_item_t;


class ListBox : public Widget
{ 
 public:
  bool always_one_selected;
private:
  // for the placement
  uint nb_visible_items, nb_visible_items_max;
  uint height_item;
  
  // what are the items ?
  uint first_visible_item;
  int selected_item; 
  std::vector<list_box_item_t> m_items;

  // Buttons
  Button *m_up, *m_down;

public:
  ListBox (uint _x, uint _y, uint _w, uint _h);
  ~ListBox();

  void Draw (uint mouse_x, uint mouse_y);
  bool Clic (uint mouse_x, uint mouse_y, uint button);  
  void SetSizePosition(uint _x, uint _y, uint _w, uint _h);

  void AddItem (bool selected, 
		const std::string &label,
		const std::string &value);
  void Sort();

  int MouseIsOnWhichItem (uint mouse_x, uint mouse_y);

  void Select (uint index);
  int GetSelectedItem ();
  void Deselect ();
  void RemoveSelected();
  const std::string& ReadLabel () const;
  const std::string& ReadValue () const;
  const std::string& ReadValue (int index) const;

  std::vector<list_box_item_t> * GetItemsList();
};

//-----------------------------------------------------------------------------
#endif
