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

#include "gui/grid_box.h"

GridBox::GridBox(uint _lines,
                 uint _columns,
                 uint margin,
                 bool _draw_border) :
  Box(Point2i(-1, -1), _draw_border, _draw_border),
  autoResize(true),
  fixedMargin(margin),
  lines(_lines),
  columns(_columns),
  grid(NULL)
{
  InitGrid();
}

GridBox::GridBox(Profile * profile,
                 const xmlNode * gridBoxNode) :
  Box(profile, gridBoxNode),
  autoResize(false),
  fixedMargin(0),
  lines(10),
  columns(10),
  grid(NULL)
{
}

GridBox::~GridBox(void)
{
  for (uint i = 0; i < lines; ++i) {
    delete [] grid[i];
  }
  delete [] grid;
}

bool GridBox::LoadXMLConfiguration(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  ParseXMLGeometry();
  ParseXMLBoxParameters();

  XmlReader * xmlFile = profile->GetXMLDocument();

  xmlFile->ReadUintAttr(widgetNode, "lines", lines);
  xmlFile->ReadUintAttr(widgetNode, "columns", columns);
  xmlFile->ReadUintAttr(widgetNode, "margin", fixedMargin);

  InitGrid();

  return true;
}

void GridBox::InitGrid(void)
{
  grid = new Widget **[lines];

  for (uint i = 0; i < lines; ++i) {
    grid[i] = new Widget*[columns];
    for (uint j = 0; j < columns; ++j) {
      grid[i][j] = NULL;
    }
  }
}

void GridBox::PlaceWidget(Widget * widget,
                          uint line,
                          uint column)
{
  if (line >= lines || column >= columns) {
    return;
  }
  grid[line][column] = widget;
}

void GridBox::AddWidget(Widget * widget)
{
  ASSERT(widget != NULL);

  for (uint i = 0; i < lines; ++i) {
    for (uint j = 0; j < columns; ++j) {
      if (NULL == grid[i][j]) {
        grid[i][j] = widget;
        WidgetList::AddWidget(widget);
        return;
      }
    }
  }
}

void GridBox::AddWidget(Widget * widget,
                        uint line,
                        uint column)
{
  ASSERT(widget != NULL);
  grid[line][column] = widget;
}

int GridBox::GetMaxHeightByLine(uint line)
{
  int height = 0;
  Widget * widget = NULL;

  for (uint i = 0; i < columns; ++i) {
    widget = grid[line][i];
    if (NULL == widget) {
      continue;
    }
    widget->Pack();
    if (widget->GetSizeY() > height) {
      height = widget->GetSizeY();
    }
  }
  return height + fixedMargin;
}

int GridBox::GetMaxWidthByColumn(uint column)
{
  int width = 0;
  Widget * widget = NULL;

  for (uint i = 0; i < lines; ++i) {
    widget = grid[i][column];
    if (NULL == widget) {
      continue;
    }
    widget->Pack();
    if (widget->GetSizeX() > width) {
      width = widget->GetSizeX();
    }
  }
  return width + fixedMargin;
}

void GridBox::Pack()
{
  if (widget_list.empty()) {
    return;
  }
  int heightMax;
  int widthMax;
  uint x       = position.x;
  uint y       = position.y;
  uint marginX = 0;
  uint marginY = 0;

  int totalWidth = 0;
  for (uint columnIt = 0; columnIt < columns; ++columnIt) {
    totalWidth += GetMaxWidthByColumn(columnIt);
  }
  if (totalWidth < GetSize().x) {
    marginX = (GetSize().x - totalWidth) / (columns + 1);
  }

  int totalHeight = 0;
  for (uint lineIt = 0; lineIt < lines; ++lineIt) {
    totalHeight += GetMaxHeightByLine(lineIt);
  }
  if (totalHeight < GetSize().y) {
    marginY = (GetSize().y - totalHeight) / (lines + 1);
  }

  uint gridWidth  = 0;
  uint gridHeight = 0;

  for (uint lineIt = 0; lineIt < lines; ++lineIt) {
    heightMax = GetMaxHeightByLine(lineIt);

    gridHeight += heightMax + marginY;
    gridWidth = 0;

    for (uint columnIt = 0; columnIt < columns; ++columnIt) {
      widthMax = GetMaxWidthByColumn(columnIt);
      gridWidth += widthMax + marginX;

      if (NULL != grid[lineIt][columnIt]) {
        Widget * widget = grid[lineIt][columnIt];
        widget->Pack();

        widget->SetPosition(x + marginX + ((widthMax + marginX) * columnIt) + ((widthMax - widget->GetSizeX()) / 2),
                            y + marginY + ((heightMax + marginY) * lineIt) + ((heightMax - widget->GetSizeY()) / 2));
        widget->Pack();
      }
    }
  }
  if (true == autoResize) {
    SetSize(gridWidth, gridHeight);
  }
}
