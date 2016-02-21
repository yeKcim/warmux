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
 * Contains AICommand classes which can be used to let the AI send actions.
 *****************************************************************************/

#ifndef AI_COMMAND_H
#define AI_COMMAND_H

#include <WORMUX_types.h>
#include <queue>
#include "weapon/weapon.h"

class AICommand
{
  public:
    /**
     * Tries to execute the command or tries to continue the execution of the command.
     * If the command requires an increment in game time then this method returns false.
     * In such a case this method should be called again when the game time has been increased.
     * A return value of true indicates that the command has been executed and that the object should not be used further.
     */
    virtual bool Execute() = 0;
    virtual ~AICommand() {};
};

class DoNothingForeverCommand : public AICommand
{
  public:
    virtual bool Execute() { return false; };
};

/**
 * Increases the weapon angle until it's higher or equal then a specifc value.
 * Does nothing if the specified angle is larger then the current angle.
 */
class IncreaseAngleCommand : public AICommand
{
  private:
    const double target_angle;
    const bool slowly;
    bool is_increasing;
  public:
    IncreaseAngleCommand(double target_angle, bool slowly);
    virtual bool Execute();
};


/**
 * Decreases the weapon angle until it's higher or equal then a specifc value.
 * Does nothing if the specified angle is smaller then the current angle.
 */
class DecreaseAngleCommand : public AICommand
{
  private:
    const double target_angle;
    const bool slowly;
    bool is_decreasing;
  public:
    DecreaseAngleCommand(double target_angle, bool slowly);
    virtual bool Execute();
};

class CommandList : public AICommand
{
  private:
    std::queue<AICommand *> commands;
  public:
    CommandList() {};
    virtual ~CommandList();
    virtual bool Execute();
    void Add(AICommand * command);
    int Size();
};

class SetWeaponAngleCommand : public AICommand
{
  private:
    CommandList commands;
    const double target_angle;
  public:
    SetWeaponAngleCommand(double angle);
    virtual bool Execute();
};

class StartShootingCommand : public AICommand
{
  public:
    virtual bool Execute();
};

class StopShootingCommand : public AICommand
{
  public:
    virtual bool Execute();
};

class StartMovingCommand : public AICommand
{
  private:
    LRDirection direction;
    bool slowly;
  public:
    StartMovingCommand(LRDirection direction, bool slowly);
    virtual bool Execute();
};

class StopMovingCommand : public AICommand
{
  private:
    LRDirection direction;
    bool slowly;
  public:
    StopMovingCommand(LRDirection direction, bool slowly);
    virtual bool Execute();
};

class SetDirectionCommand : public AICommand
{
  private:
    LRDirection direction;
    bool turning;
    CommandList * commands;
  public:
    SetDirectionCommand(LRDirection direction);
    virtual ~SetDirectionCommand();
    virtual bool Execute();
};

/*unused?
class LoadAndShootCommand : public AICommand
{
  private:
    const double target_strength;
    bool is_shooting;
  public:
    LoadAndShootCommand(double strength);
    virtual bool Execute();
};*/

class DoNothingCommand : public AICommand
{
  private:
    const uint duration;
    uint start_time;
  public:
    DoNothingCommand(const uint duration_in_ms);
    virtual bool Execute();
};

class SelectWeaponCommand : public AICommand
{
  private:
    Weapon::Weapon_type weapon;
  public:
    SelectWeaponCommand(Weapon::Weapon_type weapon);
    virtual bool Execute();
};

class SelectCharacterCommand : public AICommand
{
  private:
    Character * character;
  public:
    SelectCharacterCommand(Character * character);
    virtual bool Execute();
};

class WaitForStrengthCommand : public AICommand
{
  private:
    double target_strength;
  public:
    WaitForStrengthCommand(double target_strength);
    virtual bool Execute();
};

class SetTimeoutCommand : public AICommand
{
  private:
    int timeout;
  public:
    SetTimeoutCommand(int timeout);
    virtual bool Execute();
};

#endif
