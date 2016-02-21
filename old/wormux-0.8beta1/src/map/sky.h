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
 * Sky: background of the map
 *****************************************************************************/

#ifndef SKY_H
#define SKY_H

#include "tile.h"
#include <list>
#include "graphic/surface.h"
#include "include/base.h"
#include "tool/point.h"
#include "tool/rectangle.h"

class Sky{
private:
	Surface image;
	Point2i lastPos;
	Point2i tstVect;
	Point2i margin;
	void RedrawParticleList(std::list<Rectanglei> &list);
	void RedrawParticle(const Rectanglei &particle) const;
	Point2i GetSkyPos() const;

public:
	Sky();
	void Init();
	void Reset();
	void Draw();
	void Free();
};

#endif
