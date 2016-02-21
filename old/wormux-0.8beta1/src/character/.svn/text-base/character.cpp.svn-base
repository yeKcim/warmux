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

#include "character.h"
#include <SDL.h>
#include <sstream>
#include <iostream>
#include "body.h"
#include "team/macro.h"
#include "move.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "game/config.h"
#include "graphic/text.h"
#include "graphic/font.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "interface/interface.h"
#include "interface/cursor.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/water.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "sound/jukebox.h"
#include "tool/debug.h"
#include "tool/random.h"
#include "tool/math_tools.h"
#include "weapon/suicide.h"
#include "weapon/crosshair.h"
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

Body * Character::GetBody() const
{
  return body;
}

/* FIXME This methode is really strange, all this should probably be done in
 * constructor of Body...*/
void Character::SetBody(Body* char_body)
{
  body = char_body;
  body->SetOwner(this);
  SetClothe("normal");
  SetMovement("walk");

  SetDirection(randomSync.GetBool() ? Body::DIRECTION_LEFT : Body::DIRECTION_RIGHT);
  body->SetFrame(0);
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
  damage_stats(*this),
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
  bubble_engine(500),
  previous_strength(0),
  body(NULL)
{
  SetCollisionModel(false, true, true);
  /* body stuff */
  assert(char_body);
  SetBody(char_body);

  ResetConstants();

  // Name Text object
  if (Config::GetInstance()->GetDisplayNameCharacter())
    name_text = new Text(character_name);
  else
    name_text = NULL;

  // Energy
  life_points = GameMode::GetInstance()->character.init_energy;
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
  damage_stats(acharacter.damage_stats, *this),
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
  bubble_engine(250),
  previous_strength(acharacter.previous_strength),
  body(NULL)
{
  if (acharacter.body)
    SetBody(new Body(*acharacter.body));
  if(acharacter.name_text)
    name_text = new Text(*acharacter.name_text);
}

Character::~Character()
{
  MSG_DEBUG("character", "Unload character %s", character_name.c_str());
  if(body)
    delete body;
  if(name_text)
    delete name_text;
  body      = NULL;
  name_text = NULL;
}

void Character::SignalDrowning()
{
  SetEnergy(0);
  SetMovement("drowned");

  jukebox.Play (GetTeam().GetSoundProfile(),"sink");
  GameLoop::GetInstance()->SignalCharacterDeath (this);
}

// Si un ver devient un fantome, il meurt ! Signale sa mort
void Character::SignalGhostState (bool was_dead)
{
  // Report to damage performer this character lost all of its energy
  ActiveCharacter().damage_stats.MadeDamage(GetEnergy(), *this);

  MSG_DEBUG("character", "ghost");

  // Signal the death
  if (!was_dead) GameLoop::GetInstance()->SignalCharacterDeath (this);
}

void Character::SetDirection (Body::Direction_t nv_direction)
{
  body->SetDirection(nv_direction);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
  m_team.crosshair.Refresh(GetFiringAngle());
}

void Character::DrawEnergyBar(int dy)
{
  if( IsDead() )
	return;

  energy_bar.DrawXY( Point2i( GetCenterX() - energy_bar.GetWidth() / 2, GetY() + dy)
		  - camera.GetPosition() );
}

void Character::DrawName (int dy) const
{
  if(IsDead()) return;

  const int x =  GetCenterX();
  const int y = GetY()+dy;

  if (Config::GetInstance()->GetDisplayNameCharacter())
  {
    name_text->DrawCenterTopOnMap(x,y);
  }
}

const DamageStatistics& Character::GetDamageStats() const
{
  return damage_stats;
}

void Character::ResetDamageStats() 
{
  damage_stats.ResetDamage();
}

void Character::SetEnergyDelta (int delta, bool do_report)
{
  // If already dead, do nothing
  if (IsDead()) return;

  // Report damage to damage performer
  if (do_report)
    ActiveCharacter().damage_stats.MadeDamage(-delta, *this);

  uint saved_life_points = GetEnergy();

  // Update energy
  SetEnergy(GetEnergy() + delta);

  if(IsDead()) return;

  // Compute energy lost
  if (delta < 0) {
    lost_energy += (int)GetEnergy() - (int)saved_life_points;

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
      SetMovement("walk");
    }
  }

  //assert( m_alive != DEAD );
  if(IsDead()) return;

  // Change energy
  long energy = BorneLong((int)new_energy, 0,
                          GameMode::GetInstance()->character.max_energy);
  life_points = energy;
  energy_bar.Actu (energy);

  // Dead character ?
  if (GetEnergy() <= 0) Die();
}

bool Character::GotInjured() const
{
  return lost_energy < 0;
}

void Character::DisableDeathExplosion()
{
  death_explosion = false;
}

void Character::Die()
{
  assert (m_alive == ALIVE || m_alive == DROWNED);

  MSG_DEBUG("character", "Dying");

  if (m_alive != DROWNED)
  {
    m_alive = DEAD;

    SetEnergy(0);

    jukebox.Play(GetTeam().GetSoundProfile(),"death");
    SetClothe("dead");
    SetMovement("dead");

    if(death_explosion)
      ApplyExplosion(GetCenter(), GameMode::GetInstance()->death_explosion_cfg);
    assert(IsDead());

    // Signal the death
    GameLoop::GetInstance()->SignalCharacterDeath (this);
  }
}

bool Character::IsDiseased() const
{
  return disease_duration > 0 && !IsDead();
}

void Character::SetDiseaseDamage(const uint damage_per_turn, const uint duration)
{
  disease_damage_per_turn = damage_per_turn;
  disease_duration = duration;
}

// Keep almost 1 in energy
uint Character::GetDiseaseDamage() const
{
  if (disease_damage_per_turn < static_cast<uint>(GetEnergy()))
    return disease_damage_per_turn;
  return GetEnergy() - 1;
}

uint Character::GetDiseaseDuration() const
{
  return disease_duration;
}

void Character::DecDiseaseDuration()
{
  if (disease_duration > 0) disease_duration--;
  else disease_damage_per_turn = 0;
}

alive_t Character::GetLifeState() const
{
  return m_alive;
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
  if (hidden) return;

  // Gone in another world ?
  if (IsGhost()) return;

  // Character is visible on carema? If not, just leave the function
  Rectanglei rect(GetPosition(), Vector2<int>(GetWidth(), GetHeight()));
  if (!rect.Intersect(camera)) return;

  bool dessine_perte = (lost_energy != 0);
  if ((&ActiveCharacter() == this
    && GameLoop::GetInstance()->ReadState() != GameLoop::END_TURN)
      //&& (game_loop.ReadState() != jeuANIM_FIN_TOUR)
    || IsDead()
     )
    dessine_perte = false;

  if(GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN && body->IsWalking())
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
    SetMovement("walk");
  }

  // Stop flying if we don't go fast enough
  double n, a;
  GetSpeed(n, a);
  if(body->GetMovement() == "fly" && n < MIN_SPEED_TO_FLY)
    SetMovement("walk");


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
  int dy = -ESPACE;
  bool est_ver_actif = (this == &ActiveCharacter());
  Config * config = Config::GetInstance();
  bool display_energy = config->GetDisplayEnergyCharacter();
  display_energy &= !est_ver_actif || (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING);
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

  // Draw lost energy
  if (dessine_perte)
  {
    std::ostringstream ss;
    ss << lost_energy;
    dy -= HAUT_FONT_MIX;
    (*Font::GetInstance(Font::FONT_SMALL)).WriteCenterTop (
			GetPosition() - camera.GetPosition() + Point2i( GetWidth()/2, dy),
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
  if (GetDirection() == Body::DIRECTION_LEFT) angle = InverseAngle(angle);
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
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot");
  if(body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot")
  {
    // If a movement is defined for this weapon, just shoot
    DoShoot();
  }
  else
    prepare_shoot = true;
}

bool Character::IsPreparingShoot()
{
  return prepare_shoot;
}

void Character::DoShoot()
{
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-end-shoot");
  body->Build(); // Refresh the body
  ActiveTeam().AccessWeapon().Shoot();
}

void Character::Refresh()
{
  if (IsGhost()) return;

  UpdatePosition ();
  Time * global_time = Time::GetInstance();

  if(IsDiseased())
  {
    Point2i bubble_pos = GetPosition();
    if(GetDirection() == Body::DIRECTION_LEFT)
      bubble_pos.x += GetWidth();
    bubble_engine.AddPeriodic(bubble_pos, particle_ILL_BUBBLE, false,
                              - M_PI_2 - (float)GetDirection() * M_PI_4, 20.0);
  }

  if( IsActiveCharacter() && GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING)
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
      body->SetMovement("walk");
  }

  if(body->IsWalking())
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
    assert(&ActiveCharacter() == this);
    double rotation;
    static double speed_init = GameMode::GetInstance()->character.back_jump_strength *
       sin(GameMode::GetInstance()->character.back_jump_angle);

    Point2d speed;
    GetSpeedXY(speed);
    rotation = M_PI * speed.y / speed_init;
    body->SetRotation(rotation);
  }
}

// Prepare a new turn
void Character::PrepareTurn()
{
  damage_stats.HandleMostDamage();
  lost_energy = 0;
  pause_bouge_dg = Time::GetInstance()->Read();
}

const Team &Character::GetTeam() const
{
  return m_team;
}

bool Character::CanMoveRL() const
{
  if (!IsImmobile() || IsFalling()) return false;
  return pause_bouge_dg < Time::GetInstance()->Read();
}

bool Character::CanJump() const
{
  return CanMoveRL();
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
  back_jumping = false;
  double norme, degat;
  Point2d speed_vector;
  GameMode * game_mode = GameMode::GetInstance();
  SetMovement("walk");
  SetMovementOnce("soft-land");

  GetSpeedXY (speed_vector);
  norme = speed_vector.Norm();
  if (norme > game_mode->safe_fall && speed_vector.y>0.0)
  {
    norme -= game_mode->safe_fall;
    degat = norme * game_mode->damage_per_fall_unit;
    SetEnergyDelta (-(int)degat);
    GameLoop::GetInstance()->SignalCharacterDamage(this);
    SetMovement("walk");
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
    SetMovement("fly");
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
    GameLoop::GetInstance()->SignalCharacterDamage(this);
  }
}

Body::Direction_t Character::GetDirection() const
{
  return body->GetDirection();
}

// End of turn or change of character
void Character::StopPlaying()
{
  if(IsDead()) return;
  SetClothe("normal");
  SetMovement("walk");
  body->ResetWalk();
  SetRebounding(true);
}

// Begining of turn or changed to this character
void Character::StartPlaying()
{
  assert (!IsGhost());
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
  if (GetDirection() == Body::DIRECTION_LEFT)
    return InverseAngleRad(firing_angle);
  return firing_angle;
}

double Character::GetAbsFiringAngle() const {
  return firing_angle;
}

void Character::SetFiringAngle(double angle) {
  /*while(angle > 2 * M_PI)
    angle -= 2 * M_PI;
  while(angle <= -2 * M_PI)
    angle += 2 * M_PI;*/
  angle = BorneDouble(angle, -(ActiveTeam().GetWeapon().GetMaxAngle()),
                             -(ActiveTeam().GetWeapon().GetMinAngle()));
  firing_angle = angle;
  m_team.crosshair.Refresh(GetFiringAngle());
}

void Character::AddFiringAngle(double angle) {
  SetFiringAngle(firing_angle + angle);
}

void Character::Hide() { hidden = true; }
void Character::Show() { hidden = false; }

void Character::SetWeaponClothe()
{
  SetClothe("weapon-" + m_team.GetWeapon().GetID());
  if(body->GetClothe() != "weapon-" + m_team.GetWeapon().GetID())
    SetClothe("normal");
  SetMovement("walk");
}

void Character::SetMovement(std::string name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetMovement : %s",character_name.c_str(),name.c_str());
  body->SetMovement(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetMovementOnce(std::string name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetMovementOnce : %s",character_name.c_str(),name.c_str());
  body->SetMovementOnce(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetClothe(std::string name)
{
  if(IsDead() && name!="dead") return;
  MSG_DEBUG("body","Character %s -> SetClothe : %s",character_name.c_str(),name.c_str());
  body->SetClothe(name);
}

void Character::SetClotheOnce(std::string name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetClotheOnce : %s",character_name.c_str(),name.c_str());
  body->SetClotheOnce(name);
}

uint Character::GetTeamIndex()
{
  uint index = 0;
  teams_list.FindPlayingById( GetTeam().GetId(), index);
  return index;
}

uint Character::GetCharacterIndex()
{
  uint index = 0;
  for(Team::iterator it = m_team.begin();
                     it != m_team.end() ; ++it, ++index )
  {
    if( &(*it) == this)
      return index;
  }
  assert(false);
  return 0;
}

// ###################################################################
// ###################################################################
// ###################################################################

// #################### MOVE_RIGHT
void Character::HandleKeyPressed_MoveRight()
{
  BeginMovementRL(PAUSE_MOVEMENT);
  body->StartWalk();

  HandleKeyRefreshed_MoveRight();
}

void Character::HandleKeyRefreshed_MoveRight()
{
  HideGameInterface();

  if (ActiveCharacter().IsImmobile())
    MoveActiveCharacterRight();
}

void Character::HandleKeyReleased_MoveRight()
{
  body->StopWalk();
  SendActiveCharacterInfo();
}

// #################### MOVE_LEFT
void Character::HandleKeyPressed_MoveLeft()
{
  BeginMovementRL(PAUSE_MOVEMENT);
  body->StartWalk();

  HandleKeyRefreshed_MoveLeft();
}

void Character::HandleKeyRefreshed_MoveLeft()
{
  HideGameInterface();

  if (ActiveCharacter().IsImmobile())
    MoveActiveCharacterLeft();
}

void Character::HandleKeyReleased_MoveLeft()
{
  body->StopWalk();
  SendActiveCharacterInfo();
}

// #################### UP
void Character::HandleKeyPressed_Up()
{
  HandleKeyRefreshed_Up();
}

void Character::HandleKeyRefreshed_Up()
{
  HideGameInterface();
  if (ActiveCharacter().IsImmobile())
    {
      if (ActiveTeam().crosshair.enable)
	{
	  do_nothing_time = Time::GetInstance()->Read();
	  CharacterCursor::GetInstance()->Hide();
	  AddFiringAngle(-DELTA_CROSSHAIR);
	  SendActiveCharacterInfo();
	}
    }
}

void Character::HandleKeyReleased_Up(){}

// #################### DOWN
void Character::HandleKeyPressed_Down()
{
  HandleKeyRefreshed_Up();
}

void Character::HandleKeyRefreshed_Down()
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    {
      if (ActiveTeam().crosshair.enable)
	{
	  do_nothing_time = Time::GetInstance()->Read();
	  CharacterCursor::GetInstance()->Hide();
	  AddFiringAngle(DELTA_CROSSHAIR);
	  SendActiveCharacterInfo();
	}
    }
}

void Character::HandleKeyReleased_Down(){}

// #################### JUMP

void Character::HandleKeyPressed_Jump()
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    ActionHandler::GetInstance()->NewActionActiveCharacter(new Action(Action::ACTION_CHARACTER_JUMP));
}

void Character::HandleKeyRefreshed_Jump(){}

void Character::HandleKeyReleased_Jump(){}

// #################### HIGH JUMP
void Character::HandleKeyPressed_HighJump()
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    ActionHandler::GetInstance()->NewActionActiveCharacter(new Action(Action::ACTION_CHARACTER_HIGH_JUMP));
}

void Character::HandleKeyRefreshed_HighJump(){}
void Character::HandleKeyReleased_HighJump(){}

// #################### BACK JUMP
void Character::HandleKeyPressed_BackJump()
{
  HideGameInterface();
  if(ActiveCharacter().IsImmobile())
    ActionHandler::GetInstance()->NewActionActiveCharacter(new Action(Action::ACTION_CHARACTER_BACK_JUMP));
}

void Character::HandleKeyRefreshed_BackJump(){}
void Character::HandleKeyReleased_BackJump(){}
