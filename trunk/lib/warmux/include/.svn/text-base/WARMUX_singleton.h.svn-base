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
 ******************************************************************************
 * Declare base classes for singleton pattern design implementation.
 *****************************************************************************/

#ifndef WARMUX_SINGLETON_H
#define WARMUX_SINGLETON_H

#include <typeinfo>
#include <list>
#include <WARMUX_debug.h>

// Forward declarations
class BaseSingleton;
struct SDL_mutex;

typedef std::list<BaseSingleton*> SingletonList;
extern SingletonList singletons;

class BaseSingleton
{
  static SDL_mutex* singleton_mutex;
protected:
  BaseSingleton();
  virtual ~BaseSingleton();

public:
  static void ReleaseSingletons();
};

template<typename T>
class Singleton : public BaseSingleton
{
protected:
  static T* singleton;
  ~Singleton()
  {
    MSG_DBG_RTTI("singleton", "Deleting singleton %p of type %s",
                 singleton, typeid(T).name());
    if (singleton == this)
      singleton = NULL;
  }

public:
  static T* GetInstance()
  {
    if (!singleton)
    {
      singleton = new T();
      MSG_DBG_RTTI("singleton", "Created singleton %p of type %s",
                   singleton, typeid(singleton).name());
    }
    return singleton;
  }
  static const T* GetConstInstance() { return GetInstance(); }

  static T& GetRef() { return *GetInstance(); }
  static const T& GetConstRef() { return *GetInstance(); }

  static void CleanUp() { if (singleton) delete singleton; }
};

template<typename T>
T* Singleton<T>::singleton = NULL;

#endif // SINGLETON_H

