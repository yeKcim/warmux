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
      double angle;
      int sense;
    };

    uint last_mvt;
    double last_broken_node_angle;
    double last_broken_node_sense;

    // Rope launching data.
    bool m_attaching;
    double m_initial_angle;
    uint m_launch_time;
    uint m_hooked_time;
    Sprite* m_hook_sprite;
    Sprite* m_node_sprite;

    SoundSample cable_sound;

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
    bool TryAddNode(int CurrentSense) ;
    bool TryRemoveNodes(int CurrentSense) ;

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
    double delta_len ;

    Grapple();
    ~Grapple();
    void Draw();
    virtual void NotifyMove(bool collision);

    virtual void ActionStopUse() { DetachRope(); };
    // force detaching rope if time is out
    virtual void SignalTurnEnd() { p_Deselect(); };

    GrappleConfig& cfg();

    // Attaching and dettaching nodes rope
    // This is public because of network
    void AttachRope(const Point2i& contact_point);
    void DetachRope();

    void AttachNode(const Point2i& contact_point,
		    double angle,
		    int sense);
    void DetachNode();
    void SetRopeSize(double length) const;

    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count) const;

    // Keys management
    void HandleKeyPressed_Up(bool shift);
    void HandleKeyRefreshed_Up(bool shift);
    void HandleKeyReleased_Up(bool shift);

    void HandleKeyPressed_Down(bool shift);
    void HandleKeyRefreshed_Down(bool shift);
    void HandleKeyReleased_Down(bool shift);

    void HandleKeyPressed_MoveRight(bool shift);
    void HandleKeyRefreshed_MoveRight(bool shift);
    void HandleKeyReleased_MoveRight(bool shift);

    void HandleKeyPressed_MoveLeft(bool shift);
    void HandleKeyRefreshed_MoveLeft(bool shift);
    void HandleKeyReleased_MoveLeft(bool shift);

    void HandleKeyPressed_Shoot(bool shift);
    void HandleKeyRefreshed_Shoot(bool) { };
    void HandleKeyReleased_Shoot(bool) { };

    void PrintDebugRope();
};

//-----------------------------------------------------------------------------
#endif /* GRAPPLE_H */
