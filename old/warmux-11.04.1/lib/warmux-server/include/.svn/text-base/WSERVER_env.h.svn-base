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
 ******************************************************************************/

#ifndef WARMUX_SERVER_ENV_H
#define WARMUX_SERVER_ENV_H

#include <signal.h>
#ifdef _WIN32
typedef void siginfo_t;
#endif

class ServerConfig;

class Env
{
private:
  static ServerConfig *config;
  friend void reload_config(int, siginfo_t *, void *);

public:
  static void Daemonize();

  static void SetConfigClass(ServerConfig& config);

  static void SetChroot();
  static void SetWorkingDir();
  static void SetMaxConnection();
  static void MaskSigPipe();
  static void SetupAutoReloadConf();
};

#endif
