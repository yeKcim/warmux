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
 *****************************************************************************/

#ifndef MEMBER_H
#define MEMBER_H
#include <map>
#include <vector>
#include "body.h"
#include "movement.h"
#include "graphic/sprite.h"
#include "tool/resource_manager.h"
#include "tool/point.h"
#include "tool/xml_document.h"

typedef std::vector<Point2f> v_attached;

class c_junction; //defined in body.h

class Member
{
  /* If you need this, implement it (correctly) */
  Member operator=(const Member&);
  /**********************************************/

  Member* parent;
  double angle_rad;
protected:
  Point2f anchor;

public:
  Sprite* spr;

  std::string name;
  std::map<std::string, v_attached> attached_members;

  Point2f pos;
  Point2f scale;
  float alpha;
  std::string type;
  bool go_through_ground;

  virtual ~Member();
  Member(xmlpp::Element *xml, Profile* res);
  Member(const Member& m);
  virtual void Draw(const Point2i & _pos, int flip_x, int direction);
  void RotateSprite();
  void ResetMovement();
  void ApplySqueleton(Member* parent_member);
  void ApplyMovement(member_mvt& mvt, std::vector<class c_junction>& squel_lst);
  const Point2i GetPos();
  inline void SetAngle(const double &angle) { angle_rad = angle; };
};

class WeaponMember : public Member
{
public:
  WeaponMember();
  ~WeaponMember();
  void Draw(const Point2i & _pos, int flip_x, int direction);
};

#endif //MEMBER_H
