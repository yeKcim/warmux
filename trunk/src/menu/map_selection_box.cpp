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
 * Map selection box
 *****************************************************************************/

#include <algorithm>

#include "menu/map_selection_box.h"
#include "game/config.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/question.h"
#include "gui/horizontal_box.h"
#include "gui/select_box.h"
#include "include/action_handler.h"
#include "map/maps_list.h"
#include "network/network.h"
#include "tool/resource_manager.h"

MapSelectionBox::MapSelectionBox(const Point2i &_size, bool show_border, bool _display_only)
  : VBox(_size.x, show_border, false)
  , selected_map_index(0)
  , display_only(_display_only)
  , selectable(true)
  , common(MapsList::GetInstance()->lst) // Created with an already initialized list
{
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml",false);

  // random map
  random_map_preview = LOAD_RES_IMAGE("menu/random_map");

  // compute margin width between previews
  Point2i preview_size(_size.x - 2*5, _size.y -2*10 -40);

  // Previews
  box = new ItemBox(preview_size, true, true, false);
  box->SetNoBorder();
  box->SetMargin(5);
  box->SetExtraWidthMode();
  AddWidget(box);
  RefreshBox();

  // Map information
  map_name_label = new Label("Map", W_UNDEF, Font::FONT_SMALL,
                             Font::FONT_BOLD, dark_gray_color,
                             Text::ALIGN_CENTER_TOP, false);
  AddWidget(map_name_label);

  map_author_label = new Label("Author", W_UNDEF, Font::FONT_SMALL,
                               Font::FONT_BOLD, dark_gray_color,
                               Text::ALIGN_CENTER_TOP, false);
  AddWidget(map_author_label);
}

void MapSelectionBox::ChangeMap(uint index)
{
  if (index > common.size() || selected_map_index == index)
    return;

  selected_map_index = index;
  box->Select(index);
  if (selected_map_index == common.size()) { // random map
    MapsList::GetInstance()->SelectMapByName("random");
  } else {
    MapsList::GetInstance()->SelectMapByName(common[selected_map_index]->GetRawName());
  }

  // Callback other network players
  if (Network::GetInstance()->IsGameMaster()) {
    Action a(Action::ACTION_GAME_SET_MAP);
    MapsList::GetInstance()->FillActionMenuSetMap(a);
    Network::GetInstance()->SendActionToAll(a);
  }

  // Set Map information
  UpdateMapInfo();
}

void MapSelectionBox::UpdateMapInfo()
{
  InfoMap* info = (InfoMap*)box->GetSelectedValue();

  if (!info) {
    // Random map selected
    map_name_label->SetText(_("Random map"));
    map_author_label->SetText(_("Choose randomly between the different maps"));
    return;
  }

  InfoMapBasicAccessor* basic = info->LoadBasicInfo();
  //PictureWidget *pw = (PictureWidget *)box->GetSelectedWidget();
  //pw->SetSurface(basic->ReadPreview(), PictureWidget::NO_SCALING);

  map_name_label->SetText(basic->ReadFullMapName());
  map_author_label->SetText(basic->ReadAuthorInfo());
}

Widget* MapSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (display_only)
    return NULL;

  if (button == SDL_BUTTON_WHEELDOWN)
    ChangeMap(selected_map_index -1);
  if (button == SDL_BUTTON_WHEELUP)
    ChangeMap(selected_map_index +1);

  Widget *w = WidgetList::ClickUp(mousePosition, button);
  if (w == box) {
    uint i = box->GetSelectedItem();
    if (i != selected_map_index) {
      ChangeMap(i);
      NeedRedrawing();
    }
    selected_map_index = i;
    UpdateMapInfo();
  }
  return w;
}

void MapSelectionBox::ValidMapSelection()
{
  std::string map_name;

  if (selected_map_index == common.size()) {
    // Choose one and select it!
    map_name = "random";

    if (Network::GetInstance()->IsLocal()) {
      MapsList::GetInstance()->SelectMapByName(map_name);
    }
  } else {
    map_name = common[selected_map_index]->GetRawName();
    MapsList::GetInstance()->SelectMapByName(map_name);
  }

  /* The player chose a map, save it in the main config so that this will be
   * the defaut map at next load of the game */
  Config::GetInstance()->SetMapName(map_name);
}

void MapSelectionBox::ChangeMapCallback()
{
  const InfoMap* current = MapsList::GetInstance()->ActiveMap();
  for (uint i=0; i<common.size(); i++) {
    if (common[i] == current) {
      ChangeMap(i);
      break;
    }
  }
}

void MapSelectionBox::ChangeMapListCallback(const std::vector<uint>& index_list)
{
  std::vector<InfoMap*> local = MapsList::GetInstance()->lst;

  // Index list is made of indices of local maps: it's a subset
  common.resize(index_list.size());
  for (uint i=0; i<index_list.size(); i++) {
    common[i] = local[index_list[i]];
  }
}

void MapSelectionBox::AllowSelection()
{
  display_only = false;
  //map_preview_after2->Enable();
  NeedRedrawing();
}

void MapSelectionBox::Pack()
{
  uint h = (3*(box->GetSizeY()-20))>>2;
  if (h > 225) h = 225;
  uint w = (h<<2)/3;
  const std::vector<Widget*>& wlist = box->GetWidgets();
  for (uint i=0; i<wlist.size(); i++) {
#if 0 // Cause some refresh problems
    if (wlist[i] == box->GetSelectedWidget())
      wlist[i]->SetSize((size.y<<1)/3, size.y>>1);
    else
#endif
      wlist[i]->SetSize(w, h);
  }
  VBox::Pack();
  ChangeMapCallback();
}

void MapSelectionBox::RefreshBox()
{
  // Remove all internal boxes, and delete them
  box->Clear();

  uint h = 100;
  Point2i img_size((4*h)/3, h);

  PictureWidget *pw;
  bool error = false;
  for (uint i=0; i<common.size(); i++) {
    pw = new PictureWidget(img_size);
    InfoMapBasicAccessor* info = common[i]->LoadBasicInfo();
    if (!info) {
      // Error already reported by LoadBasicInfo()
      MapsList *map_list = MapsList::GetInstance();

      // Crude
      MapsList::iterator it = map_list->lst.begin()
                            + map_list->FindMapById(common[i]->GetRawName());
      delete *it;
      map_list->lst.erase(it);
      error = true;
    } else {
      pw->SetSurface(info->ReadPreview(), PictureWidget::FIT_SCALING);
      pw->Pack();
      box->AddItem(selected_map_index==i, pw, common[i]);
    }
  }

  // Also add random map
  pw = new PictureWidget(img_size);
  pw->SetSurface(random_map_preview, PictureWidget::FIT_SCALING);
  pw->Pack();
  box->AddItem(selected_map_index==common.size(), pw, NULL);

  if (error) {
    // Inform network if need be - will call back up to this very function
    Network::GetInstance()->SendMapsList();
  }
}

bool MapSelectionBox::Update(const Point2i & mousePosition,
                               const Point2i & lastMousePosition) {
  if (box->GetSelectedItem() >= 0 && (uint)box->GetSelectedItem() != selected_map_index) {
    ChangeMap(box->GetSelectedItem());
  }
  return VBox::Update(mousePosition, lastMousePosition);

}
