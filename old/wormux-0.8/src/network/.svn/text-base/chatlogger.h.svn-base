#ifndef CHATLOGGER_H
#define CHATLOGGER_H

#include <iostream>
#include <fstream>
#include "include/singleton.h"

class ChatLogger: public Singleton<ChatLogger>
{
 public:
  ChatLogger();
  ~ChatLogger();
  void LogMessage(const std::string& msg);
  static void LogMessageIfOpen(const std::string& msg);
  static void CloseIfOpen(void);
 protected:
  friend class Singleton<ChatLogger>;

  std::string logdir;
  std::string logfile;
  std::ofstream m_logfilename;
};

#endif //CHATLOGGER_H
