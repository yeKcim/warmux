/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 ******************************************************************************
 *
 * This file is derived from "ScummVM - Graphic Adventure Engine"
 * Original licence below:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef COMMON_VIRTUAL_KEYBOARD_H
#define COMMON_VIRTUAL_KEYBOARD_H

#include <WARMUX_config.h>

#ifdef ENABLE_VKEYBD

#include <SDL/SDL.h>
#include <map>
#include <list>
#include <WARMUX_singleton.h>
#include "graphic/surface.h"
#include "vkeybd/image-map.h"

typedef SDL_keysym KeyState;
typedef Uint8 KeyCode; //SDL_keysym.scancode

namespace Common {

class Archive;

class VirtualKeyboardGUI;
class VirtualKeyboardParser;

/**
 * Class that handles the functionality of the virtual keyboard.
 * This includes storage of the virtual key press events when the user clicks
 * a key and delivery of them when the keyboard is closed, as well as managing
 * the internal state of the keyboard, such as its active mode.
 */
class VirtualKeyboard: public Singleton<VirtualKeyboard>
{
protected:

  /**
   * Enum to describe the different types of events that can be associated
   * with an area of the virtual keyboard bitmap.
   */
  enum VKEventType
  {
    /** Standard key press event */
    kVKEventKey,
    /** Modifier key press event */
    kVKEventModifier,
    /** Switch the mode of the keyboard */
    kVKEventSwitchMode,
    /** Close the keyboard, submitting all keypresses */
    kVKEventSubmit,
    /** Close the keyboard, without submitting keypresses */
    kVKEventCancel,
    /** Clear the virtual keypress queue */
    kVKEventClear,
    /** Move the keypress queue insert position backwards */
    kVKEventMoveLeft,
    /** Move the keypress queue insert position forwards */
    kVKEventMoveRight,
    /** Delete keypress from queue at the current insert position */
    kVKEventDelete
  };

  /** VKEvent struct encapsulates data on a virtual keyboard event */
  struct VKEvent
  {
    std::string name;
    VKEventType type;
    /**
     * Void pointer that will point to different types of data depending
     * on the type of the event, these are:
     * - KeyState struct for kVKEventKey events
     * - a flags byte for kVKEventModifier events
     * - c-string stating the name of the new mode for kSwitchMode events
     */
    void *data;

    VKEvent() :
      data(0)
    {
    }
    ~VKEvent()
    {
      if (data)
        free(data);
    }
  };

  typedef std::map<std::string, VKEvent*> VKEventMap;

  /**
   * Mode struct encapsulates all the data for each mode of the keyboard
   */
  struct Mode
  {
    std::string name;
    std::string resolution;
    std::string bitmapName;
    Surface image;
    Color transparentColor;
    ImageMap imageMap;
    VKEventMap events;
    Rectanglei displayArea;
    Color displayFontColor;

    Mode() :
      image(0)
    {
    }
    ~Mode()
    {
      //	if (image) {
      //		image->Free();
      //		delete image;
      //		image = 0;
      //	}
    }
  };

  typedef std::map<std::string, Mode> ModeMap;

  enum HorizontalAlignment
  {
    kAlignLeft,
    kAlignCenter,
    kAlignRight
  };

  enum VerticalAlignment
  {
    kAlignTop,
    kAlignMiddle,
    kAlignBottom
  };

  struct VirtualKeyPress
  {
    KeyState key;
    /** length of the key presses description string */
    uint strLen;
  };

  typedef void (*SubmitCallbackFunction)(const std::string &);

  /**
   * Class that stores the queue of virtual key presses, as well as
   * maintaining a string that represents a preview of the queue
   */
  class KeyPressQueue
  {
  public:
    KeyPressQueue();
    void toggleFlags(int fl);
    void clearFlags();
    void insertKey(KeyState key);
    void deleteKey();
    void moveLeft();
    void moveRight();
    KeyState pop();
    void clear();
    bool empty();
    std::string getString();
    void setString(std::string str);
    uint getInsertIndex();
    bool hasStringChanged();

  private:
    int _flags;
    std::string _flagsStr;

    std::list<VirtualKeyPress> _keys;
    std::string _keysStr;

    bool _strChanged;

    std::list<VirtualKeyPress>::iterator _keyPos;
    uint _strPos;
  };

public:

  VirtualKeyboard();

  virtual ~VirtualKeyboard();

  /**
   * Loads the keyboard pack with the given name.
   * The system first looks for an uncompressed keyboard pack by searching
   * for packName.xml in the filesystem, if this does not exist then it
   * searches for a compressed keyboard pack by looking for packName.zip.
   * @param packName	name of the keyboard pack
   */
  bool loadKeyboardPack(const std::string &packName);

  /**
   * Shows the keyboard, starting an event loop that will intercept all
   * user input (like a modal GUI dialog).
   * It is assumed that the game has been paused, before this is called
   */
  bool show(bool blocking = true);

  /**
   * Hides the keyboard, ending the event loop.
   * @param submit	if true all accumulated key presses are submitted to
   *					the event manager
   */
  void close(bool submit);

  /**
   * Returns true if the keyboard is currently being shown
   */
  bool isDisplaying();

  /**
   * Returns true if the keyboard is loaded and ready to be shown
   */
  bool isLoaded() {
    return _loaded;
  }

  bool handleEvent(SDL_Event event);

  void handleDraw();

  std::string getString() {
    return _keyQueue.getString();
  }

  void setString(std::string str) {
    _keyQueue.setString(str);
  }

  void setSubmitCallback(SubmitCallbackFunction submitCallback);

protected:

  Archive *_fileArchive;

  friend class VirtualKeyboardGUI;
  VirtualKeyboardGUI *_kbdGUI;

  KeyPressQueue _keyQueue;

  friend class VirtualKeyboardParser;
  VirtualKeyboardParser *_parser;

  void reset();
  bool openPack(const std::string &packName, const std::string &node);
  void deleteEvents();
  bool checkModeResolutions();
  void switchMode(Mode *newMode);
  void switchMode(const std::string& newMode);
  void handleMouseDown(int x, int y);
  void handleMouseUp(int x, int y);
  std::string findArea(int x, int y);
  void processAreaClick(const std::string &area);

  bool _loaded;

  ModeMap _modes;
  Mode *_initialMode;
  Mode *_currentMode;

  HorizontalAlignment _hAlignment;
  VerticalAlignment _vAlignment;

  std::string _areaDown;

  bool _submitKeys;
  SubmitCallbackFunction _submitCallback;
};

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
#endif // #ifndef COMMON_VIRTUAL_KEYBOARD_H
