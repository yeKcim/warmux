/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include "map_selection_box.h"
#include "include/action_handler.h"
#include "map/maps_list.h"
#include "network/network.h"

MapSelectionBox::MapSelectionBox(const Rectanglei &rect, bool _display_only) :
  HBox(rect, true), selected_map_index(0)
{
  display_only = _display_only;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);

  AddWidget(new PictureWidget(Rectanglei(0,0,46,100), "menu/map_label"));

  // PreviousMap/NextMap buttons
  bt_map_plus = new Button(Point2i(0, 0), res, "menu/big_plus", false);
  bt_map_minus = new Button(Point2i(0, 0), res, "menu/big_minus", false);

  resource_manager.UnLoadXMLProfile(res);

  Box * tmp_map_box = new VBox( Rectanglei(0, 0,
					   rect.GetSizeX()-63, 0), false);
  tmp_map_box->SetBorder( Point2i(0,0) );
  tmp_map_box->SetMargin(0);

  // compute margin width between previews
  uint map_preview_height = rect.GetSizeY() -2*10 -40;

  // Previews
  Box* previews_box = new HBox( Rectanglei(0, 0, 0, map_preview_height+10 ), false);
  previews_box->SetBorder( Point2i(10,0) );

   // compute margin width between previews
  uint map_preview_width = map_preview_height*4/3;
  uint total_width_previews = map_preview_width + map_preview_width*3;

  uint margin = 0;

  if ( uint(tmp_map_box->GetSizeX() - 20) > uint(total_width_previews + bt_map_plus->GetSizeX() + bt_map_minus->GetSizeX())) {
    margin = (tmp_map_box->GetSizeX() - 20 -
	      (total_width_previews + bt_map_plus->GetSizeX() + bt_map_minus->GetSizeX()) ) / 6;
  }

  if (margin < 5) {
    margin = 5;
    uint total_size_wo_margin = tmp_map_box->GetSizeX() - 20 - 6*margin - bt_map_plus->GetSizeX() - bt_map_minus->GetSizeX();
    map_preview_width = (total_size_wo_margin)/4; // <= total = w + 4*(3/4)w
    map_preview_height = 3/4 * map_preview_width;
  }

  previews_box->SetMargin(margin);

  if (!display_only) {
    previews_box->AddWidget(bt_map_minus);
  } else {
    previews_box->AddWidget(new NullWidget(*bt_map_minus));
  }

  map_preview_before2 = new PictureWidget(Rectanglei(0, 0, map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before2);

  map_preview_before = new PictureWidget(Rectanglei(0, 0, map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before);

  // Selected map...
  map_preview_selected = new PictureWidget(Rectanglei(0, 0, map_preview_width, map_preview_height));
  previews_box->AddWidget(map_preview_selected);

  map_preview_after = new PictureWidget(Rectanglei(0, 0, map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after);

  map_preview_after2 = new PictureWidget(Rectanglei(0, 0, map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after2);

  if (!display_only) {
    previews_box->AddWidget(bt_map_plus);
  }else {
    previews_box->AddWidget(new NullWidget(*bt_map_plus));
  }

  tmp_map_box->AddWidget(previews_box);

  if (display_only) {
    map_preview_before2->Disable();
    map_preview_before->Disable();
    map_preview_after->Disable();
    map_preview_after2->Disable();
  }


  // Map information
  map_name_label = new Label("Map", Rectanglei(0,0,0,0), Font::FONT_SMALL, Font::FONT_BOLD, dark_gray_color, true, false);
  tmp_map_box->AddWidget(map_name_label);

  map_author_label = new Label("Author", Rectanglei(0,0,0,0), Font::FONT_SMALL, Font::FONT_NORMAL, dark_gray_color, true, false);
  tmp_map_box->AddWidget(map_author_label);

  AddWidget(tmp_map_box);

  // Load Maps' list
  ChangeMap(MapsList::GetInstance()->GetActiveMapIndex());
}

void MapSelectionBox::ChangeMapDelta(int delta_index)
{
  assert(!display_only);

  int tmp = selected_map_index + delta_index;

  ChangeMap(tmp);
}

void MapSelectionBox::ChangeMap(int index)
{
  if (index < 0 || index > int(MapsList::GetInstance()->lst.size() - 1)) return;

  selected_map_index = index;

  // Callback other network players
  if(Network::GetInstance()->IsServer())
    {
      ActionHandler::GetInstance()->NewAction (new Action(Action::ACTION_MENU_SET_MAP,
					    MapsList::GetInstance()->lst[selected_map_index].ReadName()));
    }

  // Set Map information
  map_preview_selected->SetSurface(MapsList::GetInstance()->lst[selected_map_index].ReadPreview(), true);
  map_name_label->SetText(MapsList::GetInstance()->lst[selected_map_index].ReadName());
  map_author_label->SetText(MapsList::GetInstance()->lst[selected_map_index].ReadAuthorInfo());

  // Set previews
  if (selected_map_index > 0)
    map_preview_before->SetSurface(MapsList::GetInstance()->lst[selected_map_index-1].ReadPreview(), true);
  else
    map_preview_before->SetNoSurface();

  if (selected_map_index > 1)
    map_preview_before2->SetSurface(MapsList::GetInstance()->lst[selected_map_index-2].ReadPreview(), true);
  else
    map_preview_before2->SetNoSurface();

  if (selected_map_index+1 < MapsList::GetInstance()->lst.size() )
    map_preview_after->SetSurface(MapsList::GetInstance()->lst[selected_map_index+1].ReadPreview(), true);
  else
    map_preview_after->SetNoSurface();

  if (selected_map_index+2 < MapsList::GetInstance()->lst.size() )
    map_preview_after2->SetSurface(MapsList::GetInstance()->lst[selected_map_index+2].ReadPreview(), true);
  else
    map_preview_after2->SetNoSurface();
}

Widget* MapSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (display_only) return NULL;

  if (!Contains(mousePosition)) return NULL;

  if (button == SDL_BUTTON_LEFT && map_preview_before2->Contains(mousePosition) ) {
    ChangeMapDelta(-2);
  } else if (   (button == SDL_BUTTON_LEFT && bt_map_minus->Contains(mousePosition))
	     || (button == SDL_BUTTON_LEFT && map_preview_before->Contains(mousePosition))
	     || (button == SDL_BUTTON_WHEELUP )) {
    ChangeMapDelta(-1);
  } else if (   (button == SDL_BUTTON_LEFT && bt_map_plus->Contains(mousePosition))
	     || (button == SDL_BUTTON_LEFT && map_preview_after->Contains(mousePosition))
	     || (button == SDL_BUTTON_WHEELDOWN)) {
    ChangeMapDelta(+1);
  } else if (button == SDL_BUTTON_LEFT && map_preview_after2->Contains(mousePosition)) {
    ChangeMapDelta(+2);
  }

  return NULL;
}

Widget* MapSelectionBox::Click(const Point2i &mousePosition, uint button)
{
  return NULL;
}

void MapSelectionBox::ValidMapSelection()
{
  MapsList::GetInstance()->SelectMapByIndex(selected_map_index);

  /* The player chose a map, save it in the main config so that this will be
   * the defaut map at next load of the game */
  Config::GetInstance()->SetMapName(MapsList::GetInstance()->lst[selected_map_index].ReadName());
}

void MapSelectionBox::ChangeMapCallback()
{
  int index = MapsList::GetInstance()->GetActiveMapIndex();
  ChangeMap(index);
}
