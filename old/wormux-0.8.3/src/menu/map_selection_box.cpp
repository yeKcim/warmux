/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

#include "menu/map_selection_box.h"
#include "game/config.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/question.h"
#include "include/action_handler.h"
#include "map/maps_list.h"
#include "network/network.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

MapSelectionBox::MapSelectionBox(const Point2i &_size, bool _display_only) :
  VBox(_size.GetX(), Network::IsConnected()), selected_map_index(0)
{
  display_only = _display_only;

  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml",false);

  // PreviousMap/NextMap buttons
  bt_map_plus = new Button(res, "menu/big_plus", false);
  bt_map_minus = new Button(res, "menu/big_minus", false);

  // random map
  random_map_preview = GetResourceManager().LoadImage(res, "menu/random_map");

  GetResourceManager().UnLoadXMLProfile(res);

  // compute margin width between previews
  uint map_preview_height = _size.GetY() -2*10 -40;

  // Previews
  Box* previews_box = new HBox(map_preview_height+10, false);
  previews_box->SetNoBorder();

   // compute margin width between previews
  uint map_preview_width = map_preview_height*4/3;
  uint total_width_previews = map_preview_width + map_preview_width*3;

  uint margin = 0;

  if ( uint(size.x) > uint(total_width_previews + bt_map_plus->GetSizeX() + bt_map_minus->GetSizeX()
			   + border.x)) {
    margin = (size.x -
              (total_width_previews + bt_map_plus->GetSizeX() + bt_map_minus->GetSizeX() + border.x) ) / 6;
  }

  if (margin < 5) {
    margin = 5;
    uint total_size_wo_margin = size.x - 6*margin - bt_map_plus->GetSizeX() - bt_map_minus->GetSizeX() - border.x;
    map_preview_width = (total_size_wo_margin)/4; // <= total = w + 4*(3/4)w
    map_preview_height = 3/4 * map_preview_width;
  }

  previews_box->SetMargin(margin);

  if (!display_only) {
    previews_box->AddWidget(bt_map_minus);
  } else {
    previews_box->AddWidget(new NullWidget(bt_map_minus->GetSize()));
    delete bt_map_minus;
  }

  map_preview_before2 = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before2);

  map_preview_before = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before);

  // Selected map...
  map_preview_selected = new PictureWidget(Point2i(map_preview_width, map_preview_height));
  previews_box->AddWidget(map_preview_selected);

  map_preview_after = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after);

  map_preview_after2 = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after2);

  if (!display_only) {
    previews_box->AddWidget(bt_map_plus);
  }else {
    previews_box->AddWidget(new NullWidget(bt_map_plus->GetSize()));
    delete bt_map_plus;
  }

  AddWidget(previews_box);

  // Map information
  map_name_label = new Label("Map", W_UNDEF, Font::FONT_SMALL,
			     Font::FONT_BOLD, dark_gray_color, true, false);
  AddWidget(map_name_label);

  map_author_label = new Label("Author", W_UNDEF, Font::FONT_SMALL,
			       Font::FONT_NORMAL, dark_gray_color, true, false);
  AddWidget(map_author_label);

  // Load Maps' list
  uint i = MapsList::GetInstance()->GetActiveMapIndex();

  ChangeMap(i);
}

void MapSelectionBox::ChangeMapDelta(int delta_index)
{
  ASSERT(!display_only);

  int tmp = selected_map_index + delta_index;

  // +1 is for random map!
  tmp = (tmp < 0 ? tmp + MapsList::GetInstance()->lst.size() + 1 : tmp) % (MapsList::GetInstance()->lst.size() + 1);

  ChangeMap(tmp);
}

void MapSelectionBox::ChangeMap(uint index)
{
  int tmp;
  if (index > MapsList::GetInstance()->lst.size()+1) return;

  // Callback other network players
  if (Network::GetInstance()->IsServer()) {

    selected_map_index = index;
    // We need to do it here to send the right map to still not connected clients
    // in distant_cpu::distant_cpu

    if (selected_map_index == MapsList::GetInstance()->lst.size()) { // random map
      MapsList::GetInstance()->SelectMapByName("random");
    } else {
      MapsList::GetInstance()->SelectMapByIndex(index);
    }

    Action* a = new Action(Action::ACTION_MENU_SET_MAP);
    MapsList::GetInstance()->FillActionMenuSetMap(*a);
    ActionHandler::GetInstance()->NewAction(a);
  } else {
    selected_map_index = index;
  }

  // Set Map information
  UpdateMapInfo(map_preview_selected, index, true);

  // Set previews
  tmp = index - 1;
  tmp = (tmp < 0 ? tmp + MapsList::GetInstance()->lst.size() + 1: tmp);
  UpdateMapInfo(map_preview_before, tmp, false);

  tmp = index - 2;
  tmp = (tmp < 0 ? tmp + MapsList::GetInstance()->lst.size() + 1: tmp);
  UpdateMapInfo(map_preview_before2, tmp, false);

  UpdateMapInfo(map_preview_after,  (index + 1) % (MapsList::GetInstance()->lst.size() +1), false);
  UpdateMapInfo(map_preview_after2, (index + 2) % (MapsList::GetInstance()->lst.size() +1), false);
}

void MapSelectionBox::UpdateMapInfo(PictureWidget * widget, uint index, bool selected)
{
  if (index == MapsList::GetInstance()->lst.size()) {
    UpdateRandomMapInfo(widget, selected);
    return;
  }

  InfoMap* info = MapsList::GetInstance()->lst[index];
  try {
    widget->SetSurface(info->ReadPreview(), true, true);
  }
  catch (const char* msg) {
    Question question(Question::WARNING);
    std::string err = Format("Map %s in folder '%s' is invalid: %s",
                             info->GetRawName().c_str(), info->GetDirectory().c_str(), msg);
    std::cerr << err << std::endl;
    question.Set(err, 1, 0);
    question.Ask();

    // Crude
    MapsList::iterator it = MapsList::GetInstance()->lst.begin();
    while (index--)
      it++;
    //delete *it;
    MapsList::GetInstance()->lst.erase(it);
    return;
  }

  if (display_only && !selected)
    widget->Disable();
  else
    widget->Enable();
  // If selected update general information
  if (selected) {
    map_name_label->SetText(MapsList::GetInstance()->lst[index]->ReadFullMapName());
    map_author_label->SetText(MapsList::GetInstance()->lst[index]->ReadAuthorInfo());
  }
}

void MapSelectionBox::UpdateRandomMapInfo(PictureWidget * widget, bool selected)
{
  widget->SetSurface(random_map_preview, true, true);
  if ((display_only && !selected))
    widget->Disable();
  else
    widget->Enable();

  // If selected update general information
  if (selected) {
    map_name_label->SetText(_("Random map"));
    map_author_label->SetText(_("Choose randomly between the different maps"));
  }
}


Widget* MapSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (display_only) return NULL;

  if (!Contains(mousePosition)) return NULL;

  if (button == SDL_BUTTON_LEFT && bt_map_minus->Contains(mousePosition)) {
    ChangeMapDelta(-3);
  } else if (button == SDL_BUTTON_LEFT && map_preview_before2->Contains(mousePosition)) {
    ChangeMapDelta(-2);
  } else if ((button == SDL_BUTTON_LEFT && map_preview_before->Contains(mousePosition))
             || (button == SDL_BUTTON_WHEELUP )) {
    ChangeMapDelta(-1);
  } else if ((button == SDL_BUTTON_LEFT && map_preview_after->Contains(mousePosition))
             || (button == SDL_BUTTON_WHEELDOWN)) {
    ChangeMapDelta(+1);
  } else if (button == SDL_BUTTON_LEFT && map_preview_after2->Contains(mousePosition)) {
    ChangeMapDelta(+2);
  } else if (button == SDL_BUTTON_LEFT && bt_map_plus->Contains(mousePosition)) {
    ChangeMapDelta(+3);
  }

  return NULL;
}

Widget* MapSelectionBox::Click(const Point2i &/*mousePosition*/, uint /*button*/)
{
  return NULL;
}

void MapSelectionBox::ValidMapSelection()
{
  std::string map_name;

  if (selected_map_index == MapsList::GetInstance()->lst.size()) {

      // Choose one and select it!
      map_name = "random";

      if (Network::GetInstance()->IsLocal()) {
	MapsList::GetInstance()->SelectMapByName(map_name);
      }
  } else {
    map_name = MapsList::GetInstance()->lst[selected_map_index]->GetRawName();
    MapsList::GetInstance()->SelectMapByIndex(selected_map_index);
  }

  /* The player chose a map, save it in the main config so that this will be
   * the defaut map at next load of the game */
  Config::GetInstance()->SetMapName(map_name);
}

void MapSelectionBox::ChangeMapCallback()
{
  int index = MapsList::GetInstance()->GetActiveMapIndex();
  ChangeMap(index);
}

void MapSelectionBox::Pack()
{
  VBox::Pack();
  ChangeMapCallback();
}
