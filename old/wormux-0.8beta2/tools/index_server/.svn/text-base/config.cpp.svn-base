/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
#include "config.h"
#include "debug.h"

Config config;

Config::Config()
{
	Load();
	SetDefault("port", 9997);
	SetDefault("working_dir", "wormux_log/");
	SetDefault("chroot", true);
	SetDefault("chroot_gid", 500);
	SetDefault("chroot_uid", 500);
	SetDefault("connexion_max", -2);
	Display();
}

static ssize_t getline(std::string& line, std::ifstream& file)
{
        line.clear();
        std::getline(file, line);
	if(file.eof())
		return -1;
        return line.size();
}

void Config::Load()
{
	DPRINT(INFO, "Loading config file");

	int line_nbr = 0;

	// Parse the file
	std::ifstream fin;
	fin.open("wormux_server.conf", std::ios::in);
	if(!fin)
	{
		DPRINT(INFO, "Unable to open config file");
		return;
	}

	ssize_t read;
	std::string line;

	while ((read = getline(line, fin)) >= 0)
	{
		line_nbr++;
		if(line.size() == 0 || line.at(0) == '#' )
			continue;

		std::string::size_type equ_pos = line.find('=',0);
		if(equ_pos == std::string::npos)
		{
			DPRINT(INFO, "Wrong format on line %i",line_nbr);
			continue;
		}

		std::string opt = line.substr(0, equ_pos);
		std::string val = line.substr(equ_pos+1);

		// val is considered to be an int if it doesn't contain
		// a '.' (ip address have to be handled as string...
		if(val.find('.',0) == std::string::npos
		&& ((val.at(0) >= '0' && val.at(0) <= '9')
		||   val.at(0) == '-' ))
		{
			int nbr = atoi(val.c_str());
			int_value[ opt ] = nbr;
		}
		else
		{
			if(val == "true")
				bool_value[ opt ] = true;
			else
			if(val == "false")
				bool_value[ opt ] = false;
			else
				str_value[ opt ] = val;
		}
	}

	DPRINT(INFO, "Config loaded");
}

void Config::Display()
{
	DPRINT(INFO, "Current config:");
	for(std::map<std::string, bool>::iterator cfg = bool_value.begin();
		cfg != bool_value.end();
		++cfg)
	{
		DPRINT(INFO, "(bool) %s = %s", cfg->first.c_str(), cfg->second?"true":"false");
	}

	for(std::map<std::string, int>::iterator cfg = int_value.begin();
		cfg != int_value.end();
		++cfg)
	{
		DPRINT(INFO, "(int) %s = %i", cfg->first.c_str(), cfg->second);
	}

	for(std::map<std::string, std::string>::iterator cfg = str_value.begin();
		cfg != str_value.end();
		++cfg)
	{
		DPRINT(INFO, "(str) %s = %s", cfg->first.c_str(), cfg->second.c_str());
	}
}

bool Config::Get(const std::string & name, bool & value)
{
	if( bool_value.find(name) == bool_value.end() )
	{
		DPRINT(INFO, "Configuration option not found: %s", name.c_str());
		return false;
	}

	value = bool_value[ name ];
	return true;
}

bool Config::Get(const std::string & name, int & value)
{
	if( int_value.find(name) == int_value.end() )
	{
		DPRINT(INFO, "Unknown config option: %s", name.c_str());
		return false;
	}

	value = int_value[ name ];
	return true;
}

bool Config::Get(const std::string & name, std::string & value)
{
	if( str_value.find(name) == str_value.end() )
	{
		DPRINT(INFO, "Unknown config option: %s", name.c_str());
		return false;
	}

	value = str_value[ name ];
	return true;
}

void Config::SetDefault(const std::string & name, const bool & value)
{
	bool val;
	if( ! Get(name, val) )
	{
		DPRINT(INFO, "Setting to default value : %s", name.c_str());
		bool_value[ name ] = value;
	}
}

void Config::SetDefault(const std::string & name, const int & value)
{
	int val;
	if( ! Get(name, val) )
	{
		DPRINT(INFO, "Setting to default value : %s", name.c_str());
		int_value[ name ] = value;
	}
}

void Config::SetDefault(const std::string & name, const std::string & value)
{
	bool val;
	if( ! Get(name, val) )
	{
		DPRINT(INFO, "Setting to default value : %s", value.c_str());
		str_value[ name ] = value;
	}
}
