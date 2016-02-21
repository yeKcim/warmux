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
 * Arrow on top of the active character
 *****************************************************************************/

#include "interface/cursor.h"
#include "character/character.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "graphic/effects.h"
#include "graphic/sprite.h"
#include "map/camera.h"
#include "team/teams_list.h" // ActiveCharacter()
#include "tool/resource_manager.h"

const uint y_min = 5; //number of pixels between the bottom of the arrow and the top of the sprite

CharacterCursor::CharacterCursor()
{
  arrow = NULL;

  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);
  arrow_jump = GetResourceManager().LoadSprite( res, "gfx/arrow-jump");
  arrow_change = GetResourceManager().LoadSprite( res, "gfx/arrow-change");
  GetResourceManager().UnLoadXMLProfile( res);
}

CharacterCursor::~CharacterCursor()
{
  delete arrow_jump;
  delete arrow_change;
}

// Draw cursor
void CharacterCursor::Draw()
{
  if (!arrow) return;
  if (ActiveCharacter().IsGhost()) return;

  // Draw cursor arround character
  Point2i center = ActiveCharacter().GetCenter();
  uint x = center.x - arrow->GetWidth()/2;
  uint y = ActiveCharacter().GetY() - arrow->GetHeight() - y_min;

  arrow->Draw( Point2i(x, y) );
}

void CharacterCursor::Refresh()
{
  if (!arrow) return;

  if (arrow->IsFinished()) {
    arrow = NULL;
    return;
  }

  arrow->Update();
}

// Hide the cursor
void CharacterCursor::Hide()
{
  if (!arrow) return;

  arrow->animation.SetLoopMode(false);

  // To force direct stop when using "arrow change"
  if (arrow == arrow_change)
    arrow->animation.Finish();
}

void CharacterCursor::Reset()
{
  arrow_jump->animation.SetLoopMode(true);
  arrow_change->animation.SetLoopMode(true);
  arrow = NULL;
}

void CharacterCursor::FollowActiveCharacter()
{
  if (arrow)
    return;

  if (GameMode::GetInstance()->AllowCharacterSelection())
    arrow = arrow_change;
  else
    arrow = arrow_jump;

  arrow->animation.SetLoopMode(true);
  arrow->Start();
}
