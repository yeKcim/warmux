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
 * Grid Box
 *****************************************************************************/

#ifndef GUI_GRID_BOX_H
#define GUI_GRID_BOX_H

#include "gui/box.h"

class GridBox : public Box
{
  private:
    bool autoResize;
    uint fixedMargin;
    uint lines;
    uint columns;
    Widget *** grid;

    uint NbWidgetsPerLine(uint nb_total_widgets);
    void PlaceWidget(Widget * widget,
                     uint line,
                     uint column);
    int GetMaxHeightByLine(uint line);
    int GetMaxWidthByColumn(uint column);
    void InitGrid(void);

  public:
    GridBox(uint lines,
            uint columns,
            uint margin,
            bool _draw_border = true);
    GridBox(Profile * _profile,
            const xmlNode * _gridBoxNode);
    virtual ~GridBox(void);
    virtual void AddWidget(Widget * widget);
    virtual void AddWidget(Widget * widget,
                           uint x,
                           uint y);
    virtual bool LoadXMLConfiguration(void);
    virtual void Pack();

    Point2i GetDefaultBoxSize(const Point2i& full_size) const
    {
      return Point2i(full_size.x/columns-fixedMargin, full_size.y/lines-fixedMargin);
    }
};

#endif

