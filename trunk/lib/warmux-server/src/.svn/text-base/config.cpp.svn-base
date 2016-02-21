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

#include <string>
#include <fstream>
#include <WSERVER_config.h>
#include <WSERVER_debug.h>
#ifdef _WIN32
typedef int ssize_t;
#define realpath(str_, ptr) ((char*)str_)
#endif

bool WSERVER_Verbose = true;

static ssize_t getline(std::string& line, std::ifstream& file)
{
  line.clear();
  std::getline(file, line);
  if(file.eof())
    return -1;
  return (ssize_t)line.size();
}

ServerConfig::ServerConfig(bool versions) : support_versions(versions)
{
  config_file = ""; // to be set with Load()
}

void ServerConfig::Load(const std::string & _config_file)
{
  if (config_file != "") {
    DPRINTMSG(stderr,
		          "Config file %s is already loaded. %s will not be loaded",
		          config_file.c_str(), _config_file.c_str());
	  exit(EXIT_FAILURE);
  }

  // Get its full pathname to allow "automatic" reloading
  char *config_path = realpath((_config_file.c_str()), NULL);
  if (!config_path) {
    DPRINTMSG(stderr, "Unable to open config file %s: %s", _config_file.c_str(), strerror(errno));
    exit(EXIT_FAILURE);
  }

  config_file = config_path;
  free(config_path);

  LoadConfigFile();
}

void ServerConfig::LoadConfigFile()
{
  // Open the config file
  std::ifstream fin;
  fin.open(config_file.c_str(), std::ios::in);
  if (!fin) {
    DPRINTMSG(stderr, "Unable to open config file %s: %s", config_file.c_str(), strerror(errno));
    exit(EXIT_FAILURE);
  }

  Parse(fin);

  fin.close();

  DPRINT(INFO, "Config loaded successfully from %s", config_file.c_str());

  if (support_versions && supported_versions.empty()) {
    DPRINT(INFO, "No supported versions ?!? You must fill option 'versions'");
    exit(EXIT_FAILURE);
  }
}

void ServerConfig::Parse(std::ifstream & fin)
{
  ssize_t read;
  std::string line;
  int line_nbr = 0;

  while ((read = getline(line, fin)) >= 0) {
    line_nbr++;
    if(line.size() == 0 || line.at(0) == '#' )
      continue;

    std::string::size_type equ_pos = line.find('=',0);
    if (equ_pos == std::string::npos) {
      DPRINT(INFO, "Wrong format on line %i",line_nbr);
      continue;
    }

    std::string opt = line.substr(0, equ_pos);
    std::string val = line.substr(equ_pos+1);

    if (opt == "versions") {
      if (support_versions) {
        supported_versions.clear(); // useful only for reloading
        ServerConfig::SplitVersionsString(val, supported_versions);
      } else {
        fprintf(stderr, "Option 'versions' is ignored.\n");
      }
      continue;
    } else if (opt == "hidden_versions") {
      if (support_versions) {
        hidden_supported_versions.clear(); // useful only for reloading
        ServerConfig::SplitVersionsString(val, hidden_supported_versions);
      } else {
        fprintf(stderr, "Option 'hidden_versions' is ignored.\n");
      }
      continue;
    }

    if (opt == "verbose")
      if (val == "false")
        WSERVER_Verbose = false;

    // val is considered to be an int if it doesn't contain
    // a '.' (ip address have to be handled as string...
    if(val.find('.',0) == std::string::npos
       && ((val.at(0) >= '0' && val.at(0) <= '9')
           ||   val.at(0) == '-' )) {
      int nbr = atoi(val.c_str());
      int_value[ opt ] = nbr;
    } else {
      if(val == "true")
        bool_value[ opt ] = true;
      else
        if(val == "false")
          bool_value[ opt ] = false;
        else
          str_value[ opt ] = val;
    }
  }
}

void ServerConfig::Reload()
{
  LoadConfigFile();
}

void ServerConfig::Display() const
{
  DPRINT(INFO, "Current config:");
  for(std::map<std::string, bool>::const_iterator cfg = bool_value.begin();
      cfg != bool_value.end();
      ++cfg) {
    DPRINT(INFO, "(bool) %s = %s", cfg->first.c_str(), cfg->second?"true":"false");
  }

  for(std::map<std::string, int>::const_iterator cfg = int_value.begin();
      cfg != int_value.end();
      ++cfg) {
    DPRINT(INFO, "(int) %s = %i", cfg->first.c_str(), cfg->second);
  }

  for(std::map<std::string, std::string>::const_iterator cfg = str_value.begin();
      cfg != str_value.end();
      ++cfg) {
    DPRINT(INFO, "(str) %s = %s", cfg->first.c_str(), cfg->second.c_str());
  }

  if (support_versions) {
    DPRINT(INFO, "Supported versions: %s",
	         ServerConfig::SupportedVersions2Str(supported_versions).c_str());
    DPRINT(INFO, "Hidden but supported versions: %s",
	         ServerConfig::SupportedVersions2Str(hidden_supported_versions).c_str());
  }
}

bool ServerConfig::Get(const std::string & name, bool & value) const
{
  std::map<std::string, bool>::const_iterator it = bool_value.find(name);

  if (it == bool_value.end()) {
    DPRINT(INFO, "Unknown or unset boolean (true/false) config option: %s", name.c_str());
    return false;
  }

  value = it->second;
  return true;
}

bool ServerConfig::Get(const std::string & name, int & value) const
{
  std::map<std::string, int>::const_iterator it = int_value.find(name);

  if (it == int_value.end()) {
    DPRINT(INFO, "Unknown or unset integer config option: %s", name.c_str());
    return false;
  }

  value = it->second;
  return true;
}

bool ServerConfig::Get(const std::string & name, std::string & value) const
{
  std::map<std::string, std::string>::const_iterator it = str_value.find(name);

  if (it == str_value.end()) {
    DPRINT(INFO, "Unknown or unset string config option: %s", name.c_str());
    return false;
  }

  value = it->second;
  return true;
}

void ServerConfig::SetDefault(const std::string & name, const bool & value)
{
  bool val;
  if( ! Get(name, val) ) {
    DPRINT(INFO, "Setting to default value : %s", name.c_str());
    bool_value[ name ] = value;
  }
}

void ServerConfig::SetDefault(const std::string & name, const int & value)
{
  int val;
  if( ! Get(name, val) ) {
    DPRINT(INFO, "Setting to default value : %s", name.c_str());
    int_value[ name ] = value;
  }
}

void ServerConfig::SetDefault(const std::string & name, const std::string & value)
{
  std::string val;
  if( ! Get(name, val) ) {
    DPRINT(INFO, "Setting to default value : %s", value.c_str());
    str_value[ name ] = value;
  }
}


bool ServerConfig::IsVersionSupported(const std::string & version) const
{
  std::list<std::string>::const_iterator it;
  for (it = supported_versions.begin();
       it != supported_versions.end();
       it++) {
    if (version == *it)
      return true;
  }

  for (it = hidden_supported_versions.begin();
       it != hidden_supported_versions.end();
       it++) {
    if (version == *it)
      return true;
  }

  return false;
}

// returns only the officially supported versions
const std::string ServerConfig::SupportedVersions2Str() const
{
  return ServerConfig::SupportedVersions2Str(supported_versions);
}

// static method
const std::string ServerConfig::SupportedVersions2Str(const std::list<std::string>& versions_lst)
{
 std::string versions = "";
  std::list<std::string>::const_iterator it;
  for (it = versions_lst.begin();
       it != versions_lst.end();
       it++) {
    versions += *it + ',';
  }

  return versions.substr(0, versions.size()-1);
}

// static method
void ServerConfig::SplitVersionsString(const std::string& val, std::list<std::string>& versions_lst)
{
  // split the string on ','
  std::string::size_type prev_pos = 0;
  std::string::size_type comma_pos = 0;
  std::string version;

  do {
    comma_pos = val.find(',', prev_pos);

    if  (comma_pos != std::string::npos) {
      version = val.substr(prev_pos, comma_pos - prev_pos);
      prev_pos = comma_pos+1;
    } else {
      version = val.substr(prev_pos);
    }
    versions_lst.push_back(version);

  } while (comma_pos != std::string::npos);
}
