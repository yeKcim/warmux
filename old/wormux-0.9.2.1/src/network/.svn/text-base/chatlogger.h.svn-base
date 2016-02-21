#ifndef CHATLOGGER_H
#define CHATLOGGER_H

#include <iostream>
#include <fstream>
#include <WORMUX_singleton.h>

class ChatLogger: public Singleton<ChatLogger>
{
  std::string logdir;
  std::string logfile;
  std::ofstream m_logfilename;
 protected:
  friend class Singleton<ChatLogger>;
 public:
  ChatLogger();
  ~ChatLogger();

  void LogMessage(const std::string& msg);
  static void LogMessageIfOpen(const std::string& msg);
  static void CloseIfOpen(void);
};

#endif //CHATLOGGER_H
