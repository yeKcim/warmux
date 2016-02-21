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
#include <assert.h>
#include <SDL/SDL.h>
#include "vkeybd/virtual-keyboard-gui.h"
#include "include/app.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "graphic/text.h"

namespace Common {

VirtualKeyboardGUI::VirtualKeyboardGUI(VirtualKeyboard *kbd) :
  _kbd(kbd), _drag(false), _displayEnabled(false), _displaying(false), _firstRun(true),
  _drawCaret(false), _cursorAnimateCounter(0), _cursorAnimateTimer(0)
{

  assert(_kbd);

  _screenW = GetMainWindow().GetWidth();
  _screenH = GetMainWindow().GetHeight();

  //memset(_cursor, 0xFF, sizeof(_cursor));
}

VirtualKeyboardGUI::~VirtualKeyboardGUI()
{
  _overlayBackup.Free();
  _dispSurface.Free();
}

void VirtualKeyboardGUI::initMode(VirtualKeyboard::Mode *mode)
{
  //ASSERT(mode->image);

  _kbdSurface = &mode->image;
  _kbdTransparentColor = mode->transparentColor;
  _kbdBound.SetSizeX(_kbdSurface->GetWidth());
  _kbdBound.SetSizeY(_kbdSurface->GetHeight());

  setupDisplayArea(mode->displayArea, mode->displayFontColor);

  if (_displaying) {
    extendDirtyRect(_kbdBound);
    redraw();
  }
}

void VirtualKeyboardGUI::setupDisplayArea(Rectanglei& r, Color forecolor)
{

  _dispFont = Font::GetInstance(Font::FONT_BIG);
  _dispX = _kbdBound.GetLeft() + r.GetLeft();
  _dispY = _kbdBound.GetTop() + r.GetTop() + (r.GetSizeY() - _dispFont->GetHeight()) / 2;
  _dispI = 0;
  _dispForeColor = forecolor;
  _dispBackColor = Color();
  _dispSurface.NewSurface(Point2i(r.GetSizeX(), _dispFont->GetHeight()), 0, true);
  _dispSurface.FillRect(Rectanglei(0, 0, _dispSurface.GetWidth(), _dispSurface.GetHeight()),
      _dispBackColor);
  _displayEnabled = true;
}

bool VirtualKeyboardGUI::fontIsSuitable(const Font */*font*/, const Rectanglei& /*rect*/)
{
  //	return (font->getMaxCharWidth() < rect.width() &&
  //			font->getFontHeight() < rect.height());
  return true;
}

void VirtualKeyboardGUI::checkScreenChanged()
{
  //	if (_lastScreenChanged != _system->getScreenChangeID())
  screenChanged();
}

void VirtualKeyboardGUI::initSize(int w, int h)
{
  _screenW = w;
  _screenH = h;
}

void VirtualKeyboardGUI::run(bool run_mainloop)
{
  if (_firstRun) {
    _firstRun = false;
    moveToDefaultPosition();
  }

  forceRedraw();
  _displaying = true;
  if (run_mainloop) {
    mainLoop();
    _overlayBackup.Free();
    _dispSurface.Free();
  }
}

void VirtualKeyboardGUI::close()
{
  _displaying = false;
}

void VirtualKeyboardGUI::reset()
{
  _kbdBound.SetSize(0, 0);
  _kbdBound.SetPosition(0, 0);
  //	_kbdBound.left = _kbdBound.top
  //		= _kbdBound.right = _kbdBound.bottom = 0;
  _displaying = _drag = false;
  _firstRun = true;
  //	_lastScreenChanged = _system->getScreenChangeID();
  _kbdSurface = 0;
}

void VirtualKeyboardGUI::moveToDefaultPosition()
{
  int kbdW = _kbdBound.GetSizeX(), kbdH = _kbdBound.GetSizeY();
  int x = 0, y = 0;
  if (_screenW != kbdW) {
    switch (_kbd->_hAlignment) {
    case VirtualKeyboard::kAlignLeft:
      x = 0;
      break;
    case VirtualKeyboard::kAlignCenter:
      x = (_screenW - kbdW) / 2;
      break;
    case VirtualKeyboard::kAlignRight:
      x = _screenW - kbdW;
      break;
    }
  }
  if (_screenH != kbdH) {
    switch (_kbd->_vAlignment) {
    case VirtualKeyboard::kAlignTop:
      y = 0;
      break;
    case VirtualKeyboard::kAlignMiddle:
      y = (_screenH - kbdH) / 2;
      break;
    case VirtualKeyboard::kAlignBottom:
      y = _screenH - kbdH;
      break;
    }
  }
  move(x, y);
}

void VirtualKeyboardGUI::move(int x, int y)
{
  // add old position to dirty area
  if (_displaying)
    extendDirtyRect(_kbdBound);

  // snap to edge of screen
  if (abs(x) < SNAP_WIDTH)
    x = 0;
  int x2 = _screenW - _kbdBound.GetSizeX();
  if (abs(x - x2) < SNAP_WIDTH)
    x = x2;
  if (abs(y) < SNAP_WIDTH)
    y = 0;
  int y2 = _screenH - _kbdBound.GetSizeY();
  if (abs(y - y2) < SNAP_WIDTH)
    y = y2;

  _dispX += x - _kbdBound.GetPositionX();
  _dispY += y - _kbdBound.GetPositionY();
  _kbdBound.SetPosition(Point2i(x, y));

  if (_displaying) {
    // add new position to dirty area
    extendDirtyRect(_kbdBound);
    redraw();
  }
}

void VirtualKeyboardGUI::screenChanged()
{
  //	g_gui.checkScreenChange();

  //	_lastScreenChanged = _system->getScreenChangeID();
  int newScreenW = GetMainWindow().GetWidth();
  int newScreenH = GetMainWindow().GetHeight();

  if (_screenW != newScreenW || _screenH != newScreenH) {
    _screenW = newScreenW;
    _screenH = newScreenH;

    _overlayBackup.NewSurface(Point2i(_screenW, _screenH), 0, true);

    if (!_kbd->checkModeResolutions()) {
      _displaying = false;
      return;
    }
    moveToDefaultPosition();
  }
}

void VirtualKeyboardGUI::mainLoop()
{

  while (_displaying) {
    handleDraw();
    AppWarmux::GetInstance()->video->Flip();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      AppWarmux::GetInstance()->CheckInactive(event);
      handleEvent(event);
    }
    // Delay for a moment
    SDL_Delay(10);
  }

}
void VirtualKeyboardGUI::handleDraw() {
  animateCaret();
  animateCursor();
  redraw();
  //if (_kbd->_keyQueue.hasStringChanged())
  updateDisplay();
}
bool VirtualKeyboardGUI::handleEvent(SDL_Event event) {
  bool contains = false;
  switch (event.type) {
  case SDL_MOUSEBUTTONDOWN:
    if (_kbdBound.Contains(Point2i(event.button.x, event.button.y))) {
       _kbd->handleMouseDown(event.button.x - _kbdBound.GetLeft(), event.button.y - _kbdBound.GetTop());
       contains = true;
    }
    break;
  case SDL_MOUSEBUTTONUP:
    if (_kbdBound.Contains(Point2i(event.button.x, event.button.y))) {
      _kbd->handleMouseUp(event.button.x - _kbdBound.GetLeft(), event.button.y - _kbdBound.GetTop());
      contains = true;
    }
    break;
  case SDL_MOUSEMOTION:
    if (_drag) {
      move(event.motion.x - _dragPoint.x, event.motion.y - _dragPoint.y);
      contains = true;
    }
    break;
  case SDL_VIDEORESIZE:
    screenChanged();
    break;
  case SDL_QUIT:
    SDL_Quit();
  default:
    break;
  }
  return contains;
}

void VirtualKeyboardGUI::startDrag(int x, int y)
{
  _drag = true;
  _dragPoint.x = x;
  _dragPoint.y = y;
}

void VirtualKeyboardGUI::endDrag()
{
  _drag = false;
}

void VirtualKeyboardGUI::extendDirtyRect(const Rectanglei &r)
{
  /*	if (_dirtyRect.isValidRect()) {
   _dirtyRect.extend(r);
   } else {
   _dirtyRect = r;
   }
   _dirtyRect.Clip(0,0,_overlayBackup.GetWidth(), _overlayBackup.GetHeight());
   */
  _dirtyRect = r;
}

void VirtualKeyboardGUI::resetDirtyRect()
{
  _dirtyRect.SetSizeX(-1);
}

void VirtualKeyboardGUI::forceRedraw()
{
  updateDisplay();
  // NULL surface extendDirtyRect(Rect(Point2i(0,0),Point2i(_overlayBackup.GetWidth(), _overlayBackup.GetHeight())));
  redraw();
}

void VirtualKeyboardGUI::redraw()
{
  assert(_kbdSurface);

  GetMainWindow().Blit(*_kbdSurface, _kbdBound.GetTopLeftPoint());
}

uint VirtualKeyboardGUI::calculateEndIndex(const std::string& str, uint startIndex)
{
  int w = 0;
  while (w <= _dispSurface.GetWidth() && startIndex < str.size()) {
    w += _dispFont->GetWidth(str.substr(startIndex++));//_dispFont->getCharWidth(str[startIndex++]);
  }
  if (w > _dispSurface.GetWidth())
    startIndex--;
  return startIndex;
}

void VirtualKeyboardGUI::animateCaret()
{
  /*	if (!_displayEnabled) return;

   if (_system->getMillis() % kCaretBlinkTime < kCaretBlinkTime / 2) {
   if (!_drawCaret) {
   _drawCaret = true;
   _dispSurface.drawLine(_caretX, 0, _caretX, _dispSurface.h, _dispForeColor);
   extendDirtyRect(Rect(_dispX + _caretX, _dispY, _dispX + _caretX + 1, _dispY + _dispSurface.h));
   }
   } else {
   if (_drawCaret) {
   _drawCaret = false;
   _dispSurface.drawLine(_caretX, 0, _caretX, _dispSurface.h, _dispBackColor);
   extendDirtyRect(Rect(_dispX + _caretX, _dispY, _dispX + _caretX + 1, _dispY + _dispSurface.h));
   }
   }
   */
}

void VirtualKeyboardGUI::updateDisplay()
{
  if (!_displayEnabled)
    return;

  Text dispTextToScreen = Text(_kbd->_keyQueue.getString(), _dispForeColor);
  dispTextToScreen.DrawLeftTop(Point2i(_dispX, _dispY + dispTextToScreen.GetHeight() / 2));
}

void VirtualKeyboardGUI::setupCursor()
{
  /*	const byte palette[] = {
   255, 255, 255, 0,
   255, 255, 255, 0,
   171, 171, 171, 0,
   87,  87,  87, 0
   };

   CursorMan.pushCursorPalette(palette, 0, 4);
   CursorMan.pushCursor(NULL, 0, 0, 0, 0, 0);
   CursorMan.showMouse(true);
   */
}

void VirtualKeyboardGUI::animateCursor()
{
  /*	int time = _system->getMillis();
   if (time > _cursorAnimateTimer + kCursorAnimateDelay) {
   for (int i = 0; i < 15; i++) {
   if ((i < 6) || (i > 8)) {
   _cursor[16 * 7 + i] = _cursorAnimateCounter;
   _cursor[16 * i + 7] = _cursorAnimateCounter;
   }
   }

   CursorMan.replaceCursor(_cursor, 16, 16, 7, 7, 255);

   _cursorAnimateTimer = time;
   _cursorAnimateCounter = (_cursorAnimateCounter + 1) % 4;
   }
   */
}

void VirtualKeyboardGUI::removeCursor()
{
  //	CursorMan.popCursor();
  //	CursorMan.popCursorPalette();
}

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
