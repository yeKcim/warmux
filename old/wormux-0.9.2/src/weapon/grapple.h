/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Grapple
 *****************************************************************************/

#ifndef GRAPPLE_H
#define GRAPPLE_H
//-----------------------------------------------------------------------------
#include "weapon.h"
#include "include/base.h"
#include <list>
//-----------------------------------------------------------------------------

class GrappleConfig;

class Grapple : public Weapon
{
  private:
    struct rope_node_t
    {
      Point2i pos;
      Double angle;
    };

    uint last_mvt;
    Double last_broken_node_angle;

    // Rope launching data.
    bool attached;
    Sprite* m_hook_sprite;
    Sprite* m_node_sprite;

    SoundSample cable_sound;
    bool move_left_pressed;
    bool move_right_pressed;
    bool move_up_pressed;
    bool move_down_pressed;

  protected:
    void Refresh();
    void p_Deselect() { DetachRope(); };
    bool p_Shoot();

    void GoUp();
    void GoDown();
    void GoLeft();
    void GoRight();
    void StopUp();
    void StopDown();
    void StopLeft();
    void StopRight();

    bool WillBeAttached();
    bool TryAttachRope();
    bool TryAddNode();
    void TryRemoveNodes();

  public:
    enum {
      ATTACH_ROPE,
      ATTACH_NODE,
      DETACH_NODE,
      SET_ROPE_SIZE,
      UPDATE_PLAYER_POSITION
    } grapple_movement_t;

    std::list<rope_node_t> rope_nodes;
    Point2i m_fixation_point;
    bool go_left, go_right;
    Double delta_len ;

    Grapple();
    ~Grapple();
    void Draw();
    virtual void NotifyMove(bool collision);

    GrappleConfig& cfg();

    // Attaching and dettaching nodes rope
    // This is public because of network
    void AttachRope(const Point2i& contact_point);
    void DetachRope();

    void AttachNode(const Point2i& contact_point, Double angle);
    void DetachNode();
    void SetRopeSize(Double length) const;

    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count) const;

    void StartMovingLeft();
    void StopMovingLeft();

    void StartMovingRight();
    void StopMovingRight();

    void StartMovingUp();
    void StopMovingUp();

    void StartMovingDown();
    void StopMovingDown();

    void StartShooting();
    void StopShooting();

    virtual bool IsPreventingLRMovement();
    virtual bool IsPreventingWeaponAngleChanges();
    // Keys management
    void HandleKeyPressed_Up(bool /*slowly*/);
    void HandleKeyReleased_Up(bool /*slowly*/);

    void HandleKeyPressed_Down(bool /*slowly*/);
    void HandleKeyReleased_Down(bool /*slowly*/);

    void HandleKeyPressed_MoveRight(bool /*slowly*/);
    void HandleKeyReleased_MoveRight(bool /*slowly*/);

    void HandleKeyPressed_MoveLeft(bool /*slowly*/);
    void HandleKeyReleased_MoveLeft(bool /*slowly*/);

    void PrintDebugRope();
};

//-----------------------------------------------------------------------------
#endif /* GRAPPLE_H */
