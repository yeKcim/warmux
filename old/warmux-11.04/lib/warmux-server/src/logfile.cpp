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
#include <stdio.h>
#include <time.h>

#include <WSERVER_config.h>
#include <WSERVER_debug.h>
#include <WSERVER_logfile.h>

LogFile::LogFile(const std::string &suffix) : suffix_filename(suffix), fd(NULL)
{
}

LogFile::~LogFile()
{
  CloseFile();
}

void LogFile::OpenFile()
{
  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  char time_str[1024];
  snprintf(time_str, 1024, "%4i-%02i-%02i_",
           1900 + t->tm_year, 1 + t->tm_mon,t->tm_mday);

  std::string full_name = std::string(time_str) + '_' + suffix_filename;

  DPRINT(INFO, "Opening logfile : %s", full_name.c_str());

  FILE *newfd = fopen(full_name.c_str(), "a+");

  if (!newfd)
    PRINT_FATAL_ERROR;

  if (fd) {
    DPRINT(INFO, "Closing previous logfile");
    fclose(fd);
  }

  fd = newfd;
  AtOpen();
  fflush(fd);
}

void LogFile::CloseFile()
{
  AtClose();

  if (fd)
    fclose(fd);
  fd = NULL;
}

void LogFile::Rotate()
{
  CloseFile();
  OpenFile();
}
