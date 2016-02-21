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

#ifndef GUI_TABS_H
#define GUI_TABS_H

#include <list>
#include "include/base.h"
#include "gui/widget.h"

class Button;
class Box;
class Text;
class Tab;

class MultiTabs : public Widget
{
private:
  std::list<Tab> tabs;
  Tab* current_tab;
  Text *current_tab_title;

  Button* prev_tab_bt;
  Button* next_tab_bt;
  Point2i tab_size;

  void SetCurrentTab(Tab* _tab);

  void PrevTab();
  void NextTab();

  void DrawHeader(const Point2i &mousePosition) const;

public:
  MultiTabs(const Point2i& size);
  ~MultiTabs();

  void AddNewTab(const std::string& id, const std::string& title, Widget* w);
  const std::string& GetCurrentTabId() const;

  uint GetHeaderHeight() const;

  // from widget
  virtual void NeedRedrawing();
  virtual void Draw(const Point2i &mousePosition) const;
  virtual void Update(const Point2i &mousePosition,
		      const Point2i &lastMousePosition);
  virtual void Pack();

  virtual bool SendKey(const SDL_keysym&);
  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
};

#endif // GUI_TABS_H
