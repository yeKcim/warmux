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
 *****************************************************************************/

#ifndef MEMBER_H
#define MEMBER_H

#include <assert.h>
#include <map>
#include <vector>
#include <WARMUX_point.h>

#include "character/member_type.h"

typedef struct attachment
{
  Point2i point;
  Double  radius;
  Double  angle;

  attachment(const Point2i& val)
    : point(val), radius(0), angle(ZERO) { }

  void SetAnchor(const Point2i& anchor)
  {
    Point2i child_delta = point - anchor;
    uint r = child_delta.x*child_delta.x + child_delta.y*child_delta.y;
    if (r) {
      radius = sqrt_approx(Double(r));
      angle  = child_delta.ComputeAngle();
    }
  }

  void Propagate(Point2i& pos, const Double& mvt_angle, const Double& angle_rad) const
  {
    if (radius) {
      Double angle_init = angle + angle_rad;
      Double angle_new  = angle_init + mvt_angle;
      pos.x  += radius * (cos(angle_new) - cos(angle_init));
      pos.y  += radius * (sin(angle_new) - sin(angle_init));
    }
  }
} attachment;

class v_attached : public std::vector<attachment>
{
public:
  void SetAnchor(const Point2i& anchor)
  {
    std::vector<attachment>::iterator it = begin();
    for (; it != end(); ++it)
      it->SetAnchor(anchor);
  }
};

// Forward declaration
class Member;
class Sprite;
class member_mvt; //defined in movement.h
class Profile;
typedef struct _xmlNode xmlNode;

/*
 * FIXME: this class is either very useless either very badly used.
 * It would be nice to keep members in private section. There is no
 * copy constructor, this is really suspect.... */
class junction
{
public:
  Member * member;
  Member * parent;

  junction():
    member(NULL),
    parent(NULL) {};
};

class Member
{
public:
  typedef std::map<MemberType, v_attached> AttachTypeMap;

private:
  Member* parent;
  Double  angle_rad;
  Double  alpha;
  bool    go_through_ground;
  AttachTypeMap   attached_types;
  Point2i pos;
  Point2d scale;

  typedef std::vector< std::pair<Member*, const v_attached*> > AttachMemberMap;
  AttachMemberMap attached_members;

protected:
  Sprite*     spr;
  std::string name;
  MemberType  type;
  Point2i     anchor;

public:

  virtual ~Member();
  Member(const std::string& name_);
  Member(const xmlNode*     xml,
         const std::string& main_folder);
  Member(const Member& m);

  virtual void Draw(const Point2i & _pos,
                    int             flip_x,
                    LRDirection   direction);

  void RotateSprite();
  void ResetMovement();
  void ApplySqueleton(Member* parent_member);
  void ApplyMovement(const member_mvt &mvt);
  void SetAngle(const Double & angle) { angle_rad = angle; }
  void RefreshSprite(LRDirection direction);

  void SetPos(const Point2i & _pos) { pos = _pos; }

  const Sprite & GetSprite() const { return *spr; }
  Sprite & GetSprite() { return *spr; }
  bool MustRefresh() const;

  const Point2i & GetPos() const { return pos; }

  const Point2i& GetAnchorPos() const { return anchor; }

  const std::string & GetName() const { return name; }
  const MemberType& GetType() const { return type; }

  bool IsGoingThroughGround() const { return go_through_ground; };

  const AttachTypeMap&   GetAttachedTypes() const { return attached_types; }

  void BuildAttachMemberMap(const std::vector<junction*>& skel_lst);
};

class WeaponMember : public Member
{
public:
  WeaponMember(void) : Member("weapon") { }
  void Draw(const Point2i & _pos,
            int             flip_x,
            LRDirection   direction);
};

#endif //MEMBER_H
