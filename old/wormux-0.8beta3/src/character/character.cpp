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
 * Refresh d'un ver de terre.
 *****************************************************************************/

#include <sstream>
#include <iostream>
#include "character/body.h"
#include "character/character.h"
#include "character/move.h"
#include "character/damage_stats.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/time.h"
#include "graphic/text.h"
#include "include/action_handler.h"
#include "interface/interface.h"
#include "interface/cursor.h"
#include "map/camera.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/macro.h"
#include "tool/random.h"
#include "tool/math_tools.h"
#include "weapon/explosion.h"

const uint HAUT_FONT_MIX = 13;

// Space between the name, the skin and the energy bar
const uint ESPACE = 3; // pixels
const uint do_nothing_timeout = 5000;
const double MIN_SPEED_TO_FLY = 15.0;

// Pause for the animation
#ifdef DEBUG
//#define ANIME_VITE
#endif
#ifdef ANIME_VITE
  const uint ANIM_PAUSE_MIN = 100;
  const uint ANIM_PAUSE_MAX = 150;
#else
  const uint ANIM_PAUSE_MIN = 5*1000;
  const uint ANIM_PAUSE_MAX = 60*1000;
#endif

#ifdef DEBUG
//#define DEBUG_STATS
#endif

// Energy bar
const uint LARG_ENERGIE = 40;
const uint HAUT_ENERGIE = 6;

// Delta angle used to move the crosshair
const double DELTA_CROSSHAIR = 0.035; /* ~1 degree */

/* FIXME This methode is really strange, all this should probably be done in
 * constructor of Body...*/
void Character::SetBody(Body* char_body)
{
  body = char_body;
  body->SetOwner(this);
  SetClothe("normal");
  SetMovement("breathe");

  SetDirection(randomSync.GetBool() ? DIRECTION_LEFT : DIRECTION_RIGHT);
  body->SetFrame(randomObj.GetLong(0, body->GetFrameCount()-1));
  SetSize(body->GetSize());
}

Character::Character (Team& my_team, const std::string &name, Body *char_body) :
  PhysicalObj("character"),
  character_name(name),
  m_team(my_team),
  step_sound_played(true),
  prepare_shoot(false),
  back_jumping(false),
  death_explosion(true),
  firing_angle(0),
  disease_damage_per_turn(0),
  disease_duration(0),
  damage_stats(new DamageStatistics(*this)),
  energy_bar(),
  survivals(0),
  name_text(NULL),
  pause_bouge_dg(0),
  do_nothing_time(0),
  walking_time(0),
  animation_time(Time::GetInstance()->Read() + randomObj.GetLong(ANIM_PAUSE_MIN,ANIM_PAUSE_MAX)),
  lost_energy(0),
  hidden(false),
  channel_step(-1),
  bubble_engine(new ParticleEngine(500)),
  previous_strength(0),
  body(NULL)
{
  m_is_character = true;
  SetCollisionModel(false, true, true);
  /* body stuff */
  ASSERT(char_body);
  SetBody(char_body);

  ResetConstants();

  // Name Text object
  if (Config::GetInstance()->GetDisplayNameCharacter())
    name_text = new Text(character_name);
  else
    name_text = NULL;

#ifdef DEBUG_SKIN
    name_text = new Text(" ");
#endif

  // Energy
  energy = GameMode::GetInstance()->character.init_energy;
  energy_bar.InitVal (GameMode::GetInstance()->character.init_energy,
                      0,
                      GameMode::GetInstance()->character.init_energy);
  energy_bar.InitPos (0,0, LARG_ENERGIE, HAUT_ENERGIE);
  energy_bar.SetBorderColor( black_color );
  energy_bar.SetBackgroundColor( gray_color );

  SetEnergy(GameMode::GetInstance()->character.init_energy);
  MSG_DEBUG("character", "Load character %s", character_name.c_str());
}

Character::Character (const Character& acharacter) :
  PhysicalObj(acharacter),
  character_name(acharacter.character_name),
  m_team(acharacter.m_team),
  step_sound_played(acharacter.step_sound_played),
  prepare_shoot(acharacter.prepare_shoot),
  back_jumping(acharacter.back_jumping),
  death_explosion(acharacter.death_explosion),
  firing_angle(acharacter.firing_angle),
  disease_damage_per_turn(acharacter.disease_damage_per_turn),
  disease_duration(acharacter.disease_duration),
  damage_stats(new DamageStatistics(*acharacter.damage_stats, *this)),
  energy_bar(acharacter.energy_bar),
  survivals(acharacter.survivals),
  name_text(NULL),
  pause_bouge_dg(acharacter.pause_bouge_dg),
  do_nothing_time(acharacter.do_nothing_time),
  walking_time(acharacter.walking_time),
  animation_time(acharacter.animation_time),
  lost_energy(acharacter.lost_energy),
  hidden(acharacter.hidden),
  channel_step(acharacter.channel_step),
  bubble_engine(new ParticleEngine(250)),
  previous_strength(acharacter.previous_strength),
  body(NULL)
{
  if (acharacter.body)
    SetBody(new Body(*acharacter.body));
  if(acharacter.name_text)
    name_text = new Text(*acharacter.name_text);

#ifdef DEBUG_SKIN
    skin_text = new Text(" ");
#endif

}

Character::~Character()
{
  MSG_DEBUG("character", "Unload character %s", character_name.c_str());
  if(body)
    delete body;
  if(name_text)
    delete name_text;
  if(bubble_engine)
    delete bubble_engine;
  body          = NULL;
  name_text     = NULL;
  bubble_engine = NULL;
#ifdef DEBUG_SKIN
  delete skin_text;
#endif
}

void Character::SignalDrowning()
{
  // Follow character
  Camera::GetInstance()->FollowObject(this, true);
  // Set energy
  SetEnergy(0);
  SetMovement("drowned");
  jukebox.Play (GetTeam().GetSoundProfile(),"sink");
  Game::GetInstance()->SignalCharacterDeath (this);
}

// Signal the character death (short life as you can notice)
// May you rest in peace young one.
void Character::SignalGhostState (bool was_dead)
{
  // Report to damage performer this character lost all of its energy
  ActiveCharacter().damage_stats->MadeDamage(GetEnergy(), *this);

  MSG_DEBUG("character", "ghost");
  // Signal the death
  if (!was_dead) Game::GetInstance()->SignalCharacterDeath (this);
}

void Character::SetDirection (BodyDirection_t nv_direction)
{
  body->SetDirection(nv_direction);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
  m_team.crosshair.Refresh(GetFiringAngle());
}

void Character::DrawEnergyBar(int dy) const
{
  if( IsDead() )
        return;

  energy_bar.DrawXY( Point2i( GetCenterX() - energy_bar.GetWidth() / 2, GetY() + dy)
                     - Camera::GetInstance()->GetPosition() );
}

void Character::DrawName (int dy) const
{
  if(IsDead()) return;

  const int x =  GetCenterX();
  const int y = GetY()+dy;

  if (Config::GetInstance()->GetDisplayNameCharacter())
  {
    name_text->DrawCenterTopOnMap(Point2i(x,y));
  }

}

void Character::ResetDamageStats()
{
  damage_stats->ResetDamage();
}

void Character::SetEnergyDelta(int delta, bool do_report)
{
  // If already dead, do nothing
  if (IsDead()) return;

  // Report damage to damage performer
  if (do_report)
    ActiveCharacter().damage_stats->MadeDamage(-delta, *this);

  uint saved_energy = GetEnergy();

  // Update energy
  SetEnergy(GetEnergy() + delta);

  if(IsDead()) return;

  // Compute energy lost
  if (delta < 0) {
    lost_energy += (int)GetEnergy() - (int)saved_energy;

    if ( lost_energy > -33 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_light");
    else if ( lost_energy > -66 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_medium");
    else
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_high");
  } else
    lost_energy = 0;

  // "Friendly fire !!"
  if ( !IsActiveCharacter() && ActiveTeam().IsSameAs(m_team) )
    jukebox.Play (GetTeam().GetSoundProfile(), "friendly_fire");
}

void Character::SetEnergy(int new_energy)
{
  if(!Network::GetInstance()->IsLocal())
  {
    if( m_alive == DEAD && new_energy > 0)
    {
      printf("%s have been resurrected\n", GetName().c_str());
      m_alive = ALIVE;
      SetClothe("normal");
      SetMovement("breathe");
    }
  }

  //ASSERT( m_alive != DEAD );
  if(IsDead()) return;

  // Change energy
  energy = InRange_Long((int)new_energy, 0,
                     GameMode::GetInstance()->character.max_energy);
  energy_bar.Actu(energy);

  // Dead character ?
  if (GetEnergy() <= 0) Die();
}

void Character::Die()
{
  ASSERT (m_alive == ALIVE || m_alive == DROWNED);

  MSG_DEBUG("character", "Dying");

  if (m_alive != DROWNED)
  {
    m_alive = DEAD;

    SetEnergy(0);

    jukebox.Play(GetTeam().GetSoundProfile(),"death");
    body->SetRotation(0.0);
    SetClothe("dead");
    SetMovement("breathe");
    SetCollisionModel(false, false, false);

    if(death_explosion)
      ApplyExplosion(GetCenter(), GameMode::GetInstance()->death_explosion_cfg);
    ASSERT(IsDead());

    // Signal the death
    Game::GetInstance()->SignalCharacterDeath (this);
  }

  damage_stats->SetDeathTime(Time::GetInstance()->Read());
}

void Character::SetLifeState(alive_t state)
{
  if (m_alive == state)
    return;

  std::cerr << "Force life's state of "<< GetName() << " to m_alive = " << state << std::endl;
  m_alive = state;
}

void Character::Draw()
{
  if (hidden) return; // Character is teleporting...

  // Gone in another world ?
  if (IsGhost()) return;

  // Character is visible on carema? If not, just leave the function
  // WARNING, this optimization is disabled if it is the active character
  // because there could be some tricks in the drawing of the weapon (cf bug #10242)
  if (&ActiveCharacter() != this) {
    Rectanglei rect(GetPosition(), Vector2<int>(GetWidth(), GetHeight()));
    if (!rect.Intersect(*Camera::GetInstance())) return;
  }

  bool dessine_perte = (lost_energy != 0);
  if ((&ActiveCharacter() == this
    && Game::GetInstance()->ReadState() != Game::END_TURN)
    || IsDead()
     )
    dessine_perte = false;

  if(Game::GetInstance()->ReadState() == Game::END_TURN && body->IsWalking())
    body->ResetWalk();

  if(Time::GetInstance()->Read() > animation_time && &ActiveCharacter()!=this && !IsDead()
  && body->GetMovement().substr(0,9) != "animation"
  &&  body->GetClothe().substr(0,9) != "animation")
  {
    body->PlayAnimation();
    animation_time = Time::GetInstance()->Read() + body->GetMovementDuration() + randomObj.GetLong(ANIM_PAUSE_MIN,ANIM_PAUSE_MAX);
  }

  // Stop the animation or the black skin if we are playing
  if(&ActiveCharacter() == this
  &&(body->GetMovement().substr(0,9) == "animation"
  || body->GetClothe().substr(0,9) == "animation"
  || body->GetClothe() == "black"))
  {
    SetClothe("normal");
    SetMovement("breathe");
  }

  // Stop flying if we don't go fast enough
  double n, a;
  GetSpeed(n, a);
  if(body->GetMovement() == "fly" && n < MIN_SPEED_TO_FLY)
    SetMovement("breathe");


  // Refresh the body (needed to determine if "weapon-*-begin-shoot" is finnished)
  body->Build();

  if(prepare_shoot)
  {
    if(body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot")
    {
      // if the movement is finnished, shoot !
      DoShoot();
      prepare_shoot = false;
    }
  }

  Point2i pos = GetPosition();
  body->Draw(pos);

   // Draw energy bar
  int dy = -((int)ESPACE);
  bool est_ver_actif = (this == &ActiveCharacter());
  Config * config = Config::GetInstance();
  bool display_energy = config->GetDisplayEnergyCharacter();
  display_energy &= !est_ver_actif || (Game::GetInstance()->ReadState() != Game::PLAYING);
  display_energy |= dessine_perte;
  display_energy &= !IsDead();
  if (display_energy)
  {
    dy -= HAUT_ENERGIE;
    DrawEnergyBar (dy);
    dy -= ESPACE;
  }

  // Draw name
  if (config->GetDisplayNameCharacter() && !est_ver_actif)
  {
    dy -= HAUT_FONT_MIX;
    DrawName (dy);
    dy -= ESPACE;
  }

#ifdef DEBUG_SKIN
  dy -= HAUT_FONT_MIX;
  skin_text->Set(body->GetClothe() + " " + body->GetMovement());
  skin_text->DrawCenterTopOnMap(Point2i(GetX(), GetY() - dy));
  dy -= ESPACE;
#endif

  // Draw lost energy
  if (dessine_perte)
  {
    std::ostringstream ss;
    ss << lost_energy;
    dy -= HAUT_FONT_MIX;
    (*Font::GetInstance(Font::FONT_SMALL)).WriteCenterTop (
        GetPosition() - Camera::GetInstance()->GetPosition() + Point2i( GetWidth()/2, dy),
        ss.str(), white_color);
  }

}

void Character::Jump(double strength, double angle /*in radian */)
{
  do_nothing_time = Time::GetInstance()->Read();
  walking_time = Time::GetInstance()->Read();

  if (!CanJump() && ActiveTeam().IsLocal()) return;

  SetMovement("jump");

  // Jump !
  if (GetDirection() == DIRECTION_LEFT) angle = InverseAngle(angle);
  SetSpeed (strength, angle);
}

void Character::Jump()
{
  MSG_DEBUG("character", "Jump");
  jukebox.Play (ActiveTeam().GetSoundProfile(), "jump");
  Jump(GameMode::GetInstance()->character.jump_strength,
       GameMode::GetInstance()->character.jump_angle);
}

void Character::HighJump()
{
  MSG_DEBUG("character", "HighJump");
  jukebox.Play (ActiveTeam().GetSoundProfile(), "superjump");
  Jump(GameMode::GetInstance()->character.super_jump_strength,
       GameMode::GetInstance()->character.super_jump_angle);
}

void Character::BackJump()
{
  MSG_DEBUG("character", "BackJump");
  back_jumping = true;
  jukebox.Play (ActiveTeam().GetSoundProfile(), "jump");
  Jump(GameMode::GetInstance()->character.back_jump_strength,
       GameMode::GetInstance()->character.back_jump_angle);
}

void Character::PrepareShoot()
{
  MSG_DEBUG("weapon.shoot", "-> begin");
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot");
  if (body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot")
  {
    MSG_DEBUG("weapon.shoot", "-> call DoShoot");
    // If a movement is defined for this weapon, just shoot
    DoShoot();
    MSG_DEBUG("weapon.shoot", "<- end of call DoShoot");
  }
  else
    prepare_shoot = true;
  MSG_DEBUG("weapon.shoot", "<- end");
}

void Character::DoShoot()
{
  if (Game::GetInstance()->ReadState() != Game::PLAYING)
    return; // hack related to bugs 8656 and 9462

  MSG_DEBUG("weapon.shoot", "-> begin");
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-end-shoot");
  body->Build(); // Refresh the body
  body->UpdateWeaponPosition(GetPosition());
  damage_stats->OneMoreShot();
  ActiveTeam().AccessWeapon().Shoot();
  MSG_DEBUG("weapon.shoot", "<- end");
}

void Character::Refresh()
{
  if (IsGhost()) return;

  UpdatePosition();

  if (IsDead()) return;

  Time * global_time = Time::GetInstance();

  // center on character who is falling
  if(FootsInVacuum()) {
    Camera::GetInstance()->FollowObject(this, true);
  }

  if(IsDiseased())
  {
    Point2i bubble_pos = GetPosition();
    if(GetDirection() == DIRECTION_LEFT)
      bubble_pos.x += GetWidth();
    bubble_engine->AddPeriodic(bubble_pos, particle_ILL_BUBBLE, false,
                              - M_PI_2 - (float)GetDirection() * M_PI_4, 20.0);
  }

  if( IsActiveCharacter() && Game::GetInstance()->ReadState() == Game::PLAYING)
  {
    if(do_nothing_time + do_nothing_timeout < global_time->Read())
      CharacterCursor::GetInstance()->FollowActiveCharacter();

    if(walking_time + 1000 < global_time->Read() && body->GetMovement().find("-shoot") == std::string::npos)
    if(body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-select")
      body->SetMovement("weapon-" + ActiveTeam().GetWeapon().GetID() + "-select");
  }
  else
  {
    if(body->GetMovement() == "weapon-" + ActiveTeam().GetWeapon().GetID() + "-select")
      body->SetMovement("breathe");
  }

  if(body->IsWalking() && body->GetMovement() == "walk")
  {
    // Play the step sound only twice during the walking animation
    uint frame_nbr = body->GetFrameCount();
    uint cur = body->GetFrame();
    frame_nbr /= 2;
    cur %= frame_nbr;

    if(cur < frame_nbr / 2 && !step_sound_played)
    {
      step_sound_played = true;
      jukebox.Play (GetTeam().GetSoundProfile(),"step");
    }

    if(cur > frame_nbr / 2)
      step_sound_played = false;
  }

  if(back_jumping)
  {
    ASSERT(&ActiveCharacter() == this);
    double rotation;
    static double speed_init = GameMode::GetInstance()->character.back_jump_strength *
       sin(GameMode::GetInstance()->character.back_jump_angle);

    Point2d speed = GetSpeedXY();
    rotation = M_PI * speed.y / speed_init;
    body->SetRotation(rotation);
  }
}

// Prepare a new turn
void Character::PrepareTurn()
{
  damage_stats->HandleMostDamage();
  lost_energy = 0;
  pause_bouge_dg = Time::GetInstance()->Read();
}

bool Character::CanMoveRL() const
{
  if (!IsImmobile() || IsFalling()) return false;
  return pause_bouge_dg < Time::GetInstance()->Read();
}

void Character::BeginMovementRL(uint pause)
{
  walking_time = Time::GetInstance()->Read();
  do_nothing_time = Time::GetInstance()->Read();
  SetMovement("walk");
  CharacterCursor::GetInstance()->Hide();
  step_sound_played = true;
  pause_bouge_dg = Time::GetInstance()->Read()+pause;
}

bool Character::CanStillMoveRL(uint pause)
{
  if(pause_bouge_dg+pause<Time::GetInstance()->Read())
  {
    walking_time = Time::GetInstance()->Read();
    pause_bouge_dg += pause;
    return true;
  }
  return false;
}

// Signal the end of a fall
void Character::SignalCollision()
{
  // Do not manage dead characters.
  if (IsDead()) return;

  pause_bouge_dg = Time::GetInstance()->Read();
  double norm, degat;
  Point2d speed_vector;
  GameMode * game_mode = GameMode::GetInstance();
  if(body->GetClothe() != "weapon-" + m_team.GetWeapon().GetID()
  && body->GetClothe() != "jetpack"
  && body->GetClothe() != "jetpack-fire"
  && body->GetClothe() != "helmet")
    SetClothe("normal");

  if(body->IsWalking())
    SetMovement("walk");
  else
    SetMovement("breathe");

  SetMovementOnce("soft-land");

  body->SetRotation(0.0);
  back_jumping = false;

  speed_vector= GetSpeedXY();
  norm = speed_vector.Norm();
  if (norm > game_mode->safe_fall && speed_vector.y>0.0)
  {
    norm -= game_mode->safe_fall;
    degat = norm * game_mode->damage_per_fall_unit;
    SetEnergyDelta (-(int)degat);
    Game::GetInstance()->SignalCharacterDamage(this);
    SetClothe("normal");

    if(body->IsWalking())
      SetMovement("walk");
    else
      SetMovement("breathe");

    SetMovementOnce("hard-land");
  }
}

void Character::SignalExplosion()
{
  if(IsDead()) return;

  double n, a;
  GetSpeed(n, a);
  SetRebounding(true);
  if(n > MIN_SPEED_TO_FLY)
  {
    SetClothe("normal");
    SetMovement("fly");
  }
  else
  {
    SetClotheOnce("black");
    SetMovementOnce("black");
    if(body->GetClothe() == "black"
    && body->GetMovement() != "black")
      std::cerr << "Error: the clothe \"black\" of the character " << GetName() << " is set, but the skin have no \"black\" movement !!!" << std::endl;
  }
  // bug #7056 : When we are hit by an explosion while using grapple, we broke the rope.
  if (IsActiveCharacter()) {
    ActiveTeam().AccessWeapon().Deselect();
    // Select the weapon back. If not, we cannot move the crosshair.
    ActiveTeam().AccessWeapon().Select();
    // End of turn
    Game::GetInstance()->SignalCharacterDamage(this);
  }
}

BodyDirection_t Character::GetDirection() const
{
  return body->GetDirection();
}

// End of turn or change of character
void Character::StopPlaying()
{
  if(IsDead()) return;
  SetClothe("normal");
  SetMovement("breathe");
  body->ResetWalk();
  SetRebounding(true);
}

// Begining of turn or changed to this character
void Character::StartPlaying()
{
  ASSERT (!IsGhost());
  SetWeaponClothe();
  ActiveTeam().crosshair.Draw();
 // SetRebounding(false);
  ShowGameInterface();
  m_team.crosshair.Refresh(GetFiringAngle());
}

bool Character::IsActiveCharacter() const
{
  return this == &ActiveCharacter();
}

// Hand position
const Point2i & Character::GetHandPosition() const {
  return body->GetHandPosition();
}

double Character::GetFiringAngle() const {
  if (GetDirection() == DIRECTION_LEFT)
    return InverseAngleRad(firing_angle);
  return firing_angle;
}

void Character::SetFiringAngle(double angle) {
  /*while(angle > 2 * M_PI)
    angle -= 2 * M_PI;
  while(angle <= -2 * M_PI)
    angle += 2 * M_PI;*/
  angle = InRange_Double(angle, -(ActiveTeam().GetWeapon().GetMaxAngle()),
                             -(ActiveTeam().GetWeapon().GetMinAngle()));
  firing_angle = angle;
  m_team.crosshair.Refresh(GetFiringAngle());
}

void Character::SetWeaponClothe()
{
  SetClothe("weapon-" + m_team.GetWeapon().GetID());
  if(body->GetClothe() != "weapon-" + m_team.GetWeapon().GetID())
    SetClothe("normal");
  SetMovement("breathe");
}

void Character::SetMovement(const std::string& name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetMovement : %s",character_name.c_str(),name.c_str());
  body->SetMovement(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetMovementOnce(const std::string& name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetMovementOnce : %s",character_name.c_str(),name.c_str());
  body->SetMovementOnce(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetClothe(const std::string& name)
{
  if(IsDead() && name!="dead") return;
  MSG_DEBUG("body","Character %s -> SetClothe : %s",character_name.c_str(),name.c_str());
  body->SetClothe(name);
}

void Character::SetClotheOnce(const std::string& name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetClotheOnce : %s",character_name.c_str(),name.c_str());
  body->SetClotheOnce(name);
}

uint Character::GetTeamIndex() const
{
  uint index = 0;
  GetTeamsList().FindPlayingById( GetTeam().GetId(), index);
  return index;
}

uint Character::GetCharacterIndex() const
{
  uint index = 0;
  for(Team::iterator it = m_team.begin();
                     it != m_team.end() ; ++it, ++index )
  {
    if( &(*it) == this)
      return index;
  }
  ASSERT(false);
  return 0;
}

// ###################################################################
// ###################################################################
// ###################################################################

// #################### MOVE_RIGHT
void Character::HandleKeyPressed_MoveRight(bool shift)
{
  BeginMovementRL(PAUSE_MOVEMENT);
  body->StartWalk();

  HandleKeyRefreshed_MoveRight(shift);
}

void Character::HandleKeyRefreshed_MoveRight(bool shift) const
{
  HideGameInterface();

  if (ActiveCharacter().IsImmobile())
    MoveActiveCharacterRight(shift);
}

void Character::HandleKeyReleased_MoveRight(bool)
{
  body->StopWalk();
  SendActiveCharacterInfo();
}

// #################### MOVE_LEFT
void Character::HandleKeyPressed_MoveLeft(bool shift)
{
  BeginMovementRL(PAUSE_MOVEMENT);
  body->StartWalk();

  HandleKeyRefreshed_MoveLeft(shift);
}

void Character::HandleKeyRefreshed_MoveLeft(bool shift) const
{
  HideGameInterface();

  if (ActiveCharacter().IsImmobile())
    MoveActiveCharacterLeft(shift);
}

void Character::HandleKeyReleased_MoveLeft(bool)
{
  body->StopWalk();
  SendActiveCharacterInfo();
}

// #################### UP
void Character::HandleKeyRefreshed_Up(bool shift)
{
  HideGameInterface();
  if (ActiveCharacter().IsImmobile())
    {
      if (ActiveTeam().crosshair.enable)
        {
          do_nothing_time = Time::GetInstance()->Read();
          CharacterCursor::GetInstance()->Hide();
          if (shift) AddFiringAngle(-DELTA_CROSSHAIR/10.0);
          else       AddFiringAngle(-DELTA_CROSSHAIR);
          SendActiveCharacterInfo();
        }
    }
}

// #################### DOWN
void Character::HandleKeyRefreshed_Down(bool shift)
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    {
      if (ActiveTeam().crosshair.enable)
        {
          do_nothing_time = Time::GetInstance()->Read();
          CharacterCursor::GetInstance()->Hide();
          if (shift) AddFiringAngle(DELTA_CROSSHAIR/10.0);
          else       AddFiringAngle(DELTA_CROSSHAIR);
          SendActiveCharacterInfo();
        }
    }
}

// #################### JUMP

void Character::HandleKeyPressed_Jump(bool) const
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    ActionHandler::GetInstance()->NewActionActiveCharacter(new Action(Action::ACTION_CHARACTER_JUMP));
}

// #################### HIGH JUMP
void Character::HandleKeyPressed_HighJump(bool) const
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    ActionHandler::GetInstance()->NewActionActiveCharacter(new Action(Action::ACTION_CHARACTER_HIGH_JUMP));
}

// #################### BACK JUMP
void Character::HandleKeyPressed_BackJump(bool) const
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    ActionHandler::GetInstance()->NewActionActiveCharacter(new Action(Action::ACTION_CHARACTER_BACK_JUMP));
}

