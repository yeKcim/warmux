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
 *****************************************************************************/

#ifndef MEMBER_H
#define MEMBER_H
#include <map>
#include <vector>
#include "tool/point.h"

typedef std::vector<Point2f> v_attached;

// Forward declaration
class Sprite;
class c_junction; //defined in body.h
class member_mvt; //defined in movement.h
class Profile;
typedef struct _xmlNode xmlNode;

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
  Member(const xmlNode* xml, const Profile* res);
  Member(const Member& m);
  virtual void Draw(const Point2i & _pos, int flip_x, int direction);
  void RotateSprite();
  void ResetMovement()
  {
    pos.x = 0;
    pos.y = 0;
    angle_rad = 0;
    alpha = 1.0;
    scale.x = 1.0;
    scale.y = 1.0;
  }
  void ApplySqueleton(Member* parent_member);
  void ApplyMovement(const member_mvt& mvt, std::vector<class c_junction>& skel_lst);
  const Point2i GetPos() { return Point2i((int)pos.x, (int)pos.y); };
  const Point2i GetAnchorPos() { return Point2i((int)anchor.x, (int)anchor.y); };
  void SetAngle(const double &angle) { angle_rad = angle; };
};

class WeaponMember : public Member
{
public:
  WeaponMember();
  ~WeaponMember();
  void Draw(const Point2i & _pos, int flip_x, int direction);
};

#endif //MEMBER_H
