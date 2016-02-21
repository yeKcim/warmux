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
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>
#ifndef _WIN32
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/resource.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/time.h>
# include <sys/ioctl.h>
#endif
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <map>

#include <WARMUX_index_server.h>
#include <WARMUX_types.h>
#include <WARMUX_config.h>
#include <WARMUX_network.h>
#include <WARMUX_random.h>
#include <WARMUX_socket.h>
#include <WSERVER_config.h>
#include <WSERVER_clock.h>
#include <WSERVER_debug.h>
#include <WSERVER_env.h>
#include "config.h"
#include "server.h"

#ifdef WIN32
# undef main
#endif

BasicClock wx_clock;
std::string config_file = "warmux_server.conf";

void printUsage(char *argv[])
{
  printf("Usage: %s [OPTIONS]\n", argv[0]);
  printf("OPTIONS:\n"
	 "  -h|--help: print this help and exit\n"
	 "  -v|--version: print version and exit\n"
	 "  -d|--daemon: start as daemon (in background)\n"
	 "  -f|--file: specify config file\n"
	 "  -i|--index-server [ip/hostname of index server]]\n"
	 );
  printf("\nConfig file is reloaded when receiving a -HUP signal BUT some configuration options need program to be restarted\n");
}

void parseArgs(int argc, char *argv[])
{
  int opt;

  struct option long_options[] = {
    {"help",	     no_argument,       NULL, 'h'},
    {"version",	     no_argument,       NULL, 'v'},
    {"daemon",       no_argument,       NULL, 'd'},
    {"file",         required_argument, NULL, 'f'},
    {"index-server", optional_argument, NULL, 'i'},
    {NULL,           no_argument,       NULL,  0 }
  };

  while ((opt = getopt_long(argc, argv, "hvdf:i::", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      printUsage(argv);
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      printf("Warmux game server version %s\n", PACKAGE_VERSION);
      exit(EXIT_SUCCESS);
    case 'd':
      Env::Daemonize();
      break;
    case 'f':
      config_file = optarg;
      break;
    case 'i':
      {
	std::string index_server_address;
	if (optarg) index_server_address = optarg;
	else index_server_address = "127.0.0.1";
	printf("Using %s as address for index server. This option must be used only for debugging.\n",
	       index_server_address.c_str());
	IndexServer::GetInstance()->SetAddress(index_server_address.c_str());
      }
      break;

    case '?': /* returns by getopt if option was invalid */
      printUsage(argv);
      exit(EXIT_FAILURE);
      break;

    default:
      fprintf(stderr, "Sorry, it seems that option '-%c' is not implemented!\n", opt);
      ASSERT(false);
      exit(EXIT_FAILURE);
      break;
    }
  }
}

int main(int argc, char* argv[])
{
  DPRINT(INFO, "Warmux game server version %s", PACKAGE_VERSION);
  DPRINT(INFO, "%s", wx_clock.DateStr());

  parseArgs(argc, argv);

  config.Load(config_file);

  Env::SetConfigClass(config);
  Env::SetWorkingDir();
  Env::SetChroot();
  Env::MaskSigPipe();
  Env::SetMaxConnection();
  Env::SetupAutoReloadConf();

  int port = 0;
  if (!config.Get("port", port)) {
    DPRINT(INFO, "ERROR: No port specified");
    exit(EXIT_FAILURE);
  }

  int max_nb_games = 10;
  if (!config.Get("max_nb_games", max_nb_games)) {
    DPRINT(INFO, "ERROR: max_nb_games not specified");
    exit(EXIT_FAILURE);
  }

  int max_nb_clients = 50;
  if (!config.Get("max_nb_clients", max_nb_clients)) {
    DPRINT(INFO, "ERROR: max_nb_clients not specified");
    exit(EXIT_FAILURE);
  }

  std::string game_name;
  if (!config.Get("game_name", game_name)) {
    DPRINT(INFO, "ERROR: game name not specified");
    exit(EXIT_FAILURE);
  }

  std::string password = "";
  if (!config.Get("password", password)) {
    DPRINT(INFO, "WARNING: password not specified (no password used)");
  }

  // Specify if the game will be visible in the index server or not
  bool browsable = false;
  if (!config.Get("public", browsable)) {
    DPRINT(INFO, "WARNING: it is not specified if game is public or private (default: private)");
  }

  RandomLocal().InitRandom();

  if (!GameServer::GetInstance()->ServerStart(uint(port), uint(max_nb_games), uint(max_nb_clients),
					      game_name, password, browsable)) {
    DPRINT(INFO, "ERROR: Server not started");
    exit(EXIT_FAILURE);
  }

  GameServer::GetInstance()->RunLoop();
}
