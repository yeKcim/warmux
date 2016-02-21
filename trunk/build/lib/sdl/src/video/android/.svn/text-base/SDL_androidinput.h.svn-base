/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#ifndef _SDL_ANDROIDINPUT_H_
#define _SDL_ANDROIDINPUT_H_

#include "SDL_config.h"

#include "SDL_version.h"
#include "SDL_video.h"
#include "SDL_mouse.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "SDL_events.h"
#if (SDL_VERSION_ATLEAST(1,3,0))
#include "../../events/SDL_events_c.h"
#include "../../events/SDL_keyboard_c.h"
#include "../../events/SDL_mouse_c.h"
#include "SDL_scancode.h"
#include "SDL_compat.h"
#else
#include "SDL_keysym.h"
#include "../../events/SDL_events_c.h"
#endif
#include "SDL_joystick.h"
#include "../../joystick/SDL_sysjoystick.h"
#include "../../joystick/SDL_joystick_c.h"

#include "../SDL_sysvideo.h"
#include "SDL_androidvideo.h"
#include "javakeycodes.h"

/* JNI-C++ wrapper stuff */

#if SDL_VERSION_ATLEAST(1,3,0)

#define SDL_KEY2(X) SDL_SCANCODE_ ## X
#define SDL_KEY(X) SDL_KEY2(X)

#else

#define SDL_KEY2(X) SDLK_ ## X
#define SDL_KEY(X) SDL_KEY2(X)

// Randomly redefining SDL 1.3 scancodes to SDL 1.2 keycodes
#define KP_0 KP0
#define KP_1 KP1
#define KP_2 KP2
#define KP_3 KP3
#define KP_4 KP4
#define KP_5 KP5
#define KP_6 KP6
#define KP_7 KP7
#define KP_8 KP8
#define KP_9 KP9
#define NUMLOCKCLEAR NUMLOCK
#define GRAVE DOLLAR
#define APOSTROPHE QUOTE
#define LGUI LMETA
// Overkill haha
#define A a
#define B b
#define C c
#define D d
#define E e
#define F f
#define G g
#define H h
#define I i
#define J j
#define K k
#define L l
#define M m
#define N n
#define O o
#define P p
#define Q q
#define R r
#define S s
#define T t
#define U u
#define V v
#define W w
#define X x
#define Y y
#define Z z

typedef SDLKey SDL_scancode;
#define SDL_GetKeyboardState SDL_GetKeyState

#endif

#define SDL_KEY_VAL(X) X

enum MOUSE_ACTION { MOUSE_DOWN = 0, MOUSE_UP=1, MOUSE_MOVE=2 };

extern int SDL_ANDROID_processTouchscreenKeyboard(int x, int y, int action, int pointerId);
extern int SDL_ANDROID_isTouchscreenKeyboardUsed;

#ifndef SDL_ANDROID_KEYCODE_0
#define SDL_ANDROID_KEYCODE_0 RETURN
#endif
#ifndef SDL_ANDROID_KEYCODE_1
#define SDL_ANDROID_KEYCODE_1 END
#endif
#ifndef SDL_ANDROID_KEYCODE_2
#define SDL_ANDROID_KEYCODE_2 PAGEUP
#endif
#ifndef SDL_ANDROID_KEYCODE_3
#define SDL_ANDROID_KEYCODE_3 PAGEDOWN
#endif
#ifndef SDL_ANDROID_KEYCODE_4
#define SDL_ANDROID_KEYCODE_4 LCTRL
#endif
#ifndef SDL_ANDROID_KEYCODE_5
#define SDL_ANDROID_KEYCODE_5 ESCAPE
#endif
#ifndef SDL_ANDROID_KEYCODE_6
#define SDL_ANDROID_KEYCODE_6 RSHIFT
#endif
#ifndef SDL_ANDROID_KEYCODE_7
#define SDL_ANDROID_KEYCODE_7 LSHIFT
#endif
#ifndef SDL_ANDROID_KEYCODE_8
#define SDL_ANDROID_KEYCODE_8 DELETE
#endif
#ifndef SDL_ANDROID_KEYCODE_9
#define SDL_ANDROID_KEYCODE_9 SDL_ANDROID_KEYCODE_1
#endif
#ifndef SDL_ANDROID_KEYCODE_10
#define SDL_ANDROID_KEYCODE_10 SDL_ANDROID_KEYCODE_1
#endif

// Touchscreen keyboard keys + zoom and rotate keycodes
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_0
#define SDL_ANDROID_SCREENKB_KEYCODE_0 SDL_ANDROID_KEYCODE_0
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_1
#define SDL_ANDROID_SCREENKB_KEYCODE_1 SDL_ANDROID_KEYCODE_1
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_2
#define SDL_ANDROID_SCREENKB_KEYCODE_2 SDL_ANDROID_KEYCODE_2
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_3
#define SDL_ANDROID_SCREENKB_KEYCODE_3 SDL_ANDROID_KEYCODE_3
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_4
#define SDL_ANDROID_SCREENKB_KEYCODE_4 SDL_ANDROID_KEYCODE_6
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_5
#define SDL_ANDROID_SCREENKB_KEYCODE_5 SDL_ANDROID_KEYCODE_7
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_6
#define SDL_ANDROID_SCREENKB_KEYCODE_6 SDL_ANDROID_KEYCODE_4
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_7
#define SDL_ANDROID_SCREENKB_KEYCODE_7 SDL_ANDROID_KEYCODE_5
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_8
#define SDL_ANDROID_SCREENKB_KEYCODE_8 SDL_ANDROID_KEYCODE_8
#endif
#ifndef SDL_ANDROID_SCREENKB_KEYCODE_9
#define SDL_ANDROID_SCREENKB_KEYCODE_9 SDL_ANDROID_KEYCODE_9
#endif

// Queue events to main thread
extern void SDL_ANDROID_MainThreadPushMouseMotion(int x, int y);
extern void SDL_ANDROID_MainThreadPushMouseButton(int pressed, int button);
extern void SDL_ANDROID_MainThreadPushKeyboardKey(int pressed, SDL_scancode key);
extern void SDL_ANDROID_MainThreadPushMultitouchButton(int id, int pressed, int x, int y, int force);
extern void SDL_ANDROID_MainThreadPushMultitouchMotion(int id, int x, int y, int force);
extern void SDL_ANDROID_MainThreadPushJoystickAxis(int joy, int axis, int value);
extern void SDL_ANDROID_MainThreadPushJoystickButton(int joy, int button, int pressed);
extern void SDL_ANDROID_MainThreadPushText( int ascii, int unicode );

#endif
