/*
    SDL - Simple DirectMedia Layer

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


*/

/*
    SDL_epocevents_c.h
    Handle the event stream, converting Epoc events into SDL events

    Symbian version by  Markus Mertama
    
*/


#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_aaevents_c.h,v 1.1.2.2 2000/03/16 15:20:39 hercules Exp $";
#endif

extern "C" {
#include "SDL_sysvideo.h"
//#include "SDL_epocvideo.h"
}



#define MAX_SCANCODE 255

/* Variables and functions exported by SDL_sysevents.c to other parts 
   of the native video subsystem (SDL_sysvideo.c)
*/

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *_this
#define Private	_this->hidden

extern "C" {
extern void InitOSKeymap(_THIS);
extern void PumpEvents(_THIS);
}

extern TBool isCursorVisible;

