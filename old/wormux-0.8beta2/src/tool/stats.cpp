/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Statistics.
 *****************************************************************************/

#include "stats.h"
#ifdef ENABLE_STATS
#include <SDL.h>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

typedef uint StatTime_t;

  class StatOutputItem
  {
  public:
    StatOutputItem(const std::string &p_function)
      : function(p_function), total(0), min(0), max(0)
    {}
    bool operator< (const StatOutputItem& b) const
    {
      return total > b.total;
    }
    std::string function;
    uint count;
    StatTime_t total;
    StatTime_t min;
    StatTime_t max;
  };


StatTime_t StatGetTimer()
{
  return SDL_GetTicks();
}

class StatItem
{
public:
  std::vector<StatTime_t> time;
  typedef std::vector<StatTime_t>::const_iterator time_it;
  uint count;
  StatTime_t last_time;
  StatItem() : count(0) { last_time = StatGetTimer(); }
};

std::map<std::string, StatItem> stats;
typedef std::map<std::string, StatItem>::iterator stats_it;

void StatStart(const std::string &function)
{
  stats_it it = stats.find(function);
  StatItem &item = (it != stats.end()) ? it->second : stats[function];
  item.count++;
  item.last_time = StatGetTimer();
}

void StatStop(const std::string &function)
{
  stats_it it = stats.find(function);
  if (it == stats.end()) return;
  it->second.time.push_back(StatGetTimer() - it->second.last_time);
}

StatTime_t ComputeStat(std::list<StatOutputItem> &table)
{
  std::list<StatOutputItem>::iterator table_it;
  stats_it it = stats.begin(), end=stats.end();
  StatTime_t total_time = 0;
  table.clear();
  for (; it != end; ++it)
  {
    StatOutputItem item(it->first);
    item.count = it->second.time.size();
    StatItem::time_it time = it->second.time.begin(), time_end=it->second.time.end();
    if (time != time_end)
    {
      item.min = *time;
      item.max = *time;
      for (; time != time_end; ++time)
      {
        item.total += *time;
        item.min = std::min(item.min, *time);
        item.max = std::max(item.max, *time);
      }
    }
    table.push_back(item);
    total_time += item.total;
  }
  if (total_time == 0) total_time = 1;
  return total_time;
}

std::string str2xml(const std::string &str)
{
// TODO
//    str.replace("&", "&amp;");
//    str.replace("<", "&lt;");
//    str.replace(">", "&gt;");
    return str;
}

void DoSaveStatToXML(const std::string &filename,
    std::list<StatOutputItem> &table,
    StatTime_t total_time)
{
  std::basic_ofstream<char> file(filename.c_str(), std::ios_base::out);
  if (!file)
  {
    std::cerr << "Can not create/open file \"" << filename << "\" to store statistics." << std::endl;
  }

  std::list<StatOutputItem>::const_iterator it=table.begin(), end=table.end();
  file << "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n";
  file << "<stats total_time=\"" << total_time << "\">\n";
  file.setf(std::ios_base::fixed);
  file.precision(2);
  for (; it != end; ++it)
  {
    file
      << "  <item function=\"" << it->function
      << "\" count=\"" << it->count
      << "\" total=\"" << it->total
      << "\" min=\"" << it->min
      << "\" max=\"" << it->max
      << "\" />\n";
  }
  file << "</stats>\n";
  file.close();
}

void SaveStatToXML(const std::string &filename)
{
  std::list<StatOutputItem> table;
  StatTime_t total_time = ComputeStat(table);
  table.sort();
  DoSaveStatToXML(filename, table, total_time);
}

#endif // ENABLE_STATS
