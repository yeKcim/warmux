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
 * Draw a graph of a series of points according to various parameters
 *****************************************************************************/

#ifndef GRAPH_CANVAS_H
#define GRAPH_CANVAS_H

#include <vector>
#include "gui/widget.h"

// Currently only handles positive values
class GraphCanvas : public Widget
{
public:
  typedef std::pair<float, float> Value;
  typedef struct
  {
    const Surface      *item;
    Color              color;
    float              xmax, ymax;
    std::vector<Value> list;
  } Result;
private:
  Surface              xaxis, yaxis;
  std::vector<Result>  results;
  uint                 thickness;

  void SetAxis(const std::string& xname, const std::string& yname);

public:
  GraphCanvas(const Point2i& size,
              const std::string& xname, const std::string& yname,
              uint thick=2);
  GraphCanvas(const Point2i& size,
              const std::string& xname, const std::string& yname,
              std::vector<Result>& res, uint thick=2);
  virtual ~GraphCanvas() {};
  virtual void Draw(const Point2i&);

  virtual void DrawGraph(uint i, float xmax, float xmin,
                         int x, float xscale,
                         int y, float yscale) const;
  virtual void DrawGraph(int x, int y, int w, int h) const;

  virtual void Pack() {};

  void AddResult(const Result& newer) { results.push_back(newer); }
  void UnsetResults() { results.clear(); }

  static void FindMax(Result& res);
};


#endif // GRAPH_CANVAS_H
