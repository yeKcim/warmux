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
#include <jni.h>
#include <android/log.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <string.h> // for memset()

#include "SDL_config.h"

#include "SDL_version.h"
#include "SDL_mutex.h"
#include "SDL_events.h"
#if SDL_VERSION_ATLEAST(1,3,0)
#include "SDL_touch.h"
#include "../../events/SDL_touch_c.h"
#endif

#include "../SDL_sysvideo.h"
#include "SDL_androidvideo.h"
#include "SDL_androidinput.h"
#include "SDL_screenkeyboard.h"
#include "jniwrapperstuff.h"
#include "atan2i.h"


static SDLKey SDL_android_keymap[KEYCODE_LAST+1];

static inline SDL_scancode TranslateKey(int scancode)
{
	if ( scancode >= SDL_arraysize(SDL_android_keymap) )
		scancode = KEYCODE_UNKNOWN;
	return SDL_android_keymap[scancode];
}

static int isTrackballUsed = 0;
static int isMouseUsed = 0;

enum { RIGHT_CLICK_NONE = 0, RIGHT_CLICK_WITH_MULTITOUCH = 1, RIGHT_CLICK_WITH_PRESSURE = 2, RIGHT_CLICK_WITH_KEY = 3, RIGHT_CLICK_WITH_TIMEOUT = 4 };
enum { LEFT_CLICK_NORMAL = 0, LEFT_CLICK_NEAR_CURSOR = 1, LEFT_CLICK_WITH_MULTITOUCH = 2, LEFT_CLICK_WITH_PRESSURE = 3,
		LEFT_CLICK_WITH_KEY = 4, LEFT_CLICK_WITH_TIMEOUT = 5, LEFT_CLICK_WITH_TAP = 6, LEFT_CLICK_WITH_TAP_OR_TIMEOUT = 7 };
static int leftClickMethod = LEFT_CLICK_NORMAL;
static int rightClickMethod = RIGHT_CLICK_NONE;
static int leftClickKey = KEYCODE_DPAD_CENTER;
static int rightClickKey = KEYCODE_MENU;
int SDL_ANDROID_ShowScreenUnderFinger = 0;
SDL_Rect SDL_ANDROID_ShowScreenUnderFingerRect = {0, 0, 0, 0}, SDL_ANDROID_ShowScreenUnderFingerRectSrc = {0, 0, 0, 0};
static int moveMouseWithArrowKeys = 0;
static int clickMouseWithDpadCenter = 0;
static int moveMouseWithKbSpeed = 0;
static int moveMouseWithKbAccel = 0;
static int moveMouseWithKbX = -1, moveMouseWithKbY = -1;
static int moveMouseWithKbSpeedX = 0, moveMouseWithKbSpeedY = 0;
static int moveMouseWithKbAccelX = 0, moveMouseWithKbAccelY = 0;
static int moveMouseWithKbAccelUpdateNeeded = 0;
static int maxForce = 0;
static int maxRadius = 0;
int SDL_ANDROID_isJoystickUsed = 0;
static int isMultitouchUsed = 0;
SDL_Joystick *SDL_ANDROID_CurrentJoysticks[MAX_MULTITOUCH_POINTERS+1] = {NULL};
static int TrackballDampening = 0; // in milliseconds
static Uint32 lastTrackballAction = 0;
enum { TOUCH_PTR_UP = 0, TOUCH_PTR_MOUSE = 1, TOUCH_PTR_SCREENKB = 2 };
int touchPointers[MAX_MULTITOUCH_POINTERS] = {0};
int firstMousePointerId = -1;
enum { MAX_MULTITOUCH_GESTURES = 4 };
int multitouchGestureKeycode[MAX_MULTITOUCH_GESTURES] = {
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_6)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_7)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_8)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_9))
};
int multitouchGestureKeyPressed[MAX_MULTITOUCH_GESTURES] = { 0, 0, 0, 0 };
int multitouchGestureSensitivity = 0;
int multitouchGestureDist = -1;
int multitouchGestureAngle = 0;
int multitouchGestureX = -1;
int multitouchGestureY = -1;
int SDL_ANDROID_TouchscreenCalibrationWidth = 480;
int SDL_ANDROID_TouchscreenCalibrationHeight = 320;
int SDL_ANDROID_TouchscreenCalibrationX = 0;
int SDL_ANDROID_TouchscreenCalibrationY = 0;
int leftClickTimeout = 0;
int rightClickTimeout = 0;
int mouseInitialX = -1;
int mouseInitialY = -1;
unsigned int mouseInitialTime = 0;
volatile int deferredMouseTap = 0;
int relativeMovement = 0;
int relativeMovementSpeed = 2;
int relativeMovementAccel = 0;
int relativeMovementX = 0;
int relativeMovementY = 0;
unsigned int relativeMovementTime = 0;
int oldMouseX = 0;
int oldMouseY = 0;
int oldMouseButtons = 0;

static int UnicodeToUtf8(int src, char * dest)
{
	int len = 0;
    if ( src <= 0x007f) {
        *dest++ = (char)src;
        len = 1;
    } else if (src <= 0x07ff) {
        *dest++ = (char)0xc0 | (src >> 6);
        *dest++ = (char)0x80 | (src & 0x003f);
        len = 2;
    } else if (src == 0xFEFF) {
        // nop -- zap the BOM
    } else if (src >= 0xD800 && src <= 0xDFFF) {
        // surrogates not supported
    } else if (src <= 0xffff) {
        *dest++ = (char)0xe0 | (src >> 12);
        *dest++ = (char)0x80 | ((src >> 6) & 0x003f);
        *dest++ = (char)0x80 | (src & 0x003f);
        len = 3;
    } else if (src <= 0xffff) {
        *dest++ = (char)0xf0 | (src >> 18);
        *dest++ = (char)0x80 | ((src >> 12) & 0x3f);
        *dest++ = (char)0x80 | ((src >> 6) & 0x3f);
        *dest++ = (char)0x80 | (src & 0x3f);
        len = 4;
    } else {
        // out of Unicode range
    }
    *dest = 0;
    return len;
}

static inline int InsideRect(const SDL_Rect * r, int x, int y)
{
	return ( x >= r->x && x <= r->x + r->w ) && ( y >= r->y && y <= r->y + r->h );
}

void UpdateScreenUnderFingerRect(int x, int y)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	return;
	/*
	int screenX = 320, screenY = 240;
	if( !SDL_ANDROID_ShowScreenUnderFinger )
		return;

	SDL_Window * window = SDL_GetFocusWindow();
	if( window && window->renderer->window ) {
		screenX = window->w;
		screenY = window->h;
	}
	*/
#else
	int screenX = SDL_ANDROID_sFakeWindowWidth, screenY = SDL_ANDROID_sFakeWindowHeight;
	if( !SDL_ANDROID_ShowScreenUnderFinger )
		return;

	SDL_ANDROID_ShowScreenUnderFingerRectSrc.w = screenX / 4;
	SDL_ANDROID_ShowScreenUnderFingerRectSrc.h = screenY / 4;
	SDL_ANDROID_ShowScreenUnderFingerRectSrc.x = x - SDL_ANDROID_ShowScreenUnderFingerRectSrc.w/2;
	SDL_ANDROID_ShowScreenUnderFingerRectSrc.y = y - SDL_ANDROID_ShowScreenUnderFingerRectSrc.h/2;
	if( SDL_ANDROID_ShowScreenUnderFingerRectSrc.x < 0 )
		SDL_ANDROID_ShowScreenUnderFingerRectSrc.x = 0;
	if( SDL_ANDROID_ShowScreenUnderFingerRectSrc.y < 0 )
		SDL_ANDROID_ShowScreenUnderFingerRectSrc.y = 0;
	if( SDL_ANDROID_ShowScreenUnderFingerRectSrc.x > screenX - SDL_ANDROID_ShowScreenUnderFingerRectSrc.w )
		SDL_ANDROID_ShowScreenUnderFingerRectSrc.x = screenX - SDL_ANDROID_ShowScreenUnderFingerRectSrc.w;
	if( SDL_ANDROID_ShowScreenUnderFingerRectSrc.y > screenY - SDL_ANDROID_ShowScreenUnderFingerRectSrc.h )
		SDL_ANDROID_ShowScreenUnderFingerRectSrc.y = screenY - SDL_ANDROID_ShowScreenUnderFingerRectSrc.h;
	
	SDL_ANDROID_ShowScreenUnderFingerRect.w = SDL_ANDROID_ShowScreenUnderFingerRectSrc.w * 3 / 2;
	SDL_ANDROID_ShowScreenUnderFingerRect.h = SDL_ANDROID_ShowScreenUnderFingerRectSrc.h * 3 / 2;
	SDL_ANDROID_ShowScreenUnderFingerRect.x = x + SDL_ANDROID_ShowScreenUnderFingerRect.w/10;
	SDL_ANDROID_ShowScreenUnderFingerRect.y = y - SDL_ANDROID_ShowScreenUnderFingerRect.h*11/10;
	if( SDL_ANDROID_ShowScreenUnderFingerRect.x < 0 )
		SDL_ANDROID_ShowScreenUnderFingerRect.x = 0;
	if( SDL_ANDROID_ShowScreenUnderFingerRect.y < 0 )
		SDL_ANDROID_ShowScreenUnderFingerRect.y = 0;
	if( SDL_ANDROID_ShowScreenUnderFingerRect.x + SDL_ANDROID_ShowScreenUnderFingerRect.w >= screenX )
		SDL_ANDROID_ShowScreenUnderFingerRect.x = screenX - SDL_ANDROID_ShowScreenUnderFingerRect.w - 1;
	if( SDL_ANDROID_ShowScreenUnderFingerRect.y + SDL_ANDROID_ShowScreenUnderFingerRect.h >= screenY )
		SDL_ANDROID_ShowScreenUnderFingerRect.y = screenY - SDL_ANDROID_ShowScreenUnderFingerRect.h - 1;
	if( InsideRect(&SDL_ANDROID_ShowScreenUnderFingerRect, x, y) )
		SDL_ANDROID_ShowScreenUnderFingerRect.x = x - SDL_ANDROID_ShowScreenUnderFingerRect.w*11/10 - 1;
#endif
}


JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(DemoGLSurfaceView_nativeMouse) ( JNIEnv*  env, jobject  thiz, jint x, jint y, jint action, jint pointerId, jint force, jint radius )
{
	int i;
#if SDL_VERSION_ATLEAST(1,3,0)

	SDL_Window * window = SDL_GetFocusWindow();
	if( !window )
		return;

#define SDL_ANDROID_sFakeWindowWidth window->w
#define SDL_ANDROID_sFakeWindowHeight window->h

#else
	if( !SDL_CurrentVideoSurface )
		return;
#endif
	if(pointerId < 0)
		pointerId = 0;
	if(pointerId > MAX_MULTITOUCH_POINTERS)
		pointerId = MAX_MULTITOUCH_POINTERS;
	
	// The touch is passed either to on-screen keyboard or as mouse event for all duration of touch between down and up,
	// even if the finger is not anymore above screen kb button it will not acr as mouse event, and if it's initially
	// touches the screen outside of screen kb it won't trigger button keypress -
	// I think it's more logical this way
	if( SDL_ANDROID_isTouchscreenKeyboardUsed && ( action == MOUSE_DOWN || touchPointers[pointerId] == TOUCH_PTR_SCREENKB ) )
	{
		if( SDL_ANDROID_processTouchscreenKeyboard(x, y, action, pointerId) && action == MOUSE_DOWN )
			touchPointers[pointerId] = TOUCH_PTR_SCREENKB;
		if( touchPointers[pointerId] == TOUCH_PTR_SCREENKB )
		{
			if( action == MOUSE_UP )
				touchPointers[pointerId] = TOUCH_PTR_UP;
			return;
		}
	}
	
	if( action == MOUSE_DOWN )
	{
		touchPointers[pointerId] = TOUCH_PTR_MOUSE;
		firstMousePointerId = -1;
		for( i = 0; i < MAX_MULTITOUCH_POINTERS; i++ )
		{
			if( touchPointers[i] == TOUCH_PTR_MOUSE )
			{
				firstMousePointerId = i;
				break;
			}
		}
	}

	x -= SDL_ANDROID_TouchscreenCalibrationX;
	y -= SDL_ANDROID_TouchscreenCalibrationY;
#if SDL_VIDEO_RENDER_RESIZE
	// Translate mouse coordinates

	x = x * SDL_ANDROID_sFakeWindowWidth / SDL_ANDROID_TouchscreenCalibrationWidth;
	y = y * SDL_ANDROID_sFakeWindowHeight / SDL_ANDROID_TouchscreenCalibrationHeight;
	if( x < 0 )
		x = 0;
	if( x > SDL_ANDROID_sFakeWindowWidth )
		x = SDL_ANDROID_sFakeWindowWidth;
	if( y < 0 )
		y = 0;
	if( y > SDL_ANDROID_sFakeWindowHeight )
		y = SDL_ANDROID_sFakeWindowHeight;
#else
	x = x * SDL_ANDROID_sRealWindowWidth / SDL_ANDROID_TouchscreenCalibrationWidth;
	y = y * SDL_ANDROID_sRealWindowHeight / SDL_ANDROID_TouchscreenCalibrationHeight;
#endif

	if( action == MOUSE_UP )
	{
		multitouchGestureX = -1;
		multitouchGestureY = -1;
		multitouchGestureDist = -1;
		for(i = 0; i < MAX_MULTITOUCH_GESTURES; i++)
		{
			if( multitouchGestureKeyPressed[i] )
			{
				multitouchGestureKeyPressed[i] = 0;
				SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, multitouchGestureKeycode[i] );
			}
		}
	}
	else
	{
		if( firstMousePointerId != pointerId )
		{
			multitouchGestureX = x;
			multitouchGestureY = y;
		}
		if( firstMousePointerId == pointerId && multitouchGestureX >= 0 )
		{
			int dist = abs( x - multitouchGestureX ) + abs( y - multitouchGestureY );
			int angle = atan2i( y - multitouchGestureY, x - multitouchGestureX );
			if( multitouchGestureDist < 0 )
			{
				multitouchGestureDist = dist;
				multitouchGestureAngle = angle;
			}
			else
			{
				int distMaxDiff = SDL_ANDROID_sFakeWindowHeight / ( 1 + (1 + multitouchGestureSensitivity) * 2 );
				int angleMaxDiff = atan2i_PI / ( 1 + (1 + multitouchGestureSensitivity) * 2 );
				if( dist - multitouchGestureDist > distMaxDiff )
				{
					multitouchGestureKeyPressed[0] = 1;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, multitouchGestureKeycode[0] );
				}
				else
				if( multitouchGestureKeyPressed[0] )
				{
					multitouchGestureKeyPressed[0] = 0;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, multitouchGestureKeycode[0] );
				}
				if( multitouchGestureDist - dist > distMaxDiff )
				{
					multitouchGestureKeyPressed[1] = 1;
				SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, multitouchGestureKeycode[1] );
				}
				else
				if( multitouchGestureKeyPressed[1] )
				{
					multitouchGestureKeyPressed[1] = 0;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, multitouchGestureKeycode[1] );
				}

				int angleDiff = angle - multitouchGestureAngle;

				while( angleDiff < atan2i_PI )
					angleDiff += atan2i_PI * 2;
				while( angleDiff > atan2i_PI )
					angleDiff -= atan2i_PI * 2;

				if( angleDiff < -angleMaxDiff )
				{
					multitouchGestureKeyPressed[2] = 1;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, multitouchGestureKeycode[2] );
				}
				else
				if( multitouchGestureKeyPressed[2] )
				{
					multitouchGestureKeyPressed[2] = 0;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, multitouchGestureKeycode[2] );
				}
				if( angleDiff > angleMaxDiff )
				{
					multitouchGestureKeyPressed[3] = 1;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, multitouchGestureKeycode[3] );
				}
				else
				if( multitouchGestureKeyPressed[3] )
				{
					multitouchGestureKeyPressed[3] = 0;
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, multitouchGestureKeycode[3] );
				}
			}
		}
	}

	if( isMultitouchUsed )
	{
#if SDL_VERSION_ATLEAST(1,3,0)
		// Use nifty SDL 1.3 multitouch API
		if( action == MOUSE_MOVE )
			SDL_ANDROID_MainThreadPushMultitouchMotion(pointerId, x, y, force*radius / 16);
		else
			SDL_ANDROID_MainThreadPushMultitouchButton(pointerId, action == MOUSE_DOWN ? 1 : 0, x, y, force*radius / 16);
#endif

		SDL_ANDROID_MainThreadPushJoystickAxis(pointerId+1, 0, x);
		SDL_ANDROID_MainThreadPushJoystickAxis(pointerId+1, 1, y);
		SDL_ANDROID_MainThreadPushJoystickAxis(pointerId+1, 2, force);
		SDL_ANDROID_MainThreadPushJoystickAxis(pointerId+1, 3, radius);
		if( action == MOUSE_DOWN )
			SDL_ANDROID_MainThreadPushJoystickButton(pointerId+1, 0, SDL_PRESSED);
		if( action == MOUSE_UP )
			SDL_ANDROID_MainThreadPushJoystickButton(pointerId+1, 0, SDL_RELEASED);
	}
	if( !isMouseUsed && !SDL_ANDROID_isTouchscreenKeyboardUsed )
	{
		SDL_keysym keysym;
		if( action != MOUSE_MOVE )
			SDL_ANDROID_MainThreadPushKeyboardKey( action == MOUSE_DOWN ? SDL_PRESSED : SDL_RELEASED, SDL_ANDROID_GetScreenKeyboardButtonKey(SDL_ANDROID_SCREENKEYBOARD_BUTTON_0) );
		return;
	}

	if( !isMouseUsed )
		return;

	if( pointerId == firstMousePointerId )
	{
		if( relativeMovement )
		{
			if( action == MOUSE_DOWN )
			{
				relativeMovementX = oldMouseX - x;
				relativeMovementY = oldMouseY - y;
			}
			x += relativeMovementX;
			y += relativeMovementY;
			
			int diffX = x - oldMouseX;
			int diffY = y - oldMouseY;
			int coeff = relativeMovementSpeed + 2;
			if( relativeMovementSpeed > 2 )
				coeff += relativeMovementSpeed - 2;
			diffX = diffX * coeff / 4;
			diffY = diffY * coeff / 4;
			if( relativeMovementAccel > 0 )
			{
				unsigned int newTime = SDL_GetTicks();
				if( newTime - relativeMovementTime > 0 )
				{
					diffX += diffX * ( relativeMovementAccel * 30 ) / (int)(newTime - relativeMovementTime);
					diffY += diffY * ( relativeMovementAccel * 30 ) / (int)(newTime - relativeMovementTime);
				}
				relativeMovementTime = newTime;
			}
			diffX -= x - oldMouseX;
			diffY -= y - oldMouseY;
			x += diffX;
			y += diffY;
			relativeMovementX += diffX;
			relativeMovementY += diffY;

			diffX = x;
			diffY = y;
			if( x < 0 )
				x = 0;
			if( x > SDL_ANDROID_sFakeWindowWidth )
				x = SDL_ANDROID_sFakeWindowWidth;
			if( y < 0 )
				y = 0;
			if( y > SDL_ANDROID_sFakeWindowHeight )
				y = SDL_ANDROID_sFakeWindowHeight;
			relativeMovementX += x - diffX;
			relativeMovementY += y - diffY;
		}
		if( action == MOUSE_UP )
		{
			SDL_ANDROID_MainThreadPushMouseButton( SDL_RELEASED, SDL_BUTTON_RIGHT );

			if( mouseInitialX >= 0 && mouseInitialY >= 0 && (
				leftClickMethod == LEFT_CLICK_WITH_TAP || leftClickMethod == LEFT_CLICK_WITH_TAP_OR_TIMEOUT ) &&
				abs(mouseInitialX - x) < SDL_ANDROID_sFakeWindowHeight / 16 &&
				abs(mouseInitialY - y) < SDL_ANDROID_sFakeWindowHeight / 16 &&
				SDL_GetTicks() - mouseInitialTime < 700 )
			{
				SDL_ANDROID_MainThreadPushMouseMotion(mouseInitialX, mouseInitialY);
				SDL_ANDROID_MainThreadPushMouseButton( SDL_PRESSED, SDL_BUTTON_LEFT );
				deferredMouseTap = 2;
				mouseInitialX = -1;
				mouseInitialY = -1;
			}
			else
			{
				SDL_ANDROID_MainThreadPushMouseButton( SDL_RELEASED, SDL_BUTTON_LEFT );
			}

			SDL_ANDROID_ShowScreenUnderFingerRect.w = SDL_ANDROID_ShowScreenUnderFingerRect.h = 0;
			SDL_ANDROID_ShowScreenUnderFingerRectSrc.w = SDL_ANDROID_ShowScreenUnderFingerRectSrc.h = 0;
			if( SDL_ANDROID_ShowScreenUnderFinger )
			{
				// Move mouse by 1 pixel so it will force screen update and mouse-under-finger window will be removed
				if( moveMouseWithKbX >= 0 )
					SDL_ANDROID_MainThreadPushMouseMotion(moveMouseWithKbX > 0 ? moveMouseWithKbX-1 : 0, moveMouseWithKbY);
				else
					SDL_ANDROID_MainThreadPushMouseMotion(x > 0 ? x-1 : 0, y);
			}
			moveMouseWithKbX = -1;
			moveMouseWithKbY = -1;
			moveMouseWithKbSpeedX = 0;
			moveMouseWithKbSpeedY = 0;
		}
		if( action == MOUSE_DOWN )
		{
			if( (moveMouseWithKbX >= 0 || leftClickMethod == LEFT_CLICK_NEAR_CURSOR) &&
				abs(oldMouseX - x) < SDL_ANDROID_sFakeWindowWidth / 4 && abs(oldMouseY - y) < SDL_ANDROID_sFakeWindowHeight / 4 )
			{
				SDL_ANDROID_MainThreadPushMouseButton( SDL_PRESSED, SDL_BUTTON_LEFT );
				moveMouseWithKbX = oldMouseX;
				moveMouseWithKbY = oldMouseX;
				action == MOUSE_MOVE;
			}
			else
			if( leftClickMethod == LEFT_CLICK_NORMAL )
			{
				SDL_ANDROID_MainThreadPushMouseMotion(x, y);
				SDL_ANDROID_MainThreadPushMouseButton( SDL_PRESSED, SDL_BUTTON_LEFT );
			}
			else
			{
				SDL_ANDROID_MainThreadPushMouseMotion(x, y);
				action == MOUSE_MOVE;
				mouseInitialX = x;
				mouseInitialY = y;
				mouseInitialTime = SDL_GetTicks();
			}
			UpdateScreenUnderFingerRect(x, y);
		}
		if( action == MOUSE_MOVE )
		{
			if( moveMouseWithKbX >= 0 )
			{
				if( abs(moveMouseWithKbX - x) > SDL_ANDROID_sFakeWindowWidth / 10 )
					moveMouseWithKbSpeedX += moveMouseWithKbX > x ? -1 : 1;
				else
					moveMouseWithKbSpeedX = moveMouseWithKbSpeedX * 2 / 3;
				if( abs(moveMouseWithKbY - y) > SDL_ANDROID_sFakeWindowHeight / 10 )
					moveMouseWithKbSpeedY += moveMouseWithKbY > y ? -1 : 1;
				else
					moveMouseWithKbSpeedY = moveMouseWithKbSpeedY * 2 / 3;

				moveMouseWithKbX += moveMouseWithKbSpeedX;
				moveMouseWithKbY += moveMouseWithKbSpeedY;

				if( abs(moveMouseWithKbX - x) > SDL_ANDROID_sFakeWindowWidth / 5 ||
					abs(moveMouseWithKbY - y) > SDL_ANDROID_sFakeWindowHeight / 5 )
				{
					moveMouseWithKbX = -1;
					moveMouseWithKbY = -1;
					moveMouseWithKbSpeedX = 0;
					moveMouseWithKbSpeedY = 0;
					SDL_ANDROID_MainThreadPushMouseMotion(x, y);
				}
				else
					SDL_ANDROID_MainThreadPushMouseMotion(moveMouseWithKbX, moveMouseWithKbY);
			}
			else
				SDL_ANDROID_MainThreadPushMouseMotion(x, y);

			if( rightClickMethod == RIGHT_CLICK_WITH_PRESSURE || leftClickMethod == LEFT_CLICK_WITH_PRESSURE )
			{
				int button = (leftClickMethod == LEFT_CLICK_WITH_PRESSURE) ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
				int buttonState = ( force > maxForce || radius > maxRadius );
				if( button == SDL_BUTTON_RIGHT )
					SDL_ANDROID_MainThreadPushMouseButton( SDL_RELEASED, SDL_BUTTON_LEFT );
				SDL_ANDROID_MainThreadPushMouseButton( buttonState ? SDL_PRESSED : SDL_RELEASED, button );
			}
			if( mouseInitialX >= 0 && mouseInitialY >= 0 && (
				leftClickMethod == LEFT_CLICK_WITH_TIMEOUT || leftClickMethod == LEFT_CLICK_WITH_TAP ||
				leftClickMethod == LEFT_CLICK_WITH_TAP_OR_TIMEOUT || rightClickMethod == RIGHT_CLICK_WITH_TIMEOUT ) )
			{
				if( abs(mouseInitialX - x) >= SDL_ANDROID_sFakeWindowHeight / 10 || abs(mouseInitialY - y) >= SDL_ANDROID_sFakeWindowHeight / 10 )
				{
					mouseInitialX = -1;
					mouseInitialY = -1;
				}
				else
				{
					if( leftClickMethod == LEFT_CLICK_WITH_TIMEOUT || leftClickMethod == LEFT_CLICK_WITH_TAP_OR_TIMEOUT )
					{
						if( SDL_GetTicks() - mouseInitialTime > leftClickTimeout )
						{
							SDL_ANDROID_MainThreadPushMouseMotion(mouseInitialX, mouseInitialY);
							SDL_ANDROID_MainThreadPushMouseButton( SDL_PRESSED, SDL_BUTTON_LEFT );
							mouseInitialX = -1;
							mouseInitialY = -1;
						}
					}
					if( rightClickMethod == RIGHT_CLICK_WITH_TIMEOUT )
					{
						if( SDL_GetTicks() - mouseInitialTime > rightClickTimeout )
						{
							SDL_ANDROID_MainThreadPushMouseMotion(mouseInitialX, mouseInitialY);
							SDL_ANDROID_MainThreadPushMouseButton( SDL_PRESSED, SDL_BUTTON_RIGHT );
							mouseInitialX = -1;
							mouseInitialY = -1;
						}
					}
				}
			}
			UpdateScreenUnderFingerRect(x, y);
		}
	}
	if( pointerId != firstMousePointerId && (action == MOUSE_DOWN || action == MOUSE_UP) )
	{
		if( leftClickMethod == LEFT_CLICK_WITH_MULTITOUCH )
		{
			SDL_ANDROID_MainThreadPushMouseButton( (action == MOUSE_DOWN) ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_LEFT );
		}
		else if( rightClickMethod == RIGHT_CLICK_WITH_MULTITOUCH )
		{
			SDL_ANDROID_MainThreadPushMouseButton( SDL_RELEASED, SDL_BUTTON_LEFT );
			SDL_ANDROID_MainThreadPushMouseButton( (action == MOUSE_DOWN) ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_RIGHT );
		}
	}

	if( action == MOUSE_UP )
	{
		touchPointers[pointerId] = TOUCH_PTR_UP;
		firstMousePointerId = -1;
		for( i = 0; i < MAX_MULTITOUCH_POINTERS; i++ )
		{
			if( touchPointers[i] == TOUCH_PTR_MOUSE )
			{
				firstMousePointerId = i;
				break;
			}
		}
	}
}

void ProcessDeferredMouseTap()
{
	if( deferredMouseTap > 0 )
	{
		deferredMouseTap--;
		if( deferredMouseTap <= 0 )
		{
			if( oldMouseX + 1 < SDL_ANDROID_sFakeWindowWidth )
				SDL_ANDROID_MainThreadPushMouseMotion(oldMouseX + 1, oldMouseY);
			SDL_ANDROID_MainThreadPushMouseButton( SDL_RELEASED, SDL_BUTTON_LEFT );
		}
		else if( oldMouseX > 0 ) // Force application to redraw, and call SDL_Flip()
			SDL_ANDROID_MainThreadPushMouseMotion(oldMouseX - 1, oldMouseY);
	}
}

void SDL_ANDROID_WarpMouse(int x, int y)
{
	if(!relativeMovement)
	{
		SDL_ANDROID_MainThreadPushMouseMotion(x, y);
	}
	else
	{
		SDL_ANDROID_MainThreadPushMouseMotion(x, y);
		// TODO: test and enable it
		/*
		relativeMovementX = x;
		relativeMovementY = y;
		*/
	}
};

static int processAndroidTrackball(int key, int action);

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(DemoGLSurfaceView_nativeKey) ( JNIEnv*  env, jobject thiz, jint key, jint action )
{
#if SDL_VERSION_ATLEAST(1,3,0)
#else
	if( !SDL_CurrentVideoSurface )
		return;
#endif
	if( isTrackballUsed )
		if( processAndroidTrackball(key, action) )
			return;
	if( key == rightClickKey && rightClickMethod == RIGHT_CLICK_WITH_KEY )
	{
		SDL_ANDROID_MainThreadPushMouseButton( action ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_RIGHT );
		return;
	}
	if( (key == leftClickKey && leftClickMethod == LEFT_CLICK_WITH_KEY) || (clickMouseWithDpadCenter && key == KEYCODE_DPAD_CENTER) )
	{
		SDL_ANDROID_MainThreadPushMouseButton( action ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_LEFT );
		return;
	}

	SDL_ANDROID_MainThreadPushKeyboardKey( action ? SDL_PRESSED : SDL_RELEASED, TranslateKey(key) );
}

static char * textInputBuffer = NULL;
int textInputBufferLen = 0;
int textInputBufferPos = 0;

void SDL_ANDROID_TextInputInit(char * buffer, int len)
{
	textInputBuffer = buffer;
	textInputBufferLen = len;
	textInputBufferPos = 0;
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeTextInput) ( JNIEnv*  env, jobject thiz, jint ascii, jint unicode )
{
	if( !textInputBuffer )
		SDL_ANDROID_MainThreadPushText(ascii, unicode);
	else
	{
		if( textInputBufferPos < textInputBufferLen + 4 )
		{
			textInputBufferPos += UnicodeToUtf8(unicode, textInputBuffer + textInputBufferPos);
		}
	}
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeTextInputFinished) ( JNIEnv*  env, jobject thiz )
{
	textInputBuffer = NULL;
	SDL_ANDROID_TextInputFinished();
}

static void updateOrientation ( float accX, float accY, float accZ );

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(AccelerometerReader_nativeAccelerometer) ( JNIEnv*  env, jobject  thiz, jfloat accPosX, jfloat accPosY, jfloat accPosZ )
{
#if SDL_VERSION_ATLEAST(1,3,0)
#else
	if( !SDL_CurrentVideoSurface )
		return;
#endif
	// Calculate two angles from three coordinates - TODO: this is faulty!
	//float accX = atan2f(-accPosX, sqrtf(accPosY*accPosY+accPosZ*accPosZ) * ( accPosY > 0 ? 1.0f : -1.0f ) ) * M_1_PI * 180.0f;
	//float accY = atan2f(accPosZ, accPosY) * M_1_PI;
	
	float normal = sqrt(accPosX*accPosX+accPosY*accPosY+accPosZ*accPosZ);
	if(normal <= 0.0000001f)
		normal = 0.00001f;
	
	
	updateOrientation (accPosX/normal, accPosY/normal, 0.0f);
}


JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(AccelerometerReader_nativeOrientation) ( JNIEnv*  env, jobject  thiz, jfloat accX, jfloat accY, jfloat accZ )
{
#if SDL_VERSION_ATLEAST(1,3,0)
#else
	if( !SDL_CurrentVideoSurface )
		return;
#endif
	updateOrientation (accX, accY, accZ); // TODO: make values in range 0.0:1.0
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetTrackballUsed) ( JNIEnv*  env, jobject thiz)
{
	isTrackballUsed = 1;
}

static int getClickTimeout(int v)
{
	switch(v)
	{
		case 0: return 300;
		case 1: return 500;
		case 2: return 700;
		case 3: return 1000;
		case 4: return 1500;
	}
	return 1000;
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetMouseUsed) ( JNIEnv*  env, jobject thiz,
		jint RightClickMethod, jint ShowScreenUnderFinger, jint LeftClickMethod,
		jint MoveMouseWithJoystick, jint ClickMouseWithDpad,
		jint MaxForce, jint MaxRadius,
		jint MoveMouseWithJoystickSpeed, jint MoveMouseWithJoystickAccel,
		jint LeftClickKeycode, jint RightClickKeycode,
		jint LeftClickTimeout, jint RightClickTimeout,
		jint RelativeMovement, jint RelativeMovementSpeed, jint RelativeMovementAccel)
{
	isMouseUsed = 1;
	rightClickMethod = RightClickMethod;
	SDL_ANDROID_ShowScreenUnderFinger = ShowScreenUnderFinger;
	moveMouseWithArrowKeys = MoveMouseWithJoystick;
	clickMouseWithDpadCenter = ClickMouseWithDpad;
	leftClickMethod = LeftClickMethod;
	maxForce = MaxForce;
	maxRadius = MaxRadius;
	moveMouseWithKbSpeed = MoveMouseWithJoystickSpeed + 1;
	moveMouseWithKbAccel = MoveMouseWithJoystickAccel;
	leftClickKey = LeftClickKeycode;
	rightClickKey = RightClickKeycode;
	leftClickTimeout = getClickTimeout(LeftClickTimeout);
	rightClickTimeout = getClickTimeout(RightClickTimeout);
	relativeMovement = RelativeMovement;
	relativeMovementSpeed = RelativeMovementSpeed;
	relativeMovementAccel = RelativeMovementAccel;
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "relativeMovementSpeed %d relativeMovementAccel %d", relativeMovementSpeed, relativeMovementAccel);
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetJoystickUsed) ( JNIEnv*  env, jobject thiz)
{
	SDL_ANDROID_isJoystickUsed = 1;
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetMultitouchUsed) ( JNIEnv*  env, jobject thiz)
{
	isMultitouchUsed = 1;
}


static float dx = 0.04, dy = 0.1, dz = 0.1, joystickSensitivity = 400.0f; // For accelerometer
enum { ACCELEROMETER_CENTER_FLOATING, ACCELEROMETER_CENTER_FIXED_START, ACCELEROMETER_CENTER_FIXED_HORIZ };
static int accelerometerCenterPos = ACCELEROMETER_CENTER_FLOATING;

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetAccelerometerSettings) ( JNIEnv*  env, jobject thiz, jint sensitivity, jint centerPos)
{
	dx = 0.04; dy = 0.08; dz = 0.08; joystickSensitivity = 32767.0f * 3.0f; // Fast sensitivity
	if( sensitivity == 1 )
	{
		dx = 0.1; dy = 0.15; dz = 0.15; joystickSensitivity = 32767.0f * 2.0f; // Medium sensitivity
	}
	if( sensitivity == 2 )
	{
		dx = 0.2; dy = 0.25; dz = 0.25; joystickSensitivity = 32767.0f; // Slow sensitivity
	}
	accelerometerCenterPos = centerPos;
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetTrackballDampening) ( JNIEnv*  env, jobject thiz, jint value)
{
	TrackballDampening = (value * 200);
}

void updateOrientation ( float accX, float accY, float accZ )
{
	SDL_keysym keysym;
	// TODO: ask user for accelerometer precision from Java

	static float midX = 0, midY = 0, midZ = 0;
	static int pressLeft = 0, pressRight = 0, pressUp = 0, pressDown = 0, pressR = 0, pressL = 0;
	
	if( accelerometerCenterPos == ACCELEROMETER_CENTER_FIXED_START )
	{
		accelerometerCenterPos = ACCELEROMETER_CENTER_FIXED_HORIZ;
		midX = accX;
		midY = accY;
		midZ = accZ;
	}
	
	// midX = 0.0f; // Do not remember old value for phone tilt, it feels weird

	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "updateOrientation(): %f %f %f", accX, accY, accZ);
	
	if( SDL_ANDROID_isJoystickUsed ) // TODO: mutex for that stuff?
	{
		//__android_log_print(ANDROID_LOG_INFO, "libSDL", "updateOrientation(): sending joystick event");
		SDL_ANDROID_MainThreadPushJoystickAxis(0, 0, (Sint16)(fminf(32767.0f, fmax(-32767.0f, (accX - midX) * joystickSensitivity))));
		SDL_ANDROID_MainThreadPushJoystickAxis(0, 1, (Sint16)(fminf(32767.0f, fmax(-32767.0f, -(accY - midY) * joystickSensitivity))));
		SDL_ANDROID_MainThreadPushJoystickAxis(0, 2, (Sint16)(fminf(32767.0f, fmax(-32767.0f, -(accZ - midZ) * joystickSensitivity))));

		if( accelerometerCenterPos == ACCELEROMETER_CENTER_FLOATING )
		{
			if( accY < midY - dy*2 )
				midY = accY + dy*2;
			if( accY > midY + dy*2 )
				midY = accY - dy*2;
			if( accZ < midZ - dz*2 )
				midZ = accZ + dz*2;
			if( accZ > midZ + dz*2 )
				midZ = accZ - dz*2;
		}
	}

	if(SDL_ANDROID_isJoystickUsed)
		return;

	if( accX < midX - dx )
	{
		if( !pressLeft )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: press left, acc %f mid %f d %f", accX, midX, dx);
			pressLeft = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(KEYCODE_DPAD_LEFT) );
		}
	}
	else
	{
		if( pressLeft )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: release left, acc %f mid %f d %f", accX, midX, dx);
			pressLeft = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_LEFT) );
		}
	}
	if( accX < midX - dx*2 )
		midX = accX + dx*2;

	if( accX > midX + dx )
	{
		if( !pressRight )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: press right, acc %f mid %f d %f", accX, midX, dx);
			pressRight = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(KEYCODE_DPAD_RIGHT) );
		}
	}
	else
	{
		if( pressRight )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: release right, acc %f mid %f d %f", accX, midX, dx);
			pressRight = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_RIGHT) );
		}
	}
	if( accX > midX + dx*2 )
		midX = accX - dx*2;

	if( accY < midY - dy )
	{
		if( !pressUp )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: press up, acc %f mid %f d %f", accY, midY, dy);
			pressUp = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(KEYCODE_DPAD_DOWN) );
		}
	}
	else
	{
		if( pressUp )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: release up, acc %f mid %f d %f", accY, midY, dy);
			pressUp = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_DOWN) );
		}
	}
	if( accY < midY - dy*2 )
		midY = accY + dy*2;

	if( accY > midY + dy )
	{
		if( !pressDown )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: press down, acc %f mid %f d %f", accY, midY, dy);
			pressDown = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(KEYCODE_DPAD_UP) );
		}
	}
	else
	{
		if( pressDown )
		{
			//__android_log_print(ANDROID_LOG_INFO, "libSDL", "Accelerometer: release down, acc %f mid %f d %f", accY, midY, dy);
			pressDown = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_UP) );
		}
	}
	if( accY > midY + dy*2 )
		midY = accY - dy*2;

	if( accZ < midZ - dz )
	{
		if( !pressL )
		{
			pressL = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(KEYCODE_ALT_LEFT) );
		}
	}
	else
	{
		if( pressL )
		{
			pressL = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_ALT_LEFT) );
		}
	}
	if( accZ < midZ - dz*2 )
		midZ = accZ + dz*2;

	if( accZ > midZ + dz )
	{
		if( !pressR )
		{
			pressR = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(KEYCODE_ALT_RIGHT) );
		}
	}
	else
	{
		if( pressR )
		{
			pressR = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_ALT_RIGHT) );
		}
	}
	if( accZ > midZ + dz*2 )
		midZ = accZ - dz*2;

}

static int leftPressed = 0, rightPressed = 0, upPressed = 0, downPressed = 0;

int processAndroidTrackball(int key, int action)
{
	SDL_keysym keysym;
	
	if( ! action && (
		key == KEYCODE_DPAD_UP ||
		key == KEYCODE_DPAD_DOWN ||
		key == KEYCODE_DPAD_LEFT ||
		key == KEYCODE_DPAD_RIGHT ) )
		return 1;
	lastTrackballAction = SDL_GetTicks();

	if( key == KEYCODE_DPAD_UP )
	{
		if( downPressed )
		{
			downPressed = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_DOWN) );
			return 1;
		}
		if( !upPressed )
		{
			upPressed = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		else
		{
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(key) );
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		return 1;
	}

	if( key == KEYCODE_DPAD_DOWN )
	{
		if( upPressed )
		{
			upPressed = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_UP) );
			return 1;
		}
		if( !upPressed )
		{
			downPressed = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		else
		{
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(key) );
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		return 1;
	}

	if( key == KEYCODE_DPAD_LEFT )
	{
		if( rightPressed )
		{
			rightPressed = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_RIGHT) );
			return 1;
		}
		if( !leftPressed )
		{
			leftPressed = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		else
		{
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(key) );
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		return 1;
	}

	if( key == KEYCODE_DPAD_RIGHT )
	{
		if( leftPressed )
		{
			leftPressed = 0;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_LEFT) );
			return 1;
		}
		if( !rightPressed )
		{
			rightPressed = 1;
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		else
		{
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(key) );
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, TranslateKey(key) );
		}
		return 1;
	}

	return 0;
}

void SDL_ANDROID_processAndroidTrackballDampening()
{
	if( !TrackballDampening )
		return;
	if( SDL_GetTicks() > TrackballDampening + lastTrackballAction  )
	{
		if( upPressed )
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_UP) );
		if( downPressed )
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_DOWN) );
		if( leftPressed )
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_LEFT) );
		if( rightPressed )
			SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, TranslateKey(KEYCODE_DPAD_RIGHT) );
		upPressed = 0;
		downPressed = 0;
		leftPressed = 0;
		rightPressed = 0;
	}
}

int SDL_SYS_JoystickInit(void)
{
	SDL_numjoysticks = 0;
	if( SDL_ANDROID_isJoystickUsed )
		SDL_numjoysticks = 1;
	if( isMultitouchUsed )
		SDL_numjoysticks = MAX_MULTITOUCH_POINTERS+1;

	return(SDL_numjoysticks);
}

/* Function to get the device-dependent name of a joystick */
const char *SDL_SYS_JoystickName(int index)
{
	if(index)
		return("Android multitouch");
	return("Android accelerometer/orientation sensor");
}

/* Function to open a joystick for use.
   The joystick to open is specified by the index field of the joystick.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */
int SDL_SYS_JoystickOpen(SDL_Joystick *joystick)
{
	joystick->nbuttons = 0; // Ignored
	joystick->nhats = 0;
	joystick->nballs = 0;
	if( joystick->index == 0 )
		joystick->naxes = 3;
	else
	{
		joystick->naxes = 4;
		joystick->nbuttons = 1;
	}
	SDL_ANDROID_CurrentJoysticks[joystick->index] = joystick;
	return(0);
}

/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */
void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick)
{
	return;
}

/* Function to close a joystick after use */
void SDL_SYS_JoystickClose(SDL_Joystick *joystick)
{
	SDL_ANDROID_CurrentJoysticks[joystick->index] = NULL;
	return;
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void)
{
	int i;
	for(i=0; i<MAX_MULTITOUCH_POINTERS+1; i++)
		SDL_ANDROID_CurrentJoysticks[i] = NULL;
	return;
}


enum { MAX_BUFFERED_EVENTS = 64 };
static SDL_Event BufferedEvents[MAX_BUFFERED_EVENTS];
static int BufferedEventsStart = 0, BufferedEventsEnd = 0;
static SDL_mutex * BufferedEventsMutex = NULL;

#if SDL_VERSION_ATLEAST(1,3,0)

#define SDL_SendKeyboardKey(state, keysym) SDL_SendKeyboardKey(state, (keysym)->sym)

#else

#define SDL_SendMouseMotion(A,B,X,Y) SDL_PrivateMouseMotion(0, 0, X, Y)
#define SDL_SendMouseButton(N, A, B) SDL_PrivateMouseButton( A, B, 0, 0 )
#define SDL_SendKeyboardKey(state, keysym) SDL_PrivateKeyboard(state, keysym)

#endif

extern void SDL_ANDROID_PumpEvents()
{
	SDL_Event ev;
	SDL_ANDROID_processAndroidTrackballDampening();
	SDL_ANDROID_processMoveMouseWithKeyboard();

	if( !BufferedEventsMutex )
		BufferedEventsMutex = SDL_CreateMutex();

	SDL_mutexP(BufferedEventsMutex);
	while( BufferedEventsStart != BufferedEventsEnd )
	{
		ev = BufferedEvents[BufferedEventsStart];
		BufferedEvents[BufferedEventsStart].type = 0;
		BufferedEventsStart++;
		if( BufferedEventsStart >= MAX_BUFFERED_EVENTS )
			BufferedEventsStart = 0;
		SDL_mutexV(BufferedEventsMutex);
		
		switch( ev.type )
		{
			case SDL_MOUSEMOTION:
				SDL_SendMouseMotion(NULL, 0, ev.motion.x, ev.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if( ((oldMouseButtons & SDL_BUTTON(ev.button.button)) != 0) != ev.button.state )
				{
					oldMouseButtons = (oldMouseButtons & ~SDL_BUTTON(ev.button.button)) | (ev.button.state ? SDL_BUTTON(ev.button.button) : 0);
					SDL_SendMouseButton( NULL, ev.button.state, ev.button.button );
				}
				break;
			case SDL_KEYDOWN:
				//__android_log_print(ANDROID_LOG_INFO, "libSDL", "SDL_KEYDOWN: %i %i", ev->key.keysym.sym, ev->key.state);
				SDL_SendKeyboardKey( ev.key.state, &ev.key.keysym );
				break;
			case SDL_JOYAXISMOTION:
				if( ev.jaxis.which < MAX_MULTITOUCH_POINTERS+1 && SDL_ANDROID_CurrentJoysticks[ev.jaxis.which] )
					SDL_PrivateJoystickAxis( SDL_ANDROID_CurrentJoysticks[ev.jaxis.which], ev.jaxis.axis, ev.jaxis.value );
				break;
			case SDL_JOYBUTTONDOWN:
				if( ev.jbutton.which < MAX_MULTITOUCH_POINTERS+1 && SDL_ANDROID_CurrentJoysticks[ev.jbutton.which] )
					SDL_PrivateJoystickButton( SDL_ANDROID_CurrentJoysticks[ev.jbutton.which], ev.jbutton.button, ev.jbutton.state );
				break;
#if SDL_VERSION_ATLEAST(1,3,0)
			case SDL_FINGERMOTION:
				SDL_SendTouchMotion(0, ev.tfinger.fingerId, 0, ev.tfinger.x, ev.tfinger.y, ev.tfinger.pressure);
				break;
			case SDL_FINGERDOWN:
				SDL_SendFingerDown(0, ev.tfinger.fingerId, ev.tfinger.state ? 1 : 0, ev.tfinger.x, ev.tfinger.y, ev.tfinger.pressure);
				break;
			case SDL_TEXTINPUT:
				SDL_SendKeyboardText(ev.text.text);
				break;
#endif
		}

		SDL_mutexP(BufferedEventsMutex);
	}
	SDL_mutexV(BufferedEventsMutex);
};
// Queue events to main thread
static int getNextEvent()
{
	int nextEvent;
	if( !BufferedEventsMutex )
		return -1;
	SDL_mutexP(BufferedEventsMutex);
	nextEvent = BufferedEventsEnd;
	nextEvent++;
	if( nextEvent >= MAX_BUFFERED_EVENTS )
		nextEvent = 0;
	while( nextEvent == BufferedEventsStart )
	{
		SDL_mutexV(BufferedEventsMutex);
		if( SDL_ANDROID_InsideVideoThread() )
			SDL_ANDROID_PumpEvents();
		else
			SDL_Delay(100);
		SDL_mutexP(BufferedEventsMutex);
		nextEvent = BufferedEventsEnd;
		nextEvent++;
		if( nextEvent >= MAX_BUFFERED_EVENTS )
			nextEvent = 0;
	}
	return nextEvent;
}

extern void SDL_ANDROID_MainThreadPushMouseMotion(int x, int y)
{
	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
	ev->type = SDL_MOUSEMOTION;
	ev->motion.x = x;
	ev->motion.y = y;
	oldMouseX = x;
	oldMouseY = y;
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
};
extern void SDL_ANDROID_MainThreadPushMouseButton(int pressed, int button)
{
	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
	ev->type = SDL_MOUSEBUTTONDOWN;
	ev->button.state = pressed;
	ev->button.button = button;
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
};

extern void SDL_ANDROID_MainThreadPushKeyboardKey(int pressed, SDL_scancode key)
{
	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	

	if( moveMouseWithArrowKeys && (
		key == SDL_KEY(UP) || key == SDL_KEY(DOWN) ||
		key == SDL_KEY(LEFT) || key == SDL_KEY(RIGHT) ) )
	{
		if( moveMouseWithKbX < 0 )
		{
			moveMouseWithKbX = oldMouseX;
			moveMouseWithKbY = oldMouseY;
		}

		if( pressed )
		{
			if( key == SDL_KEY(LEFT) )
			{
				if( moveMouseWithKbSpeedX > 0 )
					moveMouseWithKbSpeedX = 0;
				moveMouseWithKbSpeedX -= moveMouseWithKbSpeed;
				moveMouseWithKbAccelX = -moveMouseWithKbAccel;
				moveMouseWithKbAccelUpdateNeeded |= 1;
			}
			else if( key == SDL_KEY(RIGHT) )
			{
				if( moveMouseWithKbSpeedX < 0 )
					moveMouseWithKbSpeedX = 0;
				moveMouseWithKbSpeedX += moveMouseWithKbSpeed;
				moveMouseWithKbAccelX = moveMouseWithKbAccel;
				moveMouseWithKbAccelUpdateNeeded |= 1;
			}

			if( key == SDL_KEY(UP) )
			{
				if( moveMouseWithKbSpeedY > 0 )
					moveMouseWithKbSpeedY = 0;
				moveMouseWithKbSpeedY -= moveMouseWithKbSpeed;
				moveMouseWithKbAccelY = -moveMouseWithKbAccel;
				moveMouseWithKbAccelUpdateNeeded |= 2;
			}
			else if( key == SDL_KEY(DOWN) )
			{
				if( moveMouseWithKbSpeedY < 0 )
					moveMouseWithKbSpeedY = 0;
				moveMouseWithKbSpeedY += moveMouseWithKbSpeed;
				moveMouseWithKbAccelY = moveMouseWithKbAccel;
				moveMouseWithKbAccelUpdateNeeded |= 2;
			}
		}
		else
		{
			if( key == SDL_KEY(LEFT) || key == SDL_KEY(RIGHT) )
			{
				moveMouseWithKbSpeedX = 0;
				moveMouseWithKbAccelX = 0;
				moveMouseWithKbAccelUpdateNeeded &= ~1;
			}
			if( key == SDL_KEY(UP) || key == SDL_KEY(DOWN) )
			{
				moveMouseWithKbSpeedY = 0;
				moveMouseWithKbAccelY = 0;
				moveMouseWithKbAccelUpdateNeeded &= ~2;
			}
		}

		moveMouseWithKbX += moveMouseWithKbSpeedX;
		moveMouseWithKbY += moveMouseWithKbSpeedY;

		SDL_mutexV(BufferedEventsMutex);

		SDL_ANDROID_MainThreadPushMouseMotion(moveMouseWithKbX, moveMouseWithKbY);
		return;
	}

	ev->type = SDL_KEYDOWN;
	ev->key.state = pressed;
	ev->key.keysym.scancode = key;
	ev->key.keysym.sym = key;
	ev->key.keysym.mod = KMOD_NONE;
	ev->key.keysym.unicode = 0;
#if SDL_VERSION_ATLEAST(1,3,0)
#else
	if ( SDL_TranslateUNICODE )
		ev->key.keysym.unicode = key;
#endif
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
};

extern void SDL_ANDROID_MainThreadPushJoystickAxis(int joy, int axis, int value)
{
	if( ! ( joy < MAX_MULTITOUCH_POINTERS+1 && SDL_ANDROID_CurrentJoysticks[joy] ) )
		return;

	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
	ev->type = SDL_JOYAXISMOTION;
	ev->jaxis.which = joy;
	ev->jaxis.axis = axis;
	ev->jaxis.value = value;
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
};
extern void SDL_ANDROID_MainThreadPushJoystickButton(int joy, int button, int pressed)
{
	if( ! ( joy < MAX_MULTITOUCH_POINTERS+1 && SDL_ANDROID_CurrentJoysticks[joy] ) )
		return;

	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
	ev->type = SDL_JOYBUTTONDOWN;
	ev->jbutton.which = joy;
	ev->jbutton.button = button;
	ev->jbutton.state = pressed;
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
};
extern void SDL_ANDROID_MainThreadPushMultitouchButton(int id, int pressed, int x, int y, int force)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
	ev->type = SDL_FINGERDOWN;
	ev->tfinger.fingerId = id;
	ev->tfinger.state = pressed;
	ev->tfinger.x = x;
	ev->tfinger.y = y;
	ev->tfinger.pressure = force;
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
#endif
};
extern void SDL_ANDROID_MainThreadPushMultitouchMotion(int id, int x, int y, int force)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
	ev->type = SDL_FINGERMOTION;
	ev->tfinger.fingerId = id;
	ev->tfinger.x = x;
	ev->tfinger.y = y;
	ev->tfinger.pressure = force;
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
#endif
};

#if SDL_VERSION_ATLEAST(1,3,0)
extern void SDL_ANDROID_DeferredTextInput()
{
};
#else

enum { DEFERRED_TEXT_COUNT = 256 };
static struct { int scancode; int unicode; int down; } deferredText[DEFERRED_TEXT_COUNT];
static int deferredTextIdx1 = 0;
static int deferredTextIdx2 = 0;
static SDL_mutex * deferredTextMutex = NULL;

static SDL_keysym asciiToKeysym(int ascii, int unicode)
{
	SDL_keysym keysym;
	keysym.scancode = ascii;
	keysym.sym = ascii;
	keysym.mod = KMOD_NONE;
	keysym.unicode = 0;
	if ( SDL_TranslateUNICODE )
		keysym.unicode = unicode;
	return keysym;
}

static int checkShiftRequired( int * sym )
{
	switch( *sym )
	{
		case '!': *sym = '1'; return 1;
		case '@': *sym = '2'; return 1;
		case '#': *sym = '3'; return 1;
		case '$': *sym = '4'; return 1;
		case '%': *sym = '5'; return 1;
		case '^': *sym = '6'; return 1;
		case '&': *sym = '7'; return 1;
		case '*': *sym = '8'; return 1;
		case '(': *sym = '9'; return 1;
		case ')': *sym = '0'; return 1;
		case '_': *sym = '-'; return 1;
		case '+': *sym = '='; return 1;
		case '|': *sym = '\\';return 1;
		case '<': *sym = ','; return 1;
		case '>': *sym = '.'; return 1;
		case '?': *sym = '/'; return 1;
		case ':': *sym = ';'; return 1;
		case '"': *sym = '\'';return 1;
		case '{': *sym = '['; return 1;
		case '}': *sym = ']'; return 1;
		case '~': *sym = '`'; return 1;
		default: if( *sym >= 'A' && *sym <= 'Z' ) { *sym += 'a' - 'A'; return 1; };
	}
	return 0;
}

void SDL_ANDROID_DeferredTextInput()
{
	if( !deferredTextMutex )
		deferredTextMutex = SDL_CreateMutex();

	SDL_mutexP(deferredTextMutex);
	
	if( deferredTextIdx1 != deferredTextIdx2 )
	{
		int nextEvent = getNextEvent();
		if( nextEvent == -1 )
		{
			SDL_mutexV(deferredTextMutex);
			return;
		}
		SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
		
		deferredTextIdx1++;
		if( deferredTextIdx1 >= DEFERRED_TEXT_COUNT )
			deferredTextIdx1 = 0;
		
		ev->type = SDL_KEYDOWN;
		ev->key.state = deferredText[deferredTextIdx1].down;
		ev->key.keysym = asciiToKeysym( deferredText[deferredTextIdx1].scancode, deferredText[deferredTextIdx1].unicode );
		
		BufferedEventsEnd = nextEvent;
		SDL_mutexV(BufferedEventsMutex);
		SDL_ANDROID_MainThreadPushMouseMotion(oldMouseX + (oldMouseX % 2 ? -1 : 1), oldMouseY); // Force screen redraw
	}
	
	SDL_mutexV(deferredTextMutex);
};
#endif

extern void SDL_ANDROID_MainThreadPushText( int ascii, int unicode )
{
	int shiftRequired;

	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "SDL_ANDROID_MainThreadPushText(): %i %i", scancode, unicode);
	int nextEvent = getNextEvent();
	if( nextEvent == -1 )
		return;
	
	SDL_Event * ev = &BufferedEvents[BufferedEventsEnd];
	
#if SDL_VERSION_ATLEAST(1,3,0)

	ev->type = SDL_TEXTINPUT;
	UnicodeToUtf8(unicode, ev->text.text);

#else

	if( !deferredTextMutex )
		deferredTextMutex = SDL_CreateMutex();

	SDL_mutexP(deferredTextMutex);

	ev->type = 0;
	
	shiftRequired = checkShiftRequired(&ascii);
	
	if( shiftRequired )
	{
		deferredTextIdx2++;
		if( deferredTextIdx2 >= DEFERRED_TEXT_COUNT )
			deferredTextIdx2 = 0;
		deferredText[deferredTextIdx2].down = SDL_PRESSED;
		deferredText[deferredTextIdx2].scancode = SDLK_LSHIFT;
		deferredText[deferredTextIdx2].unicode = 0;
	}
	deferredTextIdx2++;
	if( deferredTextIdx2 >= DEFERRED_TEXT_COUNT )
		deferredTextIdx2 = 0;
	deferredText[deferredTextIdx2].down = SDL_PRESSED;
	deferredText[deferredTextIdx2].scancode = ascii;
	deferredText[deferredTextIdx2].unicode = unicode;

	deferredTextIdx2++;
	if( deferredTextIdx2 >= DEFERRED_TEXT_COUNT )
		deferredTextIdx2 = 0;
	deferredText[deferredTextIdx2].down = SDL_RELEASED;
	deferredText[deferredTextIdx2].scancode = ascii;
	deferredText[deferredTextIdx2].unicode = unicode;
	if( shiftRequired )
	{
		deferredTextIdx2++;
		if( deferredTextIdx2 >= DEFERRED_TEXT_COUNT )
			deferredTextIdx2 = 0;
		deferredText[deferredTextIdx2].down = SDL_RELEASED;
		deferredText[deferredTextIdx2].scancode = SDLK_LSHIFT;
		deferredText[deferredTextIdx2].unicode = 0;
	}

	SDL_mutexV(deferredTextMutex);

#endif
	
	BufferedEventsEnd = nextEvent;
	SDL_mutexV(BufferedEventsMutex);
};


Uint32 lastMoveMouseWithKeyboardUpdate = 0;

void SDL_ANDROID_processMoveMouseWithKeyboard()
{
	if( ! moveMouseWithKbAccelUpdateNeeded )
		return;

	Uint32 ticks = SDL_GetTicks();

	if( ticks - lastMoveMouseWithKeyboardUpdate < 20 ) // Update at 50 FPS max, or it will not work properlty on very fast devices
		return;

	lastMoveMouseWithKeyboardUpdate = ticks;

	moveMouseWithKbSpeedX += moveMouseWithKbAccelX;
	moveMouseWithKbSpeedY += moveMouseWithKbAccelY;

	moveMouseWithKbX += moveMouseWithKbSpeedX;
	moveMouseWithKbY += moveMouseWithKbSpeedY;
	SDL_ANDROID_MainThreadPushMouseMotion(moveMouseWithKbX, moveMouseWithKbY);
};

extern void SDL_ANDROID_ProcessDeferredEvents()
{
	SDL_ANDROID_DeferredTextInput();
	ProcessDeferredMouseTap();
};

void ANDROID_InitOSKeymap()
{
#if (SDL_VERSION_ATLEAST(1,3,0))
  SDLKey defaultKeymap[SDL_NUM_SCANCODES];
  SDL_GetDefaultKeymap(defaultKeymap);
  SDL_SetKeymap(0, defaultKeymap, SDL_NUM_SCANCODES);

  SDL_Touch touch;
  memset( &touch, 0, sizeof(touch) );
  touch.x_min = touch.y_min = touch.pressure_min = 0.0f;
  touch.pressure_max = 1000000;
  touch.x_max = SDL_ANDROID_sWindowWidth;
  touch.y_max = SDL_ANDROID_sWindowHeight;

  // These constants are hardcoded inside SDL_touch.c, which makes no sense for me.
  touch.xres = touch.yres = 32768;
  touch.native_xres = touch.native_yres = 32768.0f;

  touch.pressureres = 1;
  touch.native_pressureres = 1.0f;
  touch.id = 0;

  SDL_AddTouch(&touch, "Android touch screen");
#endif
}

JNIEXPORT jint JNICALL 
JAVA_EXPORT_NAME(Settings_nativeGetKeymapKey) (JNIEnv* env, jobject thiz, jint code)
{
	if( code < 0 || code > KEYCODE_LAST )
		return SDL_KEY(UNKNOWN);
	return SDL_android_keymap[code];
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetKeymapKey) (JNIEnv* env, jobject thiz, jint javakey, jint key)
{
	if( javakey < 0 || javakey > KEYCODE_LAST )
		return;
	SDL_android_keymap[javakey] = key;
}

JNIEXPORT jint JNICALL
JAVA_EXPORT_NAME(Settings_nativeGetKeymapKeyScreenKb) (JNIEnv* env, jobject thiz, jint keynum)
{
	if( keynum < 0 || keynum > SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 + 4 )
		return SDL_KEY(UNKNOWN);
		
	if( keynum <= SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 )
		return SDL_ANDROID_GetScreenKeyboardButtonKey(keynum + SDL_ANDROID_SCREENKEYBOARD_BUTTON_0);

	return SDL_KEY(UNKNOWN);
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetKeymapKeyScreenKb) (JNIEnv* env, jobject thiz, jint keynum, jint key)
{
	if( keynum < 0 || keynum > SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 + 4 )
		return;
		
	if( keynum <= SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 )
		SDL_ANDROID_SetScreenKeyboardButtonKey(keynum + SDL_ANDROID_SCREENKEYBOARD_BUTTON_0, key);
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetScreenKbKeyUsed) (JNIEnv*  env, jobject thiz, jint keynum, jint used)
{
	SDL_Rect rect = {0, 0, 0, 0};
	int key = -1;
	if( keynum == 0 )
		key = SDL_ANDROID_SCREENKEYBOARD_BUTTON_DPAD;
	if( keynum == 1 )
		key = SDL_ANDROID_SCREENKEYBOARD_BUTTON_TEXT;
	if( keynum - 2 >= 0 && keynum - 2 <= SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 )
		key = keynum - 2 + SDL_ANDROID_SCREENKEYBOARD_BUTTON_0;
		
	if( key >= 0 && !used )
		SDL_ANDROID_SetScreenKeyboardButtonPos(key, &rect);
}

JNIEXPORT jint JNICALL
JAVA_EXPORT_NAME(Settings_nativeGetKeymapKeyMultitouchGesture) (JNIEnv* env, jobject thiz, jint keynum)
{
	if( keynum < 0 || keynum >= MAX_MULTITOUCH_GESTURES )
		return SDL_KEY(UNKNOWN);
	return multitouchGestureKeycode[keynum];
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetKeymapKeyMultitouchGesture) (JNIEnv* env, jobject thiz, jint keynum, jint keycode)
{
	if( keynum < 0 || keynum >= MAX_MULTITOUCH_GESTURES )
		return;
	multitouchGestureKeycode[keynum] = keycode;
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetMultitouchGestureSensitivity) (JNIEnv* env, jobject thiz, jint sensitivity)
{
	multitouchGestureSensitivity = sensitivity;
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetTouchscreenCalibration) (JNIEnv* env, jobject thiz, jint x1, jint y1, jint x2, jint y2)
{
	SDL_ANDROID_TouchscreenCalibrationX = x1;
	SDL_ANDROID_TouchscreenCalibrationY = y1;
	SDL_ANDROID_TouchscreenCalibrationWidth = x2 - x1;
	SDL_ANDROID_TouchscreenCalibrationHeight = y2 - y1;
}

static int ScreenKbRedefinedByUser = 0;

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(Settings_nativeSetScreenKbKeyLayout) (JNIEnv* env, jobject thiz, jint keynum, jint x1, jint y1, jint x2, jint y2)
{
	SDL_Rect rect = {x1, y1, x2-x1, y2-y1};
	int key = -1;
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "nativeSetScreenKbKeyLayout: %d %d %d %d", (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h);
	if( keynum == 0 )
		key = SDL_ANDROID_SCREENKEYBOARD_BUTTON_DPAD;
	if( keynum == 1 )
		key = SDL_ANDROID_SCREENKEYBOARD_BUTTON_TEXT;
	if( keynum - 2 >= 0 && keynum - 2 <= SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 )
		key = keynum - 2 + SDL_ANDROID_SCREENKEYBOARD_BUTTON_0;
		
	if( key >= 0 )
	{
		ScreenKbRedefinedByUser = 1;
		SDL_ANDROID_SetScreenKeyboardButtonPos(key, &rect);
	}
}

int SDL_ANDROID_GetScreenKeyboardRedefinedByUser()
{
	return ScreenKbRedefinedByUser;
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeInitKeymap) ( JNIEnv*  env, jobject thiz )
{
  int i;
  SDLKey * keymap = SDL_android_keymap;

  for (i=0; i<SDL_arraysize(SDL_android_keymap); ++i)
    SDL_android_keymap[i] = SDL_KEY(UNKNOWN);

  keymap[KEYCODE_UNKNOWN] = SDL_KEY(UNKNOWN);

  keymap[KEYCODE_BACK] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_5));

  keymap[KEYCODE_MENU] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_4));

  keymap[KEYCODE_DPAD_CENTER] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_1));
  keymap[KEYCODE_SEARCH] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_9));
  keymap[KEYCODE_CALL] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_10));

  keymap[KEYCODE_VOLUME_UP] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_2));
  keymap[KEYCODE_VOLUME_DOWN] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_3));
  
  keymap[KEYCODE_HOME] = SDL_KEY(HOME); // Cannot be used in application

  // On some devices pressing Camera key will generate Camera keyevent, but releasing it will generate Focus keyevent.
  keymap[KEYCODE_CAMERA] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_6));
  keymap[KEYCODE_FOCUS] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_6));

  keymap[KEYCODE_0] = SDL_KEY(0);
  keymap[KEYCODE_1] = SDL_KEY(1);
  keymap[KEYCODE_2] = SDL_KEY(2);
  keymap[KEYCODE_3] = SDL_KEY(3);
  keymap[KEYCODE_4] = SDL_KEY(4);
  keymap[KEYCODE_5] = SDL_KEY(5);
  keymap[KEYCODE_6] = SDL_KEY(6);
  keymap[KEYCODE_7] = SDL_KEY(7);
  keymap[KEYCODE_8] = SDL_KEY(8);
  keymap[KEYCODE_9] = SDL_KEY(9);
  keymap[KEYCODE_STAR] = SDL_KEY(KP_DIVIDE);
  keymap[KEYCODE_POUND] = SDL_KEY(KP_MULTIPLY);

  keymap[KEYCODE_DPAD_UP] = SDL_KEY(UP);
  keymap[KEYCODE_DPAD_DOWN] = SDL_KEY(DOWN);
  keymap[KEYCODE_DPAD_LEFT] = SDL_KEY(LEFT);
  keymap[KEYCODE_DPAD_RIGHT] = SDL_KEY(RIGHT);

  keymap[KEYCODE_SOFT_LEFT] = SDL_KEY(KP_4);
  keymap[KEYCODE_SOFT_RIGHT] = SDL_KEY(KP_6);
  keymap[KEYCODE_ENTER] = SDL_KEY(RETURN); //SDL_KEY(KP_ENTER);


  keymap[KEYCODE_CLEAR] = SDL_KEY(BACKSPACE);
  keymap[KEYCODE_A] = SDL_KEY(A);
  keymap[KEYCODE_B] = SDL_KEY(B);
  keymap[KEYCODE_C] = SDL_KEY(C);
  keymap[KEYCODE_D] = SDL_KEY(D);
  keymap[KEYCODE_E] = SDL_KEY(E);
  keymap[KEYCODE_F] = SDL_KEY(F);
  keymap[KEYCODE_G] = SDL_KEY(G);
  keymap[KEYCODE_H] = SDL_KEY(H);
  keymap[KEYCODE_I] = SDL_KEY(I);
  keymap[KEYCODE_J] = SDL_KEY(J);
  keymap[KEYCODE_K] = SDL_KEY(K);
  keymap[KEYCODE_L] = SDL_KEY(L);
  keymap[KEYCODE_M] = SDL_KEY(M);
  keymap[KEYCODE_N] = SDL_KEY(N);
  keymap[KEYCODE_O] = SDL_KEY(O);
  keymap[KEYCODE_P] = SDL_KEY(P);
  keymap[KEYCODE_Q] = SDL_KEY(Q);
  keymap[KEYCODE_R] = SDL_KEY(R);
  keymap[KEYCODE_S] = SDL_KEY(S);
  keymap[KEYCODE_T] = SDL_KEY(T);
  keymap[KEYCODE_U] = SDL_KEY(U);
  keymap[KEYCODE_V] = SDL_KEY(V);
  keymap[KEYCODE_W] = SDL_KEY(W);
  keymap[KEYCODE_X] = SDL_KEY(X);
  keymap[KEYCODE_Y] = SDL_KEY(Y);
  keymap[KEYCODE_Z] = SDL_KEY(Z);
  keymap[KEYCODE_COMMA] = SDL_KEY(COMMA);
  keymap[KEYCODE_PERIOD] = SDL_KEY(PERIOD);
  keymap[KEYCODE_TAB] = SDL_KEY(TAB);
  keymap[KEYCODE_SPACE] = SDL_KEY(SPACE);
  keymap[KEYCODE_DEL] = SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_KEYCODE_8));
  keymap[KEYCODE_GRAVE] = SDL_KEY(GRAVE);
  keymap[KEYCODE_MINUS] = SDL_KEY(KP_MINUS);
  keymap[KEYCODE_PLUS] = SDL_KEY(KP_PLUS);
  keymap[KEYCODE_EQUALS] = SDL_KEY(EQUALS);
  keymap[KEYCODE_LEFT_BRACKET] = SDL_KEY(LEFTBRACKET);
  keymap[KEYCODE_RIGHT_BRACKET] = SDL_KEY(RIGHTBRACKET);
  keymap[KEYCODE_BACKSLASH] = SDL_KEY(BACKSLASH);
  keymap[KEYCODE_SEMICOLON] = SDL_KEY(SEMICOLON);
  keymap[KEYCODE_APOSTROPHE] = SDL_KEY(APOSTROPHE);
  keymap[KEYCODE_SLASH] = SDL_KEY(SLASH);
  keymap[KEYCODE_AT] = SDL_KEY(KP_PERIOD);

  keymap[KEYCODE_MEDIA_PLAY_PAUSE] = SDL_KEY(KP_2);
  keymap[KEYCODE_MEDIA_STOP] = SDL_KEY(HELP);
  keymap[KEYCODE_MEDIA_NEXT] = SDL_KEY(KP_8);
  keymap[KEYCODE_MEDIA_PREVIOUS] = SDL_KEY(KP_5);
  keymap[KEYCODE_MEDIA_REWIND] = SDL_KEY(KP_1);
  keymap[KEYCODE_MEDIA_FAST_FORWARD] = SDL_KEY(KP_3);
  keymap[KEYCODE_MUTE] = SDL_KEY(KP_0);

  keymap[KEYCODE_SYM] = SDL_KEY(LGUI);
  keymap[KEYCODE_NUM] = SDL_KEY(NUMLOCKCLEAR);

  keymap[KEYCODE_ALT_LEFT] = SDL_KEY(KP_7);
  keymap[KEYCODE_ALT_RIGHT] = SDL_KEY(KP_9);

  keymap[KEYCODE_SHIFT_LEFT] = SDL_KEY(F1);
  keymap[KEYCODE_SHIFT_RIGHT] = SDL_KEY(F2);

  keymap[KEYCODE_EXPLORER] = SDL_KEY(F3);
  keymap[KEYCODE_ENVELOPE] = SDL_KEY(F4);

  keymap[KEYCODE_HEADSETHOOK] = SDL_KEY(F5);
  keymap[KEYCODE_NOTIFICATION] = SDL_KEY(F7);

  // Cannot be received by application, OS internal
  keymap[KEYCODE_ENDCALL] = SDL_KEY(LSHIFT);
  keymap[KEYCODE_POWER] = SDL_KEY(RALT);

}
