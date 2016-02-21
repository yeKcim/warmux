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
	S60 Adaptation Markus Mertama
	2003 - 2008
*/

#ifndef EPOCVIDEO_H
#define EPOCVIDEO_H

#include <w32std.h>
#include <egltypes.h>

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *_this
#define Private	_this->hidden

class CFbsBitmap;

struct SDL_VideoDevice;
void DisableKeyBlocking(SDL_VideoDevice*);

enum 
    {
    EFocusedWindow = 0x1,
  //  EContainerChange = 0x2,
  //  EAutoRotationOn  = 0x4
    };

struct SDL_PrivateVideoData 
    {
    TPoint					iScreenPos;
    TInt                    iFlags;              
    TSize                   iSwSurfaceSize;
    TInt                    iSwSurfaceDepth;
    TUint8*                 iSwSurface;
    SDL_Rect                iUpdateRect;
    SDL_Rect				iRect; //same info in SDL format
    SDL_Rect* 				iRectPtr;
    int 					null; //terminator							
    };
    
struct SDL_PrivateGLData
	{
	EGLDisplay	iDisplay;
	EGLConfig	iConfig;
	EGLSurface	iContext;
	EGLSurface	iSurface;	
	RLibrary	iLibrary;
	};    
    
#endif    




