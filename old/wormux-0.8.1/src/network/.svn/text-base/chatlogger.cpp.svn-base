#include <cstring>
#include <iostream>
#include <fstream>
#include "include/base.h"
#include "tool/error.h"
#include "tool/i18n.h"
#ifdef DEBUG
#include "tool/random.h"
#endif
#include "network/chatlogger.h"
#include "game/config.h"
#include <time.h>

#define TIME_BASE_YEAR 1900

ChatLogger::ChatLogger() :
        logdir(Config::GetInstance()->GetChatLogDir()),
        logfile(logdir + "chat.log")
{
  // FIXME: also add the game name to the filename
  //
  //  currently, it seems this info doesn't get to the client,
  //  and probably the game_name should be stored after selecting
  //  the game
  time_t t;
  struct tm lt, *plt;
  std::string timestamp;

  if ( ((time_t) -1) == time(&t) )
  {
    timestamp = std::string (_("(unknown time)")) ;
  }
  else
  {
    // convert to local time
    plt = localtime(&t);
    memcpy(&lt, plt, sizeof(struct tm));

    timestamp = Format ( "%4d-%02d-%02d-%02dH%02dm%02d" ,
                         lt.tm_year + TIME_BASE_YEAR, lt.tm_mon+1, lt.tm_mday+1,
                         lt.tm_hour, lt.tm_min, lt.tm_sec ) ;

#ifndef DEBUG
    logfile = Format ( "%s.log" , timestamp.c_str() );
#else // DEBUG
    logfile = Format ( "%s-%c.log" , timestamp.c_str(), (char)(RandomLocal().GetInt(0,10)+'a') );
#endif // DEBUG
  }

  // TRANSLATORS: after this string there will be a time stamp or the string '(unknown time)'
  timestamp = std::string(_("New network game at ")) + timestamp ;

  std::string fn = logdir + logfile ;

  m_logfilename.open(fn.c_str(), std::ios::out | std::ios::app);
  if(m_logfilename.fail())
    Error(Format(_("Couldn't open file %s"), fn.c_str()));
  else
    this->LogMessage(timestamp);
}

ChatLogger::~ChatLogger()
{
  m_logfilename.close();
}

void ChatLogger::LogMessage(const std::string &msg)
{
  time_t t;
  struct tm lt, *plt;
  std::string timestamp;

  if (m_logfilename.fail())
    return;

  if ( ((time_t) -1) == time(&t) )
  {
    timestamp = std::string (_("(unknown time)")) ;
  }
  else
  {
    // convert to local time
    plt = localtime(&t);
    memcpy(&lt, plt, sizeof(struct tm));

    timestamp = Format( "(%02dH%02dm%02d) ", lt.tm_hour, lt.tm_min, lt.tm_sec );
  }

  m_logfilename << timestamp << msg << std::endl << std::flush;
}

void ChatLogger::LogMessageIfOpen(const std::string &msg)
{
  if ( singleton ) ChatLogger::GetInstance()->LogMessage(msg);
}

void ChatLogger::CloseIfOpen()
{
  if ( singleton ) ChatLogger::GetInstance()->CleanUp();
}
