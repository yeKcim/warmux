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
 ******************************************************************************/
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <WSERVER_env.h>
#include <WSERVER_config.h>
#include <WSERVER_debug.h>

static BasicConfig* config = NULL;

void Env::SetConfigClass(BasicConfig& _config)
{
  config = &_config;
}

void Env::SetChroot()
{
  bool r;

  bool chroot_opt;
  r = config->Get("chroot", chroot_opt);
  if (!r)
    PRINT_FATAL_ERROR;

  // Attempt chroot only when root
  if (chroot_opt && !getgid()) {

    // If root, do chroot
    if (chroot("./") == -1)
      PRINT_FATAL_ERROR;
    if (chdir("/") == -1)
      PRINT_FATAL_ERROR;

    int uid, gid;
    r = config->Get("chroot_uid", uid);
    if (!r)
      PRINT_FATAL_ERROR;

    r = config->Get("chroot_gid", gid);
    if (!r)
      PRINT_FATAL_ERROR;

    if (setgid(gid) == -1)
      PRINT_FATAL_ERROR;
    if (setuid(uid) == -1)
      PRINT_FATAL_ERROR;
  }


  // Check we are not root anymore
  if (getuid() == 0) {
    DPRINT(INFO, "Don't start me as root user!!");
    exit(EXIT_FAILURE);
  }
}

void Env::SetWorkingDir()
{
  bool r;
  std::string working_dir;
  r = config->Get("working_dir", working_dir);
  if (!r)
    PRINT_FATAL_ERROR;

  DPRINT(INFO, "Entering folder %s", working_dir.c_str());
  if (chdir(working_dir.c_str()) == -1) {
    DPRINTMSG(stderr, "ERROR: %s: %s", working_dir.c_str(), strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void Env::SetMaxConnection()
{
  bool r;

  // Set the maximum number of file descriptor allowed
  int max_conn;
  r = config->Get("connexion_max", max_conn);
  if (!r)
    PRINT_FATAL_ERROR;

  struct rlimit limit;

  if (getrlimit(RLIMIT_NOFILE, &limit) == -1)
    PRINT_FATAL_ERROR;

  if (max_conn == -2) {
    // Keep the system default
    DPRINT(INFO, "Number of connexions allowed : system default");

  } else if(max_conn == -1) {
    // Use the max allowed
    DPRINT(INFO, "Number of connexions allowed : maximum allowed");
    limit.rlim_cur = limit.rlim_max;

  } else {
    DPRINT(INFO, "Number of connexions allowed : user defined");
    limit.rlim_cur = max_conn;
    limit.rlim_max = max_conn;
  }

  if (setrlimit(RLIMIT_NOFILE, &limit) == -1)
    PRINT_FATAL_ERROR;

  DPRINT(INFO, "Number of connexions allowed : %i", max_conn);
}

static void signal_handler(int sig, siginfo_t */*si*/, void */*unused*/)
{
  DPRINT(INFO, "Signal received: %d", sig);
}

void Env::MaskSigPipe()
{
  // silently handle SIGPIPE... (not sure it is needed :-/)
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = signal_handler;
  if (sigaction(SIGPIPE, &sa, NULL) == -1)
    PRINT_FATAL_ERROR;
}
