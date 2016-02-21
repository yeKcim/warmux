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
 * Contains AICommand implementations which can be used to let the AI send actions.
 *****************************************************************************/

#include <WORMUX_action.h>
#include "ai/ai_command.h"
#include "character/character.h"
#include "game/time.h"
#include "include/action_handler.h"
#include "team/team.h"
#include "team/teams_list.h"

IncreaseAngleCommand::IncreaseAngleCommand(double target_angle, bool slowly):
  target_angle(target_angle),
  slowly(slowly),
  is_increasing(false)
{
  // Do nothing.
}

bool IncreaseAngleCommand::Execute()
{
  double current_angle = ActiveCharacter().GetAbsFiringAngle();
  if (current_angle >= target_angle) {
    if (is_increasing) {
      Action *a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_DOWN);
      a->Push(slowly);
      ActionHandler::GetInstance()->NewAction(a);
    }
    return true;
  } else {
    if (!is_increasing) {
      Action *a = new Action(Action::ACTION_CHARACTER_START_MOVING_DOWN);
      a->Push(slowly);
      ActionHandler::GetInstance()->NewAction(a);
      is_increasing = true;
    }
    return false;
  }
}

DecreaseAngleCommand::DecreaseAngleCommand(double target_angle, bool slowly):
  target_angle(target_angle),
  slowly(slowly),
  is_decreasing(false)
{
  // Do nothing.
}


bool DecreaseAngleCommand::Execute()
{
  double current_angle = ActiveCharacter().GetAbsFiringAngle();
  if (current_angle <= target_angle) {
    if (is_decreasing) {
      Action *a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_UP);
      a->Push(slowly);
      ActionHandler::GetInstance()->NewAction(a);
    }
    return true;
  } else {
    if (!is_decreasing) {
      Action *a = new Action(Action::ACTION_CHARACTER_START_MOVING_UP);
      a->Push(slowly);
      ActionHandler::GetInstance()->NewAction(a);
      is_decreasing = true;
    }
    return false;
  }
}

CommandList::~CommandList() {
  while (commands.size() > 0) {
    AICommand * command = commands.front();
    commands.pop();
    delete command;
  }
}

void CommandList::Add(AICommand * command)
{
  commands.push(command);
}

bool CommandList::Execute()
{
  while (commands.size() > 0) {
    AICommand * command = commands.front();
    if (command->Execute()) {
      commands.pop();
      delete command;
    } else {
      return false;
    }
  }
  return true;
}

int CommandList::Size()
{
  return commands.size();
}

SetWeaponAngleCommand::SetWeaponAngleCommand(double angle):target_angle(angle)
{
  // do nothing
}

bool SetWeaponAngleCommand::Execute()
{
  bool first_call = commands.Size() == 0;
  if (first_call) {
    double current_angle = ActiveCharacter().GetAbsFiringAngle();
    if (current_angle > target_angle) {
      commands.Add(new DecreaseAngleCommand(target_angle, false));
      commands.Add(new IncreaseAngleCommand(target_angle, true));
    } else if (current_angle < target_angle) {
      commands.Add(new IncreaseAngleCommand(target_angle, false));
      commands.Add(new DecreaseAngleCommand(target_angle, true));
    } else {
      ASSERT(current_angle == target_angle);
      return true;
    }
  }
  return commands.Execute();
}

bool StartShootingCommand::Execute()
{
  Action *a = new Action(Action::ACTION_WEAPON_START_SHOOTING);
  ActionHandler::GetInstance()->NewAction(a);
  return true;
}

bool StopShootingCommand::Execute()
{
  Action *a = new Action(Action::ACTION_WEAPON_STOP_SHOOTING);
  ActionHandler::GetInstance()->NewAction(a);
  return true;
}

StartMovingCommand::StartMovingCommand(LRDirection direction, bool slowly):
  direction(direction),
  slowly(slowly)
{
  // do nothing
}

bool StartMovingCommand::Execute()
{
  Action *a;
  if (direction == DIRECTION_LEFT)
    a = new Action(Action::ACTION_CHARACTER_START_MOVING_LEFT);
  else
    a = new Action(Action::ACTION_CHARACTER_START_MOVING_RIGHT);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
  return true;
}

StopMovingCommand::StopMovingCommand(LRDirection direction, bool slowly):
  direction(direction),
  slowly(slowly)
{
  // do nothing
}

bool StopMovingCommand::Execute()
{
  Action *a;
  if (direction == DIRECTION_LEFT)
    a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_LEFT);
  else
    a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_RIGHT);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
  return true;
}

SetDirectionCommand::SetDirectionCommand(LRDirection direction):
  direction(direction),
  commands(NULL)
{
}

bool SetDirectionCommand::Execute()
{
  if (commands == NULL) {
    if (ActiveCharacter().GetDirection() == direction) {
      return true;
    } else {
      commands = new CommandList();
      commands->Add(new StartMovingCommand(direction, true));
      // Let one frame pass so that the movement get noticed.
      commands->Add(new DoNothingCommand(1));
      commands->Add(new StopMovingCommand(direction, true));
    }
  }
  return commands->Execute();
}

SetDirectionCommand::~SetDirectionCommand()
{
  if (commands)
    delete commands;
};

DoNothingCommand::DoNothingCommand(uint duration_in_ms):
  duration(duration_in_ms),
  start_time(0)
{
  // nothing else to do
}

bool DoNothingCommand::Execute()
{
  uint now = Time::GetInstance()->Read();
  if (!start_time) {
    start_time = now;
  }
  return (now >= start_time + duration);
}

SelectWeaponCommand::SelectWeaponCommand(Weapon::Weapon_type weapon):
  weapon(weapon)
{
  // do nothing
}

bool SelectWeaponCommand::Execute()
{
  if (weapon == ActiveTeam().GetWeapon().GetType())
    return true;
  Action * a = new Action(Action::ACTION_PLAYER_CHANGE_WEAPON, weapon);
  ActionHandler::GetInstance()->NewAction(a);
  return true;
}

SelectCharacterCommand::SelectCharacterCommand(Character * character):
  character(character)
{
  // do nothing
}

bool SelectCharacterCommand::Execute()
{
  if (character->IsActiveCharacter())
    return true;
  Action * action = new Action(Action::ACTION_PLAYER_CHANGE_CHARACTER);
  uint index = character->GetCharacterIndex();
  action->Push((int)index);
  ActionHandler::GetInstance()->NewAction(action);
  return true;
}

WaitForStrengthCommand::WaitForStrengthCommand(double target_strength):
target_strength(target_strength)
{
  // do nothing
}

bool WaitForStrengthCommand::Execute()
{
  double current_strength = ActiveTeam().GetWeapon().GetStrength();
  return (current_strength >= target_strength);
}

SetTimeoutCommand::SetTimeoutCommand(int timeout):
  timeout(timeout)
{
  // do nothing
}

bool SetTimeoutCommand::Execute()
{
  Action * a = new Action(Action::ACTION_WEAPON_SET_TIMEOUT, timeout);
  ActionHandler::GetInstance()->NewAction(a);
  return true;
}
