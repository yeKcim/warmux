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

#ifndef CONFIG_H
#define CONFIG_H
#include <map>
#include <string>

const int VERSION = 1;

class Config
{
	std::map<std::string, std::string> str_value;
	std::map<std::string, int> int_value;
	std::map<std::string, bool> bool_value;

	void SetDefault(const std::string & name, const std::string & value);
	void SetDefault(const std::string & name, const int & value);
	void SetDefault(const std::string & name, const bool & value);

	void Load();
	void Display();
public:
	Config();

	bool Get( const std::string & name, 
                        std::string & value);
	bool Get( const std::string & name, 
                        int & value);
	bool Get( const std::string & name, 
                        bool & value);
};

extern Config config;

#endif
