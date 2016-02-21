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
    SDL_epocvideo.cpp
    Epoc based SDL video driver implementation

    Markus Mertama
*/



#include "epoc_sdl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include "SDL_error.h"
#include "SDL_timer.h"
#include "SDL_video.h"
#undef NULL
#include "SDL_pixels_c.h"
#include "SDL.h"
#include "SDL_mouse.h"
}

#include "SDL_epocvideo.h"
#include "SDL_epocevents_c.h"



#include <coedef.h>
#include <flogger.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include "sdlepocapi.h"
#include <SDL_gliop.h>
#include <egl.h>

#include "appsrv.h"
#ifdef WINS
#include "gles_wincw_def.h"
#else
#include "gles_armv5_def.h"
#endif


//extern TBuf<64> gLastError;

////////////////////////////////////////////////////////////////

#ifdef DEBUG_TRACE_ENABLED

#ifdef TRACE_TO_FILE
_LIT(KLibName, "SDL");
#endif



void RDebug_Print_b(char* error_str, void* param)
    {
    TBuf8<128> error8((TUint8*)error_str);
    TBuf<128> error;
    error.Copy(error8);

#ifndef TRACE_TO_FILE
    if (param) //!! Do not work if the parameter is really 0!!
        RDebug::Print(error, param);
    else 
        RDebug::Print(error);
#else
    if (param) //!! Do not work if the parameter is really 0!!
        RFileLogger::WriteFormat(KLibName, _L("SDL.txt"), EFileLoggingModeAppend, error, param);
    else 
        RFileLogger::Write(KLibName, _L("SDL.txt"), EFileLoggingModeAppend, error);
#endif

    }

extern "C" void RDebug_Print(char* error_str, void* param)
    {
    RDebug_Print_b(error_str, param);
    }
    
#endif //DEBUG_TRACE_ENABLED

/*
int Debug_AvailMem2()
    {
    //User::CompressAllHeaps();
    TMemoryInfoV1Buf membuf; 
    User::LeaveIfError(UserHal::MemoryInfo(membuf));
    TMemoryInfoV1 minfo = membuf();
	return(minfo.iFreeRamInBytes);
    }

extern "C" int Debug_AvailMem()
    {
    return(Debug_AvailMem2());
    }
    
*/

extern "C" {

/* Initialization/Query functions */

static int VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect** ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface* SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int SetColors(_THIS, int firstcolor, int ncolors,
			  SDL_Color *colors);
static void VideoQuit(_THIS);

/* Hardware surface functions */

static int AllocHWSurface(_THIS, SDL_Surface *surface);
static int LockHWSurface(_THIS, SDL_Surface *surface);
static int FlipHWSurface(_THIS, SDL_Surface *surface);
static void UnlockHWSurface(_THIS, SDL_Surface *surface);
static void FreeHWSurface(_THIS, SDL_Surface *surface);
static void DirectUpdate(_THIS, int numrects, SDL_Rect *rects);

static int Available(void);
static SDL_VideoDevice *CreateDevice(int devindex);

void DrawBackground(_THIS);
void DirectDraw(_THIS, int numrects, SDL_Rect *rects, TUint16* screenBuffer);
void DirectDrawRotated(_THIS, int numrects, SDL_Rect *rects, TUint16* screenBuffer);

/* Mouse functions */

static WMcursor* CreateWMCursor(_THIS, Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);
static void FreeWMCursor(_THIS, WMcursor *cursor);
static int ShowWMCursor(_THIS, WMcursor *cursor);

/*GL Functions*/
static int GlesLoadLibrary(_THIS, const char* path);
static void* GlesGetProcAddress(_THIS, const char *proc);
static int GlesGetAttribute(_THIS, SDL_GLattr attrib, int* value);
static int GlesMakeCurrent(_THIS);
static void GlesSwapBuffers(_THIS);
}



extern "C"
	{
	struct WMcursor
		{
		};
	}

/* Epoc video driver bootstrap functions */


static int Available(void)
    {
    return 1; /* Always available */
    }

static void DeleteDevice(SDL_VideoDevice *device)
    {
    User::Free(device->gl_data);
		User::Free(device->hidden);
		User::Free(device);
    }

static SDL_VideoDevice* CreateDevice(int /*devindex*/)
    {
	SDL_VideoDevice *device;

	SDL_TRACE("SDL:S60_CreateDevice");

	/* Allocate all variables that we free on delete */
	device = static_cast<SDL_VideoDevice*>(User::Alloc(sizeof(SDL_VideoDevice)));
	if (device) 
	    {
		Mem::FillZ(device, (sizeof *device));
		device->hidden = static_cast<struct SDL_PrivateVideoData*>
				(User::Alloc((sizeof *device->hidden)));
		device->gl_data = static_cast<struct SDL_PrivateGLData*>
				(User::Alloc((sizeof (SDL_PrivateGLData))));
	    }
	 else
	 	{
	 	SDL_OutOfMemory();
	 	return 0;	
	 	}
	if((device->hidden == NULL)  ||  (device->gl_data == NULL))
	    {
		SDL_OutOfMemory();
		User::Free(device->hidden);
		User::Free(device->gl_data);
		User::Free(device);
		return(0);
		}
		
	Mem::FillZ(device->hidden, (sizeof *device->hidden));

	/* Set the function pointers */
	device->VideoInit = VideoInit;
	device->ListModes = ListModes;
	device->SetVideoMode = SetVideoMode;
	device->SetColors = SetColors;
	device->UpdateRects = DirectUpdate;
	device->VideoQuit = VideoQuit;
	device->AllocHWSurface = AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = LockHWSurface;
	device->UnlockHWSurface = UnlockHWSurface;
	device->FlipHWSurface = FlipHWSurface;
	device->FreeHWSurface = FreeHWSurface;
	device->SetIcon = NULL;
	device->SetCaption = NULL;
	device->GetWMInfo = NULL;
	device->FreeWMCursor = FreeWMCursor;
	device->CreateWMCursor = CreateWMCursor;
	device->ShowWMCursor = ShowWMCursor;
	device->WarpWMCursor = NULL;
	device->InitOSKeymap = InitOSKeymap;
	device->PumpEvents = PumpEvents;
	device->free = DeleteDevice;
	
	/*gles funtions*/
	
	device->GL_LoadLibrary 		= GlesLoadLibrary;
   	device->GL_GetProcAddress	= GlesGetProcAddress;
   	device->GL_GetAttribute 	= GlesGetAttribute;
   	device->GL_MakeCurrent 		= GlesMakeCurrent;
   	device->GL_SwapBuffers 		= GlesSwapBuffers;
   		
	device->gl_data->iLibrary.SetHandle(0);

	return device;
}


VideoBootStrap EPOC_bootstrap = {
	"epoc\0\0\0", "EPOC system",
    Available, CreateDevice
};



void DisableKeyBlocking(_THIS)
    {
    EpocSdlEnv::Request(EpocSdlEnv::EDisableKeyBlocking);
    }


int VideoInit(_THIS, SDL_PixelFormat *vformat)
	{
    /* Construct Epoc window */


    /* Initialise Epoc frame buffer */

	DisableKeyBlocking(_this); //disable key blocking
  
    const TDisplayMode displayMode = EpocSdlEnv::DisplayMode();

    /* The "best" video format should be returned to caller. */

    vformat->BitsPerPixel 	= TDisplayModeUtils::NumDisplayModeBitsPerPixel(displayMode);
    vformat->BytesPerPixel  = TDisplayModeUtils::NumDisplayModeBitsPerPixel(displayMode) / 8;

    

    Private->iScreenPos = TPoint(0, 0);
    const TSize sz = EpocSdlEnv::ScreenSize();
    
    Private->iRect.x = Private->iScreenPos.iX;
    Private->iRect.y = Private->iScreenPos.iY;
    
    
    if (sz.iHeight > sz.iWidth)
        {
        Private->iRect.h = sz.iWidth;
	    Private->iRect.w = sz.iHeight;
    	}
    else
        {
        Private->iRect.w = sz.iWidth;
        Private->iRect.h = sz.iHeight;
        }
	Private->iRectPtr = &Private->iRect;

	/*_this->info->hw_available    = 0;   
	_this->info->wm_available      = 0; 
	_this->info->blit_hw           = 0; 
	_this->info->blit_hw_CC        = 0; 
	_this->info->blit_hw_A         = 0; 
	_this->info->blit_sw           = 0; 
	_this->info->blit_sw_CC        = 0;
	_this->info-> blit_sw_A        = 0; 
	_this->info->blit_fill         = 0; 
	_this->info->video_mem         = 0;  */
	
	_this->info.current_w = sz.iWidth; 
	_this->info.current_h = sz.iHeight;  
	
	return(0);
	}


SDL_Rect **ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
	{
    /*
	if(flags & SDL_HWSURFACE)
		{
		if(format->BytesPerPixel != 4) //in HW only full color is supported
			return NULL;
		}
	*/
	if(flags & SDL_FULLSCREEN)
		{
		return &Private->iRectPtr;
		}
	
    return (SDL_Rect **)(-1); //everythingisok, but too small shoes
	}


int SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
	{
	if ((firstcolor + ncolors) > 256)
		return -1;
	TUint32 palette[256];
	const TDisplayMode mode = EpocSdlEnv::DisplayMode();
    if(TDisplayModeUtils::NumDisplayModeColors(mode) == 4096)
        {
	// Set 12 bit palette
        for(int i = 0 ; i < ncolors; i++)
            {
	        // 4k value: 0000 rrrr gggg bbbb
	        TUint32 color4K	 = (colors[i].r & 0x0000f0) << 4;
	        color4K			|= (colors[i].g & 0x0000f0);      
	        color4K			|= (colors[i].b & 0x0000f0) >> 4;
            palette[i + firstcolor] = color4K;
            }
        }
    else if(TDisplayModeUtils::NumDisplayModeColors(mode) == 65536)
        {
        for(int i = 0; i < ncolors; i++)
            {
			// 64k-colour displays effectively support RGB values
			// with 5 bits allocated to red, 6 to green and 5 to blue
			// 64k value: rrrr rggg gggb bbbb
	        TUint32 color64K = (colors[i].r & 0x0000f8) << 8;
	        color64K		|= (colors[i].g & 0x0000fc) << 3;
	        color64K		|= (colors[i].b & 0x0000f8) >> 3;
            palette[i + firstcolor] = color64K;
            }
        }
    else if(TDisplayModeUtils::NumDisplayModeColors(mode) == 16777216)
        {
        for(int i = 0; i < ncolors; i++)
            {
			// 16M-colour
            //0000 0000 rrrr rrrr gggg gggg bbbb bbbb
	        TUint32 color16M = colors[i].r << 16;
	        color16M		|= colors[i].g << 8;
	        color16M		|= colors[i].b;
            palette[i + firstcolor] = color16M;
            }
        }
    else
        {
        return -2;
        }
    if(EpocSdlEnv::SetPalette(firstcolor, ncolors, palette) == KErrNone)
    	return 0;
	return -1;
	}





int GlesLoadLibrary(SDL_VideoDevice* _this, const char* path)
	{
	if(_this->gl_data->iLibrary.Handle() != 0)
		return KErrAlreadyExists; //already loaded
	const char* const gles_lib[] = {"libgles_cm.dll", "libSWGLES.dll"};
	TInt err = KErrNotFound;
	for(TInt i = 0; i < 2 && err != KErrNone; i++)
		{
		const char* name8 = path == NULL ? gles_lib[i] : path;
		TFileName lib;
		lib.Copy(TPtrC8((unsigned char*)name8));
		err = _this->gl_data->iLibrary.Load(lib); 
		}
	if(err == KErrNone)
		_this->gl_config.driver_loaded = 1;
	return err;
	}
	
typedef TInt (*Ftp)(...);	
#define DC(x) ((Ftp) GlesGetProcAddress(_this, #x))	

const char* const OpenGL[] = //these funtions are in gl, but not in gles, at least in all in all versions...
	{
	"glBegin",
	"glEnd",
	"glOrtho",
	"glPopAttrib",
	"glPopClientAttrib",
	"glPushAttrib",
	"glPushClientAttrib",
	"glTexCoord2f",
	"glVertex2i",
	"glTexParameteri"
	};
	
TInt NotSupported()
	{
	User::Panic(_L("SDL, Gles"), KErrNotSupported);
	return 0;
	}
		
void* GlesGetProcAddress(_THIS, const char *proc)
	{
	if(_this->gl_data->iLibrary.Handle() == 0)
		return NULL; //not loaded
	TLibraryFunction f = NULL;
	for(TInt i = 0; i < G_ordinals_count; i++)
		{
		if(strcmp(G_ordinals[i].name, proc) == 0)
			{
			f = _this->gl_data->iLibrary.Lookup(G_ordinals[i].ord);
			break;
			}
		}
	
	if(f != NULL) /*Lookup may fail*/
		return  (void*) f;	
		
	for(TInt i = 0; i < sizeof(OpenGL) / sizeof(char*); i++)
		{
		if(strcmp(OpenGL[i], proc) == 0)
			return (void*) NotSupported;
		}
		
	return NULL;
	}
	
		

int GlesGetAttribute(_THIS, SDL_GLattr aAttrib, int* aValue)
	{
	EGLint attrib = 0;
	switch(aAttrib)
		{
		/*todo*/
		case SDL_GL_RED_SIZE: attrib = EGL_RED_SIZE; break;
	    case SDL_GL_GREEN_SIZE: attrib = EGL_GREEN_SIZE; break;
	    case SDL_GL_BLUE_SIZE:attrib = EGL_BLUE_SIZE; break;
	    case SDL_GL_ALPHA_SIZE: attrib = EGL_ALPHA_SIZE; break;
	    case SDL_GL_BUFFER_SIZE: attrib = EGL_BUFFER_SIZE; break;
	    case SDL_GL_DOUBLEBUFFER: *aValue = 1; return 0; //always
	    case SDL_GL_DEPTH_SIZE: attrib = EGL_DEPTH_SIZE; break;
	    case SDL_GL_STENCIL_SIZE: attrib = EGL_STENCIL_SIZE; break;
	    case SDL_GL_ACCUM_RED_SIZE: 
	    case SDL_GL_ACCUM_GREEN_SIZE:
	    case SDL_GL_ACCUM_BLUE_SIZE:
	    case SDL_GL_ACCUM_ALPHA_SIZE:
	    case SDL_GL_STEREO:
	    case SDL_GL_MULTISAMPLEBUFFERS:
	    case SDL_GL_MULTISAMPLESAMPLES:
	    case SDL_GL_ACCELERATED_VISUAL:
	    case SDL_GL_SWAP_CONTROL:
	    	*aValue = 0;
	    	return -1;
		}
	const int success = DC(eglGetConfigAttrib)
		(
		_this->gl_data->iDisplay,
		_this->gl_data->iConfig,
		attrib,
		aValue);
	return success == EGL_FALSE ? -1 : 0;
	}
	

int GlesMakeCurrent(_THIS)
	{
	DC(eglMakeCurrent)
	(_this->gl_data->iDisplay,
	_this->gl_data->iSurface,
	_this->gl_data->iSurface,
	_this->gl_data->iContext);
	return DC(eglGetError)();
	}

void GlesSwapBuffers(_THIS)
	{
	DC(eglSwapBuffers)(
		_this->gl_data->iDisplay,
		_this->gl_data->iSurface);
	}

TDisplayMode GetDisplayMode(TInt aBitsPerPixel)
	{
	const TDisplayMode displayMode = EpocSdlEnv::DisplayMode();
	TInt dmode = EColorLast;
	if(
	TDisplayModeUtils::NumDisplayModeBitsPerPixel(displayMode)
	== aBitsPerPixel)
		{
		dmode = displayMode;
		}
	else
		{
		--dmode;
		while(TDisplayModeUtils::IsDisplayModeColor(TDisplayMode(dmode)) &&
			TDisplayModeUtils::NumDisplayModeBitsPerPixel(TDisplayMode(dmode)) !=
			aBitsPerPixel)
			--dmode;
		}
	return TDisplayMode(dmode);
	}
	

LOCAL_C void glAssert(_THIS)
	{
	const EGLint err = DC(eglGetError)();
	if(err != EGL_SUCCESS)
		{
		User::Leave(err);
		}
	}

LOCAL_C void CreateGles(_THIS, RWindow& aWindow, int aBpp, SDL_PrivateGLData& aData)	
	{		
	SDL_GL_LoadLibrary(NULL); //just if its not already loaded
	aData.iDisplay = DC(eglGetDisplay)(EGL_DEFAULT_DISPLAY);
	DC(eglInitialize)(aData.iDisplay, NULL, NULL);
	
	glAssert(_this);
	
	int configs = 0;
	EGLConfig* configList = NULL;
	int configSz = 0;
	DC(eglGetConfigs)(aData.iDisplay, configList, configSz, &configs);
	configSz = configs; 
	
	glAssert(_this);
	
	configList = (EGLConfig*) User::Alloc(sizeof(EGLConfig) * configSz);

#ifdef __WIN32__	
	const TDisplayMode mode = aWindow.DisplayMode();
	const TInt sz = TDisplayModeUtils::NumDisplayModeBitsPerPixel(mode);
	aBpp;
#else
	TInt red, green, blue;	
	if(aBpp == 16)
		{
		red = 5;
		green = 6;
		blue = 5;	
		}
	else
		{ 
		red = 8;
		green = 8;
		blue = 8;
		}
#endif		
		
	const EGLint attribList[] = 
		{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
#ifdef __WIN32__
		EGL_BUFFER_SIZE, sz,
#else
		EGL_RED_SIZE, red,
		EGL_GREEN_SIZE, green, 
		EGL_BLUE_SIZE, blue,
		EGL_BUFFER_SIZE, EGL_DONT_CARE,
#endif 
		EGL_DEPTH_SIZE, 8,
	//	EGL_ALPHA_SIZE, 0,
		EGL_NONE
		};
	
	DC(eglChooseConfig)(aData.iDisplay,
		 attribList,
		 configList,
		 configSz,
		 &configs);
			 
		 
	glAssert(_this);
	
	__ASSERT_ALWAYS(configs > 0, User::Invariant());
	
	aData.iConfig = configList[0];
		 
	User::Free(configList);	 
		 
	aData.iContext = DC(eglCreateContext)(aData.iDisplay,
		 	aData.iConfig,
		  	EGL_NO_CONTEXT,
		  	NULL);
		  	
	glAssert(_this);
		  	
	aData.iSurface	= DC(eglCreateWindowSurface)(aData.iDisplay,
		 aData.iConfig,
		 &aWindow, 
		 NULL);
		 
	glAssert(_this);
		 	
	}
	

LOCAL_C void DestroyGles(_THIS)
	{
	if(	_this->gl_config.driver_loaded)
		{
		DC(eglMakeCurrent)(_this->gl_data->iDisplay, 
		EGL_NO_SURFACE,
		EGL_NO_SURFACE,
		EGL_NO_CONTEXT);
		DC(eglDestroySurface)(_this->gl_data->iDisplay, _this->gl_data->iSurface);
		DC(eglDestroyContext)(_this->gl_data->iDisplay, _this->gl_data->iContext);
		DC(eglTerminate)(_this->gl_data->iDisplay);
		_this->gl_data->iLibrary.Close();
		_this->gl_config.driver_loaded = 0;
		}
	}


SDL_Surface *SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
	{

	if(EpocSdlEnv::Flags(CSDL::EAutoOrientation))
	        {
	      //  current_video->hidden->iFlags |= EAutoRotationOn;
	        EpocSdlEnv::AutoOrientation(TSize(width, height));
	      //  current_video->hidden->iFlags &= ~EAutoRotationOn;
	        }	    
 
		
   	if(flags & SDL_OPENGL)
   		{
   	//	current_video->hidden->iFlags &= ~EContainerChange;
   		
   		current->flags |= SDL_OPENGL;
   		current->w = width;
   		current->h = height;
   			
		RWindow* win = EpocSdlEnv::Window();
		EpocSdlEnv::ApplyGlesDsa();	
				
		CreateGles(_this, *win, bpp, *_this->gl_data);		
		
		const TSize sz(width, height);
		const TInt param = reinterpret_cast<TInt>(&sz);
		EpocSdlEnv::ObserverEvent(MSDLObserver::EEventScreenSurfaceCreated, param);

   		return current;
   		}
   	
   	if(EpocSdlEnv::Flags(CSDL::EAllowImageResize))
   	    EpocSdlEnv::CreateZoomer(TSize(width, height));
   	
   	const TSize screenSize = EpocSdlEnv::WindowSize();
   		
	if(width > screenSize.iWidth || height > screenSize.iHeight || width <= 0 || height <= 0)
	    {
	    if(flags & SDL_FULLSCREEN)
	        {
	        width = screenSize.iWidth;
	        height = screenSize.iHeight;
	        }
	    else    
		    return NULL;
	    }

    if(current && current->pixels)
    	{
        current->pixels = NULL;
    	}
    	
	if(!SDL_ReallocFormat(current, bpp, 0, 0, 0, 0))
	 	{
		return(NULL);
	 	}

	current->flags = 0;
	if(width == screenSize.iWidth && height == screenSize.iHeight)
		current->flags |= SDL_FULLSCREEN;
	
	const int numBytesPerPixel = ((bpp-1)>>3) + 1;   
	current->pitch = numBytesPerPixel * width; // Number of bytes in scanline 

    /* Set up the new mode framebuffer */
   	current->flags |= SDL_PREALLOC;
   	
   	if(bpp <= 8)
   		current->flags |= SDL_HWPALETTE;
   	
   	User::Free(Private->iSwSurface);
   	current->pixels = NULL;
   	Private->iSwSurface = NULL;
   	
   	if(flags & SDL_HWSURFACE)
   	    {
   	    const TInt err = EpocSdlEnv::AllocHwSurface(TSize(width, height), GetDisplayMode(current->format->BitsPerPixel));
   	    current->flags |= SDL_HWSURFACE;
   	    current->pixels = NULL;
   	    }
   	else
   	    {
   	    if(!(current->flags & SDL_SWSURFACE) ||
   	            Private->iSwSurfaceSize != TSize(width, height) ||
   	            Private->iSwSurfaceDepth != numBytesPerPixel)
   	        {
   	        current->flags |= SDL_SWSURFACE;
   	        const TInt surfacesize = width * height * numBytesPerPixel;  
   	        Private->iSwSurfaceSize = TSize(width, height);
   	        Private->iSwSurfaceDepth = numBytesPerPixel;
   	        delete Private->iSwSurface;
   	        Private->iSwSurface = NULL;
   	        current->pixels = (TUint8*) User::Alloc(surfacesize);
   	        Private->iSwSurface = (TUint8*) current->pixels;
   	        const TInt err = EpocSdlEnv::AllocSwSurface
                (TSize(width, height), GetDisplayMode(current->format->BitsPerPixel));
   	        if(err != KErrNone || current->pixels == NULL)
   	            return NULL;
   	        }
   	    }
	
	current->w = width;
	current->h = height;
	
 

    /*
     *  Logic for getting suitable screen dimensions, offset, scaling and orientation
     */


    /* Centralize game window on device screen  */
   
    
    Private->iScreenPos.iX = Max(0, (screenSize.iWidth  - width)  / 2);
    Private->iScreenPos.iY = Max(0, (screenSize.iHeight - height) / 2);

    
    SDL_TRACE1("View width %d", width);
    SDL_TRACE1("View height %d", height);
    SDL_TRACE1("View bmode %d", bpp);
    SDL_TRACE1("View x %d", Private->iScreenPos.iX);
    SDL_TRACE1("View y %d", Private->iScreenPos.iY);
    
    const TPoint pos = EpocSdlEnv::WindowCoordinates(Private->iScreenPos);
    
    Private->iRect.x = pos.iX;
    Private->iRect.y = pos.iY;
    
    const TSize winSz = EpocSdlEnv::WindowSize();
       
    Private->iRect.w = winSz.iWidth;
    Private->iRect.h = winSz.iHeight;


    EpocSdlEnv::EnableDraw();
    EpocSdlEnv::ResumeDsa();
    
	EpocSdlEnv::LockPalette(EFalse);
	
	Private->iFlags |= EFocusedWindow;
	
	const TSize sz(width, height);
	const TInt param = reinterpret_cast<TInt>(&sz);
	EpocSdlEnv::ObserverEvent(MSDLObserver::EEventScreenSurfaceCreated, param);

	
//	current_video->hidden->iFlags &= ~EContainerChange;
	/* We're done */
	return(current);
}



static int AllocHWSurface(_THIS, SDL_Surface* surface)
	{
	if(KErrNone == EpocSdlEnv::AllocHwSurface(TSize(surface->w, surface->h), GetDisplayMode(surface->format->BitsPerPixel)))
	    {
	    Private->iFlags |= EFocusedWindow;
	    return true;
	    }
	return false;
	}
	
static void FreeHWSurface(_THIS, SDL_Surface* /*surface*/)
	{
	}

static int LockHWSurface(_THIS, SDL_Surface* surface)
	{
	if(EpocSdlEnv::IsDsaAvailable())
		{
		TUint8* address = EpocSdlEnv::LockHwSurface();
		if(address != NULL)
			{
			surface->pixels = address;
			return 1;
			}
		}
	return 0;
	}
static void UnlockHWSurface(_THIS, SDL_Surface* /*surface*/)
	{
	EpocSdlEnv::UnlockHwSurface();
	}

static int FlipHWSurface(_THIS, SDL_Surface* /*surface*/)
	{
	if(EpocSdlEnv::IsDsaAvailable())
	    {
	    EpocSdlEnv::Request(CSdlAppServ::EVideoUpdate);
	    return true;
	    }
	return false;
	}

static void RequestVideoUpdate(_THIS, int numrects, SDL_Rect* rects)
    {
    if(numrects > 0)
        {
        Private->iUpdateRect = rects[0];
        for(int i = 1; i < numrects; i++)
            {
            const SDL_Rect& r = rects[0];
            if(r.x < Private->iUpdateRect.x)
                Private->iUpdateRect.x = r.x;
            if((r.x + r.w) >  Private->iUpdateRect.x + Private->iUpdateRect.w)
                Private->iUpdateRect.w = r.w;
            if(r.y < Private->iUpdateRect.y)
                Private->iUpdateRect.y = r.y;
            if((r.y + r.h) >  Private->iUpdateRect.y + Private->iUpdateRect.h)
                Private->iUpdateRect.h = r.h;
            }
        EpocSdlEnv::Request(CSdlAppServ::EVideoUpdate);
        }
    }

static void DirectUpdate(_THIS, int numrects, SDL_Rect *rects)
	{
	if(!EpocSdlEnv::IsVideoThread())
	    {
        RequestVideoUpdate(_this, numrects, rects);
	    return;
	    } 
	
	if(EpocSdlEnv::IsDsaAvailable())
		{
		const TSize screenSize = EpocSdlEnv::WindowSize();
		if(screenSize.iWidth < SDL_VideoSurface->w || screenSize.iHeight <  SDL_VideoSurface->h)
		    return; // video surface dimenstions do not match 
	    	
		if(Private->iSwSurface /*&& 0 == (current_video->hidden->iFlags & EContainerChange)*/)
		    {
		    const TRect target(Private->iScreenPos, Private->iSwSurfaceSize);
		    for(TInt i = 0; i < numrects ;i++)
		    	{
		    	const TRect rect(TPoint(rects[i].x, rects[i].y),
		    		TSize(rects[i].w, rects[i].h));
		    	if(!EpocSdlEnv::AddUpdateRect(Private->iSwSurface, rect, target))
		    	    {
		    		return; //not succesful
		    	    }
		    	}
	        EpocSdlEnv::UpdateSwSurface();
		    }
		else
		    {
		    EpocSdlEnv::UpdateHwSurface();
		    }
		EpocSdlEnv::PostUpdate();
		}
	//gLastError = _L("out du");
	}


/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void VideoQuit(_THIS)
	{
	
	if(_this->gl_data)
		DestroyGles(_this);
	
	User::Free(Private->iSwSurface);
	Private->iSwSurface = NULL;
	}
	

WMcursor *CreateWMCursor(_THIS, Uint8* /*data*/, Uint8* /*mask*/, int /*w*/, int /*h*/, int /*hot_x*/, int /*hot_y*/)
    {
    return (WMcursor*) 1; //hii! prevents SDL to view a std cursor
    }

void FreeWMCursor(_THIS, WMcursor* /*cursor*/)
    {
    }

int ShowWMCursor(_THIS, WMcursor* /*cursor*/)
    {
    return true;  
    }

void PrepareContainerWindowChanged()
    {
 /*   if(current_video)
        {
        current_video->hidden->iFlags |= EContainerChange;
        } */
    }

void ContainerWindowChanged()
    {
    if(current_video && SDL_VideoSurface)
        {
  /*      if((current_video->hidden->iFlags & EContainerChange) &&
            !(current_video->hidden->iFlags & EAutoRotationOn) &&
            (SDL_VideoSurface->flags & SDL_FULLSCREEN))
            {
            const TSize screenSize = EpocSdlEnv::WindowSize();
            if(current_video->info.current_w != screenSize.iWidth || current_video->info.current_h != screenSize.iHeight)
                {
                current_video->info.current_w = screenSize.iWidth;
                current_video->info.current_h = screenSize.iHeight;
                SDL_SetVideoMode(screenSize.iWidth, screenSize.iHeight, 0, SDL_VideoSurface->flags);
                }
            }
        current_video->hidden->iFlags &= ~EContainerChange;*/
        SDL_UpdateRect(SDL_VideoSurface, 0, 0, SDL_VideoSurface->w, SDL_VideoSurface->h);
        }
    }

/*FOR GL comp*/

void glBegin(GLenum /*a*/) {}
void glEnd(void) {}
void glOrtho(GLdouble /*l*/, GLdouble /*r*/, GLdouble /*b*/, GLdouble /*t*/, GLdouble /*n*/, GLdouble /*f*/) {}
void glPopAttrib(void) {}
void glPopClientAttrib(void){}
void glPushAttrib(GLbitfield /*mask*/) {}
void glPushClientAttrib(GLbitfield /*mask*/) {}
void glTexCoord2f(GLfloat /*s*/, GLfloat /*t*/) {}
void glVertex2i(GLint /*x*/, GLint /*y*/) {}

/*
 * Video update goes around problem that video update cannot be called from other thread
 * that it is created. As timer works in its own window SDL_Update rect called from that
 * callback reroutes update request from appserv AO
 * */


void VideoUpdate()
    {
    DirectUpdate(current_video, 1, &current_video->hidden->iUpdateRect);
    }



