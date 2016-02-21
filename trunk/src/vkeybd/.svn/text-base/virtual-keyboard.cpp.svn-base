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

#include <WARMUX_config.h>

#ifdef ENABLE_VKEYBD

#include <WARMUX_error.h>
#include <WARMUX_file_tools.h>
#include "graphic/video.h"
#include "include/constant.h"
#include "include/app.h"
#include "vkeybd/virtual-keyboard.h"
#include "vkeybd/virtual-keyboard-gui.h"
#include "vkeybd/virtual-keyboard-parser.h"
#include "vkeybd/keycode-descriptions.h"
#include "game/config.h"

#define KEY_START_CHAR ('[')
#define KEY_END_CHAR (']')

namespace Common {

VirtualKeyboard::VirtualKeyboard() :
  _currentMode(0),
  _submitCallback(NULL)
{
  _parser = new VirtualKeyboardParser(this);
  _kbdGUI = new VirtualKeyboardGUI(this);
  _submitKeys = _loaded = false;
  _fileArchive = 0;
}

VirtualKeyboard::~VirtualKeyboard()
{
  deleteEvents();
  delete _kbdGUI;
  delete _parser;
}

void VirtualKeyboard::deleteEvents()
{
  ModeMap::iterator it_m;
  VKEventMap::iterator it_e;
  for (it_m = _modes.begin(); it_m != _modes.end(); ++it_m) {
    VKEventMap *evt = &(it_m->second.events);
    for (it_e = evt->begin(); it_e != evt->end(); ++it_e)
      delete it_e->second;
  }
}

void VirtualKeyboard::reset()
{
  deleteEvents();
  _modes.clear();
  _initialMode = _currentMode = 0;
  _hAlignment = kAlignCenter;
  _vAlignment = kAlignBottom;
  _keyQueue.clear();
  _loaded = false;
  _kbdGUI->reset();
}

bool VirtualKeyboard::openPack(const std::string &packName, const std::string &node)
{
  if (DoesFileExist(node + "/" + packName + "/" + packName + ".xml")) {
    // uncompressed keyboard pack
    if (!_parser->Load(node + "/" + packName + "/" + packName + ".xml")) {
      return false;
    }
    return true;
  }
  return false;
}

bool VirtualKeyboard::loadKeyboardPack(const std::string &packName)
{
  _kbdGUI->initSize(GetMainWindow().GetWidth(), GetMainWindow().GetHeight());

  _loaded = false;

  bool opened = false;
  opened = openPack(packName, Config::GetConstInstance()->GetDataDir() + "/vkeyb");

  // fallback to the current dir
  if (!opened)
    opened = openPack(packName, ".");

  if (opened) {
    _parser->setParseMode(VirtualKeyboardParser::kParseFull);
    _loaded = _parser->parse();

    if (_loaded) {
      printf("Keyboard pack '%s' loaded successfully\n", packName.c_str());
    } else {
      printf("Error parsing the keyboard pack '%s\n'", packName.c_str());
    }
  } else {
    printf("Keyboard pack not found\n");
  }

  return _loaded;
}

bool VirtualKeyboard::checkModeResolutions()
{
  _parser->setParseMode(VirtualKeyboardParser::kParseCheckResolutions);
  _loaded = _parser->IsOk();
  if (_currentMode)
    _kbdGUI->initMode(_currentMode);
  return _loaded;
}

std::string VirtualKeyboard::findArea(int x, int y)
{
  return _currentMode->imageMap.findMapArea(x, y);
}

void VirtualKeyboard::processAreaClick(const std::string& area)
{
  if (!_currentMode->events.count(area))
    return;

  VKEvent *evt = _currentMode->events[area];

  switch (evt->type) {
  case kVKEventKey:
    // add virtual keypress to queue
    _keyQueue.insertKey(*(KeyState*) evt->data);
    break;
  case kVKEventModifier:
    _keyQueue.toggleFlags(*(int*) (evt->data));
    break;
  case kVKEventSwitchMode:
    // switch to new mode
    switchMode((char *) evt->data);
    _keyQueue.clearFlags();
    break;
  case kVKEventSubmit:
    if (_submitCallback != NULL)
      _submitCallback(getString());
    close(true);
    break;
  case kVKEventCancel:
    close(false);
    break;
  case kVKEventClear:
    _keyQueue.clear();
    break;
  case kVKEventDelete:
    _keyQueue.deleteKey();
    break;
  case kVKEventMoveLeft:
    _keyQueue.moveLeft();
    break;
  case kVKEventMoveRight:
    _keyQueue.moveRight();
    break;
  }
}

void VirtualKeyboard::switchMode(Mode *newMode)
{
  _kbdGUI->initMode(newMode);
  _currentMode = newMode;
}

void VirtualKeyboard::switchMode(const std::string&newMode)
{
  if (!_modes.count(newMode)) {
    Warning("Keyboard mode '" + newMode + "' unknown");
  } else {
    switchMode(&_modes[newMode]);
  }
}

void VirtualKeyboard::handleMouseDown(int x, int y)
{
  _areaDown = findArea(x, y);
  if (_areaDown.empty())
    _kbdGUI->startDrag(x, y);
}

void VirtualKeyboard::handleMouseUp(int x, int y)
{
  if (!_areaDown.empty() && _areaDown == findArea(x, y)) {
    processAreaClick(_areaDown);
    _areaDown.clear();
  }
  _kbdGUI->endDrag();
}

bool VirtualKeyboard::show(bool blocking)
{
  if (!_loaded) {
    Warning("Virtual keyboard not loaded");
    return false;
  } else {
    _kbdGUI->checkScreenChanged();
  }

  switchMode(_initialMode);
  _kbdGUI->run(blocking);

  return _submitKeys;
}

void VirtualKeyboard::close(bool submit)
{
  AppWarmux::GetInstance()->RefreshDisplay();
  _submitKeys = submit;
  _kbdGUI->close();
}

bool VirtualKeyboard::isDisplaying()
{
  return _kbdGUI->isDisplaying();
}

bool VirtualKeyboard::handleEvent(SDL_Event event) {
  return _kbdGUI->handleEvent(event);
}

void VirtualKeyboard::handleDraw() {
  _kbdGUI->handleDraw();
}

void VirtualKeyboard::setSubmitCallback(SubmitCallbackFunction submitCallback) {
  _submitCallback = submitCallback;
}

VirtualKeyboard::KeyPressQueue::KeyPressQueue()
{
  _keyPos = _keys.end();
  _strPos = 0;
  _strChanged = false;
  _flags = 0;
}

void VirtualKeyboard::KeyPressQueue::toggleFlags(int fl)
{
  _flags ^= fl;
  _flagsStr.clear();
  _strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::clearFlags()
{
  _flags = 0;
  _flagsStr.clear();
  _strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::insertKey(KeyState key)
{
  _strChanged = true;
  key.mod = (SDLMod) (key.mod ^ _flags);
  if ((key.sym >= SDLK_a) && (key.sym <= SDLK_z))
    key.unicode = (key.mod & KMOD_SHIFT) ? key.scancode - 32 : key.scancode;
  clearFlags();
  std::string keyStr = "";

  if (key.scancode >= 32 && key.scancode <= 255) {
    keyStr = (char) key.unicode;
  } else {
    if (key.scancode >= 0 && key.scancode < keycodeDescTableSize)
      keyStr = keycodeDescTable[key.scancode];
  }

  if (keyStr.empty())
    keyStr = "???";

  const char *k = keyStr.c_str();
  while (char ch = *k++)
    _keysStr.insert(_strPos++, &ch);
}

void VirtualKeyboard::KeyPressQueue::deleteKey()
{
  if (!_keysStr.empty())
    _keysStr.erase(--_strPos, 1);
}

void VirtualKeyboard::KeyPressQueue::moveLeft()
{
  if (_keyPos == _keys.begin())
    return;
  _keyPos--;
  _strPos -= _keyPos->strLen;
  _strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::moveRight()
{
  if (_keyPos == _keys.end())
    return;
  _strPos += _keyPos->strLen;
  _keyPos++;
  _strChanged = true;
}

KeyState VirtualKeyboard::KeyPressQueue::pop()
{
  bool front = (_keyPos == _keys.begin());
  VirtualKeyPress kp = *(_keys.begin());
  _keys.pop_front();

  if (front)
    _keyPos = _keys.begin();
  else
    _strPos -= kp.strLen;

  while (kp.strLen-- > 0)
    _keysStr.erase(0, 1);

  return kp.key;
}

void VirtualKeyboard::KeyPressQueue::clear()
{
  _keys.clear();
  _keyPos = _keys.end();
  _keysStr.clear();
  _strPos = 0;
  clearFlags();
  _strChanged = true;
}

bool VirtualKeyboard::KeyPressQueue::empty()
{
  return _keys.empty();
}

std::string VirtualKeyboard::KeyPressQueue::getString()
{
  return _keysStr;
}

void VirtualKeyboard::KeyPressQueue::setString(std::string str)
{
  _keysStr = str;
  _strPos = str.length();
}

uint VirtualKeyboard::KeyPressQueue::getInsertIndex()
{
  return _strPos + _flagsStr.size();
}

bool VirtualKeyboard::KeyPressQueue::hasStringChanged()
{
  bool ret = _strChanged;
  _strChanged = false;
  return ret;
}

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
