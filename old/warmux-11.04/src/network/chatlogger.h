#ifndef CHATLOGGER_H
#define CHATLOGGER_H

#include <iostream>
#include <fstream>
#include <WARMUX_singleton.h>

class ChatLogger: public Singleton<ChatLogger>
{
  std::string logdir;
  std::string logfile;
  std::ofstream m_logfilename;
 protected:
  friend class Singleton<ChatLogger>;
 public:
  ChatLogger();
  ~ChatLogger() { m_logfilename.close(); }

  void LogMessage(const std::string& msg);
  static void LogMessageIfOpen(const std::string& msg) { if (singleton) singleton->LogMessage(msg); } 
  static void CloseIfOpen(void) { if (singleton) singleton->CleanUp(); }
};

#endif //CHATLOGGER_H
