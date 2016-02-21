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

#ifndef __SDLMAIN_H__
#define __SDLMAIN_H__

class CApaApplication;

#include<sdlepocapi.h>

NONSHARABLE_CLASS(MSDLMainObs)
	{
public:
	enum TSDLMainObsEvent {EError, ESDLCreated};
	virtual TInt SDLMainEvent(TInt aEvent, TInt aParam, CSDL* aSdl) = 0;
	};

NONSHARABLE_CLASS(SDLEnv)
	{
public:
	enum TSdlEnvFlags
		{
		EFlagsNone            = 0x0, 
		EParamQuery           = 0x1, //request parameters from sdl_param.txt file
		EAllowConsoleView     = 0x2, //printf etc. uses extra console window
		EVirtualMouse		  = 0x4, //YES key enables a virtaul mouse that is controlled using arrow keys
		EParamQueryDialog	          = 0x9, //view parameter query dialog if paramt.txt is not found
		EFastZoomBlitter              = 0x10, //Use faster blitter when zoomin up frames, CSDL::EMainThread and CSDL::EAllowImageResize should be set! (not checked however, if some one writes more flexible blitter :-)
		EEnableVirtualMouseMoveEvents = 0x24, // mouse + move event simulation. When arrow is yellow, move messages are passed
		};
	IMPORT_C static TInt SetMain(const TMainFunc& aFunc, TInt aSdlFlags, MSDLMainObs* aObs = NULL, TInt aEnvFlags = EFlagsNone);
 	};



#endif

