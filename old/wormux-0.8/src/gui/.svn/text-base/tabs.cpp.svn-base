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
 * Tabs
 *****************************************************************************/

#include "graphic/colors.h"
#include "graphic/font.h"
#include "graphic/text.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/tabs.h"
#include "tool/resource_manager.h"

static Point2i widget_size(150, 150);

class Tab
{
private:
  std::string id;
  std::string title;

public:
  Widget* box;

public:
  Tab(const std::string& _id, const std::string& _title, Widget* _box) :
    id(_id), title(_title), box(_box) {};

  ~Tab() {};

  const std::string& GetTitle() const { return title; };
  const std::string& GetId() const { return id; };
};

#define CIRCULAR_TABS   1

MultiTabs::MultiTabs(const Point2i& size):
  Widget(size), current_tab(NULL)
{
  tab_size = Point2i(size.x, size.y - 32);

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);

  prev_tab_bt = new Button(res, "menu/really_big_minus", false);
  next_tab_bt = new Button(res, "menu/really_big_plus", false);

  current_tab_title = new Text("No tab!", primary_red_color, Font::FONT_MEDIUM, Font::FONT_BOLD, true);

  Widget::SetBorder(defaultOptionColorRect, 2);
  Widget::SetBackgroundColor(defaultOptionColorBox);

  resource_manager.UnLoadXMLProfile(res);
}

MultiTabs::~MultiTabs()
{
  for(std::list<Tab>::iterator t=tabs.begin();
      t != tabs.end();
      t++)
    delete t->box;

  tabs.clear();
}

void MultiTabs::SetCurrentTab(Tab* _tab)
{
  ASSERT(_tab != NULL);

  if (current_tab != _tab) {
    current_tab = _tab;
    current_tab_title->Set(current_tab->GetTitle());
  }
  NeedRedrawing();
}

void MultiTabs::PrevTab()
{
  if (current_tab == &(tabs.front()))
  {
#if CIRCULAR_TABS
    SetCurrentTab(&tabs.back());
#endif
    return;
  }

  std::list<Tab>::reverse_iterator it;
  for (it = tabs.rbegin(); it != tabs.rend(); it++)
    if ( &(*it) == current_tab)
      break;

  it++;
  SetCurrentTab( &(*it));
}

void MultiTabs::NextTab()
{
  if (current_tab == &(tabs.back()))
  {
#if CIRCULAR_TABS
    SetCurrentTab(&tabs.front());
#endif
    return;
  }

  std::list<Tab>::iterator it;
  for (it = tabs.begin(); it != tabs.end(); it++) {
    if ( &(*it) == current_tab)
      break;
  }
  it++;
  SetCurrentTab( &(*it));
}

void MultiTabs::AddNewTab(const std::string& id, const std::string& title, Widget * w)
{
  Tab tab(id, title, w);
  tabs.push_back(tab);
  w->SetContainer(this);

  SetCurrentTab(&(tabs.front()));
}

void MultiTabs::DrawHeader(const Point2i &mousePosition) const
{
  // Draw the buttons to change tab
  if (tabs.size() != 1)
#if !CIRCULAR_TABS
  if (current_tab != &(tabs.front()))
#endif
    prev_tab_bt->Draw(mousePosition);

  if (tabs.size() != 1)
#if !CIRCULAR_TABS
  if (current_tab != &(tabs.back()))
#endif
    next_tab_bt->Draw(mousePosition);

  // Draw the title of the current tab
  uint center_x = (prev_tab_bt->GetPositionX() + prev_tab_bt->GetSizeX()
		 + next_tab_bt->GetPositionX() )/2;
  current_tab_title->DrawCenterTop(Point2i(center_x, position.y) + 5);
}

void MultiTabs::Draw(const Point2i &mousePosition) const
{
  // Draw the header
  DrawHeader(mousePosition);

  // Draw the current tab
  if (current_tab)
    current_tab->box->Draw(mousePosition);
}

void MultiTabs::Update(const Point2i &mousePosition,
		       const Point2i &lastMousePosition)
{
  Rectanglei header(position.x, position.y,
		    next_tab_bt->GetPositionX() + next_tab_bt->GetSizeX() - position.x,
		    prev_tab_bt->GetSizeY());

  if (need_redrawing) {
    Widget::Update(mousePosition, lastMousePosition);

  } else if ((header.Contains(mousePosition) && mousePosition != lastMousePosition) ||
	     (header.Contains(lastMousePosition) && !header.Contains(mousePosition))) {
    RedrawBackground(header);
    DrawHeader(mousePosition);
  }

  if (current_tab)
    current_tab->box->Update(mousePosition, lastMousePosition);

  need_redrawing = false;
}

void MultiTabs::Pack()
{
  // Update buttons position
  uint margin = 5;
  prev_tab_bt->SetPosition(position.x + margin, position.y);
  next_tab_bt->SetPosition(position.x + size.x - margin - next_tab_bt->GetSizeX(), position.y);

  // Update tabs position
  Point2i tab_pos(position.x + margin,
		  position.y + GetHeaderHeight());
  Point2i tab_size(size.x - 2*margin,
		   size.y - GetHeaderHeight() - margin);

  std::list<Tab>::iterator it;
  for (it = tabs.begin(); it != tabs.end(); it++)
    {
      (*it).box->SetPosition(tab_pos);
      (*it).box->SetSize(tab_size);
      (*it).box->Pack();
    }
}

void MultiTabs::NeedRedrawing()
{
  need_redrawing = true;

  if (current_tab)
    current_tab->box->NeedRedrawing();
}

bool MultiTabs::SendKey(const SDL_keysym& key)
{
  if (current_tab)
    return current_tab->box->SendKey(key);

  return false;
}

Widget* MultiTabs::Click(const Point2i &mousePosition, uint button)
{
  if (current_tab)
    return current_tab->box->Click(mousePosition, button);

  return NULL;
}

Widget* MultiTabs::ClickUp(const Point2i &mousePosition, uint button)
{
  if (tabs.size() != 1) {

    Rectanglei rect_header(position.x,
			   position.y,
			   size.x,
			   prev_tab_bt->GetSizeY());

    if (button == SDL_BUTTON_LEFT && prev_tab_bt->Contains(mousePosition)) {
      PrevTab();
      return this;

    } else if (button == SDL_BUTTON_LEFT && next_tab_bt->Contains(mousePosition)) {
      NextTab();
      return this;

    } else if (button == SDL_BUTTON_WHEELDOWN && rect_header.Contains(mousePosition)) {
      PrevTab();
      return this;

    } else if (button == SDL_BUTTON_WHEELUP && rect_header.Contains(mousePosition)) {
      NextTab();
      return this;
    }
  }

  if (current_tab)
    return current_tab->box->ClickUp(mousePosition, button);

  return NULL;
}

const std::string& MultiTabs::GetCurrentTabId() const
{
  ASSERT(current_tab);
  return current_tab->GetId();
}

uint MultiTabs::GetHeaderHeight() const
{
  uint header_h = std::max(current_tab_title->GetHeight(),
			   prev_tab_bt->GetSizeY());
  header_h += 5;
  return header_h;
}
