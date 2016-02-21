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
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <netinet/in.h>

#include "SDL_config.h"

#include "SDL_version.h"

//#include "SDL_opengles.h"
#include "SDL_screenkeyboard.h"
#include "../SDL_sysvideo.h"
#include "SDL_androidvideo.h"
#include "SDL_androidinput.h"
#include "jniwrapperstuff.h"

// #include "touchscreentheme.h" // Not used yet

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

enum { MAX_BUTTONS = SDL_ANDROID_SCREENKEYBOARD_BUTTON_NUM-1, MAX_BUTTONS_AUTOFIRE = 2, BUTTON_TEXT_INPUT = SDL_ANDROID_SCREENKEYBOARD_BUTTON_TEXT-1 } ; // Max amount of custom buttons

int SDL_ANDROID_isTouchscreenKeyboardUsed = 0;
static int touchscreenKeyboardTheme = 0;
static int touchscreenKeyboardShown = 1;
static int AutoFireButtonsNum = 0;
static int buttonsize = 1;
static int transparency = 128;

static SDL_Rect arrows, buttons[MAX_BUTTONS], buttonsAutoFireRect[MAX_BUTTONS_AUTOFIRE];
static SDLKey buttonKeysyms[MAX_BUTTONS] = { 
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_0)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_1)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_2)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_3)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_4)),
SDL_KEY(SDL_KEY_VAL(SDL_ANDROID_SCREENKB_KEYCODE_5)),
0
};

enum { ARROW_LEFT = 1, ARROW_RIGHT = 2, ARROW_UP = 4, ARROW_DOWN = 8 };
static int oldArrows = 0;
static int ButtonAutoFire[MAX_BUTTONS_AUTOFIRE] = {0, 0};
static int ButtonAutoFireX[MAX_BUTTONS_AUTOFIRE*2] = {0, 0, 0, 0};
static int ButtonAutoFireRot[MAX_BUTTONS_AUTOFIRE] = {0, 0};
static int ButtonAutoFireDecay[MAX_BUTTONS_AUTOFIRE] = {0, 0};

static int pointerInButtonRect[MAX_BUTTONS + 1] = {0};

typedef struct
{
    GLuint id;
    GLfloat w;
    GLfloat h;
} GLTexture_t;

static GLTexture_t arrowImages[5] = { {0, 0, 0}, };
static GLTexture_t buttonAutoFireImages[MAX_BUTTONS_AUTOFIRE*2] = { {0, 0, 0}, };
static GLTexture_t buttonImages[MAX_BUTTONS*2] = { {0, 0, 0}, };


static inline int InsideRect(const SDL_Rect * r, int x, int y)
{
	return ( x >= r->x && x <= r->x + r->w ) && ( y >= r->y && y <= r->y + r->h );
}

static struct ScreenKbGlState_t
{
	GLboolean texture2d;
	GLuint textureId;
	GLfloat color[4];
	GLfloat texEnvMode;
	GLboolean blend;
	GLenum blend1, blend2;
	GLint texFilter1, texFilter2;
}
oldGlState;

static inline void beginDrawingTex()
{
	// Save OpenGL state
	// TODO: this code does not work on 1.6 emulator, and on some devices
	/*
	oldGlState.texture2d = glIsEnabled(GL_TEXTURE_2D);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldGlState.textureId);
	glGetFloatv(GL_CURRENT_COLOR, &(oldGlState.color[0]));
	glGetTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &oldGlState.texEnvMode);
	oldGlState.blend = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC, &oldGlState.blend1);
	glGetIntegerv(GL_BLEND_DST, &oldGlState.blend2);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &oldGlState.texFilter1);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &oldGlState.texFilter2);
	// It's very unlikely that some app will use GL_TEXTURE_CROP_RECT_OES, so just skip it
	*/

	glEnable(GL_TEXTURE_2D);
}

static inline void endDrawingTex()
{
	/*
	// Restore OpenGL state
	if( oldGlState.texture2d == GL_FALSE)
		glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oldGlState.textureId);
	glColor4f(oldGlState.color[0], oldGlState.color[1], oldGlState.color[2], oldGlState.color[3]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, oldGlState.texEnvMode);
	if( oldGlState.blend == GL_FALSE)
		glDisable(GL_BLEND);
	glBlendFunc(oldGlState.blend1, oldGlState.blend2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, oldGlState.texFilter1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, oldGlState.texFilter2);
	*/
}

static inline void drawCharTex(GLTexture_t * tex, SDL_Rect * src, SDL_Rect * dest, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	GLint cropRect[4];

	if( !dest->h || !dest->w )
		return;

	glBindTexture(GL_TEXTURE_2D, tex->id);

	glColor4x(r * 0x100, g * 0x100, b * 0x100,  a * 0x100 );

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if( SDL_ANDROID_SmoothVideo )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	cropRect[0] = 0;
	cropRect[1] = tex->h;
	cropRect[2] = tex->w;
	cropRect[3] = -tex->h;
	if(src)
	{
		cropRect[0] = src->x;
		cropRect[1] = src->h;
		cropRect[2] = src->w;
		cropRect[3] = -src->h;
	}
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, cropRect);
	glDrawTexiOES(dest->x, SDL_ANDROID_sWindowHeight - dest->y - dest->h, 0, dest->w, dest->h);
}

int SDL_ANDROID_drawTouchscreenKeyboard()
{
	int i;
	int blendFactor;

	if( !SDL_ANDROID_isTouchscreenKeyboardUsed || !touchscreenKeyboardShown )
		return 0;

	blendFactor =		( SDL_GetKeyboardState(NULL)[SDL_KEY(LEFT)] ? 1 : 0 ) +
						( SDL_GetKeyboardState(NULL)[SDL_KEY(RIGHT)] ? 1 : 0 ) +
						( SDL_GetKeyboardState(NULL)[SDL_KEY(UP)] ? 1 : 0 ) +
						( SDL_GetKeyboardState(NULL)[SDL_KEY(DOWN)] ? 1 : 0 );

	beginDrawingTex();
	if( blendFactor == 0 )
		drawCharTex( &arrowImages[0], NULL, &arrows, 255, 255, 255, transparency );
	else
	{
		if( SDL_GetKeyboardState(NULL)[SDL_KEY(LEFT)] )
			drawCharTex( &arrowImages[1], NULL, &arrows, 255, 255, 255, transparency / blendFactor );
		if( SDL_GetKeyboardState(NULL)[SDL_KEY(RIGHT)] )
			drawCharTex( &arrowImages[2], NULL, &arrows, 255, 255, 255, transparency / blendFactor );
		if( SDL_GetKeyboardState(NULL)[SDL_KEY(UP)] )
			drawCharTex( &arrowImages[3], NULL, &arrows, 255, 255, 255, transparency / blendFactor );
		if( SDL_GetKeyboardState(NULL)[SDL_KEY(DOWN)] )
			drawCharTex( &arrowImages[4], NULL, &arrows, 255, 255, 255, transparency / blendFactor );
	}

	for( i = 0; i < MAX_BUTTONS; i++ )
	{
		if( ! buttons[i].h || ! buttons[i].w )
			continue;
		if( i < AutoFireButtonsNum )
		{
			if( ButtonAutoFire[i] == 1 && SDL_GetTicks() - ButtonAutoFireDecay[i] > 1000 )
			{
				ButtonAutoFire[i] = 0;
			}
			if( ! ButtonAutoFire[i] && SDL_GetTicks() - ButtonAutoFireDecay[i] > 300 )
			{
				if( ButtonAutoFireX[i*2] > 0 )
					ButtonAutoFireX[i*2] --;
				if( ButtonAutoFireX[i*2+1] > 0 )
					ButtonAutoFireX[i*2+1] --;
				ButtonAutoFireDecay[i] = SDL_GetTicks();
			}
		}

		if( i < AutoFireButtonsNum && ! ButtonAutoFire[i] && 
			( ButtonAutoFireX[i*2] > 0 || ButtonAutoFireX[i*2+1] > 0 ) )
		{
			int pos1src = buttonImages[i*2+1].w / 2 - ButtonAutoFireX[i*2];
			int pos1dst = buttons[i].w * pos1src / buttonImages[i*2+1].w;
			int pos2src = buttonImages[i*2+1].w - ( buttonImages[i*2+1].w / 2 - ButtonAutoFireX[i*2+1] );
			int pos2dst = buttons[i].w * pos2src / buttonImages[i*2+1].w;
			
			SDL_Rect autoFireCrop = { 0, 0, pos1src, buttonImages[i*2+1].h };
			SDL_Rect autoFireDest = buttons[i];
			autoFireDest.w = pos1dst;
			
			drawCharTex( &buttonImages[i*2+1],
						&autoFireCrop, &autoFireDest, 255, 255, 255, transparency );

			autoFireCrop.x = pos2src;
			autoFireCrop.w = buttonImages[i*2+1].w - pos2src;
			autoFireDest.x = buttons[i].x + pos2dst;
			autoFireDest.w = buttons[i].w - pos2dst;

			drawCharTex( &buttonImages[i*2+1],
						&autoFireCrop, &autoFireDest, 255, 255, 255, transparency );
			
			autoFireCrop.x = pos1src;
			autoFireCrop.w = pos2src - pos1src;
			autoFireDest.x = buttons[i].x + pos1dst;
			autoFireDest.w = pos2dst - pos1dst;

			drawCharTex( &buttonAutoFireImages[i*2+1],
						&autoFireCrop, &autoFireDest, 255, 255, 255, transparency );
		}
		else
		{
			drawCharTex( ( i < AutoFireButtonsNum && ButtonAutoFire[i] ) ? &buttonAutoFireImages[i*2] :
						&buttonImages[ SDL_GetKeyboardState(NULL)[buttonKeysyms[i]] ? (i * 2 + 1) : (i * 2) ],
						NULL, &buttons[i], 255, 255, 255, transparency );
		}
	}
	endDrawingTex();

	return 1;
};

static inline int ArrowKeysPressed(int x, int y)
{
	int ret = 0, dx, dy;
	dx = x - arrows.x - arrows.w / 2;
	dy = y - arrows.y - arrows.h / 2;
	// Single arrow key pressed
	if( abs(dy / 2) >= abs(dx) )
	{
		if( dy < 0 )
			ret |= ARROW_UP;
		else
			ret |= ARROW_DOWN;
	}
	else
	if( abs(dx / 2) >= abs(dy) )
	{
		if( dx > 0 )
			ret |= ARROW_RIGHT;
		else
			ret |= ARROW_LEFT;
	}
	else // Two arrow keys pressed
	{
		if( dx > 0 )
			ret |= ARROW_RIGHT;
		else
			ret |= ARROW_LEFT;

		if( dy < 0 )
			ret |= ARROW_UP;
		else
			ret |= ARROW_DOWN;
	}
	return ret;
}

int SDL_ANDROID_processTouchscreenKeyboard(int x, int y, int action, int pointerId)
{
	int i;
	int processed = 0;

	
	if( !touchscreenKeyboardShown )
		return 0;

	
	if( action == MOUSE_DOWN )
	{
		//__android_log_print(ANDROID_LOG_INFO, "libSDL", "touch %03dx%03d ptr %d action %d", x, y, pointerId, action);
		if( InsideRect( &arrows, x, y ) )
		{
			processed = 1;
			if( pointerInButtonRect[MAX_BUTTONS] == -1 )
			{
				pointerInButtonRect[MAX_BUTTONS] = pointerId;
				if( SDL_ANDROID_isJoystickUsed )
				{
					SDL_ANDROID_MainThreadPushJoystickAxis(0, 0, (x - arrows.x - arrows.w / 2) * 65534 / arrows.w );
					SDL_ANDROID_MainThreadPushJoystickAxis(0, 1, (y - arrows.y - arrows.h / 2) * 65534 / arrows.h );
				}
				else
				{
					i = ArrowKeysPressed(x, y);
					if( i & ARROW_UP )
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(UP) );
					if( i & ARROW_DOWN )
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(DOWN) );
					if( i & ARROW_LEFT )
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(LEFT) );
					if( i & ARROW_RIGHT )
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(RIGHT) );
					oldArrows = i;
				}
			}
		}

		for( i = 0; i < MAX_BUTTONS; i++ )
		{
			if( ! buttons[i].h || ! buttons[i].w )
				continue;
			if( InsideRect( &buttons[i], x, y) )
			{
				processed = 1;
				if( pointerInButtonRect[i] == -1 )
				{
					pointerInButtonRect[i] = pointerId;
					if( i == BUTTON_TEXT_INPUT )
						SDL_ANDROID_ToggleScreenKeyboardTextInput(NULL);
					else
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, buttonKeysyms[i] );
					if( i < AutoFireButtonsNum )
					{
						ButtonAutoFire[i] = 0;
						ButtonAutoFireX[i*2] = 0;
						ButtonAutoFireX[i*2+1] = 0;
						ButtonAutoFireRot[i] = x;
						ButtonAutoFireDecay[i] = SDL_GetTicks();
					}
				}
			}
		}
	}
	else
	if( action == MOUSE_UP )
	{
		//__android_log_print(ANDROID_LOG_INFO, "libSDL", "touch %03dx%03d ptr %d action %d", x, y, pointerId, action);
		if( pointerInButtonRect[MAX_BUTTONS] == pointerId )
		{
			processed = 1;
			pointerInButtonRect[MAX_BUTTONS] = -1;
			if( SDL_ANDROID_isJoystickUsed )
			{
				SDL_ANDROID_MainThreadPushJoystickAxis(0, 0, 0 );
				SDL_ANDROID_MainThreadPushJoystickAxis(0, 1, 0 );
			}
			else
			{
				SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(UP) );
				SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(DOWN) );
				SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(LEFT) );
				SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(RIGHT) );
				oldArrows = 0;
			}
		}
		for( i = 0; i < MAX_BUTTONS; i++ )
		{
			if( ! buttons[i].h || ! buttons[i].w )
				continue;
			if( pointerInButtonRect[i] == pointerId )
			{
				processed = 1;
				pointerInButtonRect[i] = -1;
				if( i < AutoFireButtonsNum && ButtonAutoFire[i] )
				{
					ButtonAutoFire[i] = 2;
				}
				else
				{
					if( i != BUTTON_TEXT_INPUT )
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, buttonKeysyms[i] );
				}
				if( i < AutoFireButtonsNum )
				{
					ButtonAutoFireX[i*2] = 0;
					ButtonAutoFireX[i*2+1] = 0;
				}
			}
		}
	}
	else
	if( action == MOUSE_MOVE )
	{
		// Process cases when pointer enters button area (it won't send keypress twice if button already pressed)
		processed = SDL_ANDROID_processTouchscreenKeyboard(x, y, MOUSE_DOWN, pointerId);
		
		// Process cases when pointer leaves button area
		// TODO: huge code size, split it or somehow make it more readable
		if( pointerInButtonRect[MAX_BUTTONS] == pointerId )
		{
			processed = 1;
			if( ! InsideRect( &arrows, x, y ) )
			{
				pointerInButtonRect[MAX_BUTTONS] = -1;
				if( SDL_ANDROID_isJoystickUsed )
				{
					SDL_ANDROID_MainThreadPushJoystickAxis(0, 0, 0 );
					SDL_ANDROID_MainThreadPushJoystickAxis(0, 1, 0 );
				}
				else
				{
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(UP) );
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(DOWN) );
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(LEFT) );
					SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(RIGHT) );
					oldArrows = 0;
				}
			}
			else
			{
				if( SDL_ANDROID_isJoystickUsed )
				{
					SDL_ANDROID_MainThreadPushJoystickAxis(0, 0, (x - arrows.x - arrows.w / 2) * 65534 / arrows.w );
					SDL_ANDROID_MainThreadPushJoystickAxis(0, 1, (y - arrows.y - arrows.h / 2) * 65534 / arrows.h );
				}
				else
				{
					i = ArrowKeysPressed(x, y);
					if( i != oldArrows )
					{
						if( oldArrows & ARROW_UP && ! (i & ARROW_UP) )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(UP) );
						if( oldArrows & ARROW_DOWN && ! (i & ARROW_DOWN) )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(DOWN) );
						if( oldArrows & ARROW_LEFT && ! (i & ARROW_LEFT) )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(LEFT) );
						if( oldArrows & ARROW_RIGHT && ! (i & ARROW_RIGHT) )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, SDL_KEY(RIGHT) );
						if( i & ARROW_UP )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(UP) );
						if( i & ARROW_DOWN )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(DOWN) );
						if( i & ARROW_LEFT )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(LEFT) );
						if( i & ARROW_RIGHT )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_PRESSED, SDL_KEY(RIGHT) );
					}
					oldArrows = i;
				}
			}
		}
		for( i = 0; i < AutoFireButtonsNum; i++ )
		{
			if( pointerInButtonRect[i] == pointerId )
			{
				processed = 1;
				if( ! InsideRect( &buttonsAutoFireRect[i], x, y ) )
				{
					pointerInButtonRect[i] = -1;
					if( !ButtonAutoFire[i] )
					{
						if( i != BUTTON_TEXT_INPUT )
							SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, buttonKeysyms[i] );
					}
					else
					{
						ButtonAutoFire[i] = 2;
					}
					ButtonAutoFireX[i*2] = 0;
					ButtonAutoFireX[i*2+1] = 0;
				}
				else
				{
					int coeff = (buttonAutoFireImages[i*2+1].w > buttons[i].w) ? buttonAutoFireImages[i*2+1].w / buttons[i].w + 1 : 1;
					if( ButtonAutoFireRot[i] < x )
						ButtonAutoFireX[i*2+1] += (x - ButtonAutoFireRot[i]) * coeff;
					if( ButtonAutoFireRot[i] > x )
						ButtonAutoFireX[i*2] += (ButtonAutoFireRot[i] - x) * coeff;

					ButtonAutoFireRot[i] = x;

					if( ButtonAutoFireX[i*2] < 0 )
						ButtonAutoFireX[i*2] = 0;
					if( ButtonAutoFireX[i*2+1] < 0 )
						ButtonAutoFireX[i*2+1] = 0;
					if( ButtonAutoFireX[i*2] > buttonAutoFireImages[i*2+1].w / 2 )
						ButtonAutoFireX[i*2] = buttonAutoFireImages[i*2+1].w / 2;
					if( ButtonAutoFireX[i*2+1] > buttonAutoFireImages[i*2+1].w / 2 )
						ButtonAutoFireX[i*2+1] = buttonAutoFireImages[i*2+1].w / 2;

					if( ButtonAutoFireX[i*2] == buttonAutoFireImages[i*2+1].w / 2 &&
						ButtonAutoFireX[i*2+1] == buttonAutoFireImages[i*2+1].w / 2 )
					{
						if( ! ButtonAutoFire[i] )
							ButtonAutoFireDecay[i] = SDL_GetTicks();
						ButtonAutoFire[i] = 1;
					}
				}
			}
		}
		for( i = AutoFireButtonsNum; i < MAX_BUTTONS; i++ )
		{
			if( ! buttons[i].h || ! buttons[i].w )
				continue;
			if( pointerInButtonRect[i] == pointerId )
			{
				processed = 1;
				if( ! InsideRect( &buttons[i], x, y ) )
				{
					pointerInButtonRect[i] = -1;
					if( i != BUTTON_TEXT_INPUT )
						SDL_ANDROID_MainThreadPushKeyboardKey( SDL_RELEASED, buttonKeysyms[i] );
				}
			}
		}
	}
	
	return processed;
};

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetupScreenKeyboard) ( JNIEnv*  env, jobject thiz, jint size, jint theme, jint nbuttonsAutoFire, jint _transparency )
{
	int i, ii;
	int nbuttons1row, nbuttons2row;
	int _nbuttons = MAX_BUTTONS;
	touchscreenKeyboardTheme = theme;
	AutoFireButtonsNum = nbuttonsAutoFire;
	if( AutoFireButtonsNum > MAX_BUTTONS_AUTOFIRE )
		AutoFireButtonsNum = MAX_BUTTONS_AUTOFIRE;
	// TODO: works for horizontal screen orientation only!
	buttonsize = size;
	switch(_transparency)
	{
		case 0: transparency = 16; break;
		case 1: transparency = 32; break;
		case 2: transparency = 64; break;
		case 3: transparency = 128; break;
		case 4: transparency = 192; break;
		default: transparency = 128; break;
	}
	
	// Arrows to the lower-left part of screen
	arrows.x = SDL_ANDROID_sWindowWidth / 4;
	arrows.y = SDL_ANDROID_sWindowHeight - SDL_ANDROID_sWindowWidth / 4;
	arrows.w = SDL_ANDROID_sWindowWidth / (size + 2);
	arrows.h = arrows.w;
	arrows.x -= arrows.w/2;
	arrows.y -= arrows.h/2;
	// Move arrows from the center of the screen
	arrows.x -= size * SDL_ANDROID_sWindowWidth / 32;
	arrows.y += size * SDL_ANDROID_sWindowWidth / 32;

	// Buttons to the lower-right in 2 rows
	for(i = 0; i < 2; i++)
	for(ii = 0; ii < 3; ii++)
	{
		// Custom button ordering
		int iii = ii + i*2;
		if( ii == 2 )
			iii = 4 + i;
		buttons[iii].x = SDL_ANDROID_sWindowWidth - SDL_ANDROID_sWindowWidth / 12 - (SDL_ANDROID_sWindowWidth * ii / 6);
		buttons[iii].y = SDL_ANDROID_sWindowHeight - SDL_ANDROID_sWindowHeight / 8 - (SDL_ANDROID_sWindowHeight * i / 4);
		buttons[iii].w = SDL_ANDROID_sWindowWidth / (size + 2) / 3;
		buttons[iii].h = buttons[iii].w;
		buttons[iii].x -= buttons[iii].w/2;
		buttons[iii].y -= buttons[iii].h/2;
	}
	buttons[6].x = 0;
	buttons[6].y = 0;
	buttons[6].w = SDL_ANDROID_sWindowHeight/10;
	buttons[6].h = SDL_ANDROID_sWindowHeight/10;

	for( i = 0; i < sizeof(pointerInButtonRect)/sizeof(pointerInButtonRect[0]); i++ )
	{
		pointerInButtonRect[i] = -1;
	}
	for( i = 0; i < nbuttonsAutoFire; i++ )
	{
		buttonsAutoFireRect[i].w = buttons[i].w * 2;
		buttonsAutoFireRect[i].h = buttons[i].h * 2;
		buttonsAutoFireRect[i].x = buttons[i].x - buttons[i].w / 2;
		buttonsAutoFireRect[i].y = buttons[i].y - buttons[i].h / 2;
	}
};


JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetTouchscreenKeyboardUsed) ( JNIEnv*  env, jobject thiz)
{
	SDL_ANDROID_isTouchscreenKeyboardUsed = 1;
}

static int
power_of_2(int input)
{
    int value = 1;

    while (value < input) {
        value <<= 1;
    }
    return value;
}

static int setupScreenKeyboardButton( int buttonID, Uint8 * charBuf )
{
	// TODO: softstretch with antialiasing
	int w, h, len, format;
	GLTexture_t * data = NULL;
	int texture_w, texture_h;
	
	if( buttonID < 5 )
		data = &(arrowImages[buttonID]);
	else
	if( buttonID < 9 )
		data = &(buttonAutoFireImages[buttonID-5]);
	else
		data = &(buttonImages[buttonID-9]);

	if( buttonID > 22 ) // Error, array too big
		return 12; // Return value bigger than zero to iterate it

	memcpy(&w, charBuf, sizeof(int));
	memcpy(&h, charBuf + sizeof(int), sizeof(int));
	memcpy(&format, charBuf + 2*sizeof(int), sizeof(int));
	w = ntohl(w);
	h = ntohl(h);
	format = ntohl(format);
	
	texture_w = power_of_2(w);
	texture_h = power_of_2(h);
	data->w = w;
	data->h = h;

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &data->id);
	glBindTexture(GL_TEXTURE_2D, data->id);
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "On-screen keyboard generated OpenGL texture ID %d", data->id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_RGBA,
					format ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_SHORT_5_5_5_1, NULL);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA,
						format ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_SHORT_5_5_5_1,
						charBuf + 3*sizeof(int) );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDisable(GL_TEXTURE_2D);

	return 3*sizeof(int) + w * h * 2;
}

JNIEXPORT void JNICALL 
JAVA_EXPORT_NAME(Settings_nativeSetupScreenKeyboardButtons) ( JNIEnv*  env, jobject thiz, jbyteArray charBufJava )
{
	jboolean isCopy = JNI_TRUE;
	int len = (*env)->GetArrayLength(env, charBufJava);
	Uint8 * charBuf = (Uint8 *) (*env)->GetByteArrayElements(env, charBufJava, &isCopy);
	int but, pos;
	
	for( but = 0, pos = 0; pos < len; but ++ )
		pos += setupScreenKeyboardButton( but, charBuf + pos );
	
	(*env)->ReleaseByteArrayElements(env, charBufJava, (jbyte *)charBuf, 0);
}


int SDL_ANDROID_SetScreenKeyboardButtonPos(int buttonId, SDL_Rect * pos)
{
	if( buttonId < 0 || buttonId >= SDL_ANDROID_SCREENKEYBOARD_BUTTON_NUM || ! pos )
		return 0;
	
	if( buttonId == SDL_ANDROID_SCREENKEYBOARD_BUTTON_DPAD )
	{
		arrows = *pos;
	}
	else
	{
		int i = buttonId - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0;
		buttons[i] = *pos;
		if( i < AutoFireButtonsNum )
		{
			buttonsAutoFireRect[i].w = buttons[i].w * 2;
			buttonsAutoFireRect[i].h = buttons[i].h * 2;
			buttonsAutoFireRect[i].x = buttons[i].x - buttons[i].w / 2;
			buttonsAutoFireRect[i].y = buttons[i].y - buttons[i].h / 2;
		}
	}
	return 1;
};

int SDL_ANDROID_GetScreenKeyboardButtonPos(int buttonId, SDL_Rect * pos)
{
	if( buttonId < 0 || buttonId >= SDL_ANDROID_SCREENKEYBOARD_BUTTON_NUM || ! pos )
		return 0;
	
	if( buttonId == SDL_ANDROID_SCREENKEYBOARD_BUTTON_DPAD )
	{
		*pos = arrows;
	}
	else
	{
		*pos = buttons[buttonId - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0];
	}
	return 1;
};

int SDL_ANDROID_SetScreenKeyboardButtonKey(int buttonId, SDLKey key)
{
	if( buttonId < SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 || buttonId > SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 || ! key )
		return 0;
	buttonKeysyms[buttonId - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0] = key;
	return 1;
};

SDLKey SDL_ANDROID_GetScreenKeyboardButtonKey(int buttonId)
{
	if( buttonId < SDL_ANDROID_SCREENKEYBOARD_BUTTON_0 || buttonId > SDL_ANDROID_SCREENKEYBOARD_BUTTON_5 )
		return SDLK_UNKNOWN;
	return buttonKeysyms[buttonId - SDL_ANDROID_SCREENKEYBOARD_BUTTON_0];
};

int SDL_ANDROID_SetScreenKeyboardAutoFireButtonsAmount(int nbuttons)
{
	if( nbuttons < 0 || nbuttons >= MAX_BUTTONS_AUTOFIRE )
		return 0;
	AutoFireButtonsNum = nbuttons;
	return 1;
};

int SDL_ANDROID_GetScreenKeyboardAutoFireButtonsAmount()
{
	return AutoFireButtonsNum;
};

int SDL_ANDROID_SetScreenKeyboardShown(int shown)
{
	touchscreenKeyboardShown = shown;
};

int SDL_ANDROID_GetScreenKeyboardShown()
{
	return touchscreenKeyboardShown;
};

int SDL_ANDROID_GetScreenKeyboardSize()
{
	return buttonsize;
};

int SDL_ANDROID_ToggleScreenKeyboardTextInput(const char * previousText)
{
	static char textIn[255];
	if( previousText == NULL )
		previousText = "";
	strncpy(textIn, previousText, sizeof(textIn));
	textIn[sizeof(textIn)-1] = 0;
	SDL_ANDROID_CallJavaShowScreenKeyboard(textIn, NULL, 0);
	return 1;
};

int SDLCALL SDL_ANDROID_GetScreenKeyboardTextInput(char * textBuf, int textBufSize)
{
	SDL_ANDROID_CallJavaShowScreenKeyboard(textBuf, textBuf, textBufSize);
	return 1;
};

