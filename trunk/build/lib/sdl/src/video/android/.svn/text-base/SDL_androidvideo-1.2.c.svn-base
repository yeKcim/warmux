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
#include "SDL_config.h"

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_pixels.h"
#include "SDL_video-1.3.h"
#include "SDL_surface.h"
#include "SDL_androidvideo.h"

#include <jni.h>
#include <android/log.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <string.h> // for memset()
#include <dlfcn.h>

#define _THIS	SDL_VideoDevice *this

//#define DEBUGOUT(...)
#define DEBUGOUT(...) __android_log_print(ANDROID_LOG_INFO, "libSDL", __VA_ARGS__)

static int ANDROID_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **ANDROID_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *ANDROID_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int ANDROID_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void ANDROID_VideoQuit(_THIS);

static int ANDROID_AllocHWSurface(_THIS, SDL_Surface *surface);
static int ANDROID_LockHWSurface(_THIS, SDL_Surface *surface);
static void ANDROID_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void ANDROID_FreeHWSurface(_THIS, SDL_Surface *surface);
static int ANDROID_FlipHWSurface(_THIS, SDL_Surface *surface);
static void ANDROID_GL_SwapBuffers(_THIS);
static void ANDROID_PumpEvents(_THIS);
static int ANDROID_CheckHWBlit(_THIS, SDL_Surface *src, SDL_Surface *dst);
static int ANDROID_FillHWRect(_THIS, SDL_Surface *dst, SDL_Rect *rect, Uint32 color);
static int ANDROID_SetHWColorKey(_THIS, SDL_Surface *surface, Uint32 key);
static int ANDROID_SetHWAlpha(_THIS, SDL_Surface *surface, Uint8 value);
static void* ANDROID_GL_GetProcAddress(_THIS, const char *proc);
static void ANDROID_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

// Multithreaded video support for speed optimization
static int ANDROID_VideoInitMT(_THIS, SDL_PixelFormat *vformat);
static SDL_Surface *ANDROID_SetVideoModeMT(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static void ANDROID_VideoQuitMT(_THIS);
static void ANDROID_UpdateRectsMT(_THIS, int numrects, SDL_Rect *rects);
static int ANDROID_FlipHWSurfaceMT(_THIS, SDL_Surface *surface);


// Stubs to get rid of crashing in OpenGL mode
// The implementation dependent data for the window manager cursor
struct WMcursor {
    int unused ;
};

void ANDROID_FreeWMCursor(_THIS, WMcursor *cursor) {
    SDL_free (cursor);
    return;
}
WMcursor * ANDROID_CreateWMCursor(_THIS, Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y) {
    WMcursor * cursor;
    cursor = (WMcursor *) SDL_malloc (sizeof (WMcursor)) ;
    if (cursor == NULL) {
        SDL_OutOfMemory () ;
        return NULL ;
    }
    return cursor;
}
int ANDROID_ShowWMCursor(_THIS, WMcursor *cursor) {
    return 1;
}
void ANDROID_WarpWMCursor(_THIS, Uint16 x, Uint16 y)
{
	SDL_ANDROID_WarpMouse(x, y);
}
//void ANDROID_MoveWMCursor(_THIS, int x, int y) { }

#if 0
# define SDL_NUMMODES 12
#else
# define SDL_NUMMODES 1
#endif

static SDL_Rect *SDL_modelist[SDL_NUMMODES+1];

//#define SDL_modelist		(this->hidden->SDL_modelist)

enum { ANDROID_BYTESPERPIXEL = 2, ANDROID_BITSPERPIXEL = 16 };
typedef struct SDL_Texture private_hwdata;

// Pointer to in-memory video surface
int SDL_ANDROID_sFakeWindowWidth = 640;
int SDL_ANDROID_sFakeWindowHeight = 480;
int sdl_opengl = 0;
static SDL_Window *SDL_VideoWindow = NULL;
SDL_Surface *SDL_CurrentVideoSurface = NULL;
static int HwSurfaceCount = 0;
static SDL_Surface ** HwSurfaceList = NULL;
void * glLibraryHandle = NULL;

static Uint32 SDL_VideoThreadID = 0;
int SDL_ANDROID_InsideVideoThread()
{
	return SDL_VideoThreadID == SDL_ThreadID();
}


/* ANDROID driver bootstrap functions */

static int ANDROID_Available(void)
{
	return 1;
}

static void ANDROID_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *ANDROID_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, sizeof(SDL_VideoDevice));
	} else {
		SDL_OutOfMemory();
		return(0);
	}

	/* Set the function pointers */
	device->VideoInit = SDL_ANDROID_VideoMultithreaded ? ANDROID_VideoInitMT : ANDROID_VideoInit;
	device->ListModes = ANDROID_ListModes;
	device->SetVideoMode = SDL_ANDROID_VideoMultithreaded ? ANDROID_SetVideoModeMT : ANDROID_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = ANDROID_SetColors;
	device->UpdateRects = SDL_ANDROID_VideoMultithreaded ? ANDROID_UpdateRectsMT : ANDROID_UpdateRects;
	device->VideoQuit = SDL_ANDROID_VideoMultithreaded ? ANDROID_VideoQuitMT : ANDROID_VideoQuit;
	device->AllocHWSurface = ANDROID_AllocHWSurface;
	device->CheckHWBlit = ANDROID_CheckHWBlit;
	device->FillHWRect = ANDROID_FillHWRect;
	device->SetHWColorKey = ANDROID_SetHWColorKey;
	device->SetHWAlpha = ANDROID_SetHWAlpha;
	device->LockHWSurface = ANDROID_LockHWSurface;
	device->UnlockHWSurface = ANDROID_UnlockHWSurface;
	device->FlipHWSurface = SDL_ANDROID_VideoMultithreaded ? ANDROID_FlipHWSurfaceMT : ANDROID_FlipHWSurface;
	device->FreeHWSurface = ANDROID_FreeHWSurface;
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = ANDROID_InitOSKeymap;
	device->PumpEvents = ANDROID_PumpEvents;
	device->GL_SwapBuffers = ANDROID_GL_SwapBuffers;
	device->GL_GetProcAddress = ANDROID_GL_GetProcAddress;
	device->free = ANDROID_DeleteDevice;
	device->WarpWMCursor = ANDROID_WarpWMCursor;

	// Stubs
	device->FreeWMCursor = ANDROID_FreeWMCursor;
	device->CreateWMCursor = ANDROID_CreateWMCursor;
	device->ShowWMCursor = ANDROID_ShowWMCursor;

	glLibraryHandle = dlopen("libGLESv1_CM.so", RTLD_NOW);
	
	return device;
}

VideoBootStrap ANDROID_bootstrap = {
	"android", "SDL android video driver",
	ANDROID_Available, ANDROID_CreateDevice
};

int ANDROID_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	int i;
	static SDL_PixelFormat alphaFormat;

	/* Determine the screen depth (use default 16-bit depth) */
	/* we change this during the SDL_SetVideoMode implementation... */
	if( vformat ) {
		vformat->BitsPerPixel = ANDROID_BITSPERPIXEL;
		vformat->BytesPerPixel = ANDROID_BYTESPERPIXEL;
	}

	int bpp;
	SDL_memset(&alphaFormat, 0, sizeof(alphaFormat));
	SDL_PixelFormatEnumToMasks( SDL_PIXELFORMAT_RGBA4444, &bpp,
								&alphaFormat.Rmask, &alphaFormat.Gmask, 
								&alphaFormat.Bmask, &alphaFormat.Amask );
	alphaFormat.BitsPerPixel = ANDROID_BITSPERPIXEL;
	alphaFormat.BytesPerPixel = ANDROID_BYTESPERPIXEL;
	this->displayformatalphapixel = &alphaFormat;

	this->info.hw_available = 1;
	this->info.blit_hw = 1;
	this->info.blit_hw_CC = 1;
	this->info.blit_hw_A = 1;
	this->info.blit_fill = 1;
	this->info.video_mem = 128 * 1024; // Random value
	this->info.current_w = SDL_ANDROID_sWindowWidth;
	this->info.current_h = SDL_ANDROID_sWindowHeight;
	SDL_VideoThreadID = SDL_ThreadID();

	for ( i=0; i<SDL_NUMMODES; ++i ) {
		SDL_modelist[i] = SDL_malloc(sizeof(SDL_Rect));
		SDL_modelist[i]->x = SDL_modelist[i]->y = 0;
	}
	/* Modes sorted largest to smallest */
	SDL_modelist[0]->w = SDL_ANDROID_sWindowWidth;
	SDL_modelist[0]->h = SDL_ANDROID_sWindowHeight;
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "First video mode is %dx%d",
                      SDL_ANDROID_sWindowWidth, SDL_ANDROID_sWindowHeight );
#if SDL_NUMMODES > 1
	SDL_modelist[1]->w = 800; SDL_modelist[1]->h = 600; // Will likely be shrinked
	SDL_modelist[2]->w = 640; SDL_modelist[2]->h = 480; // Will likely be shrinked
	SDL_modelist[3]->w = 640; SDL_modelist[3]->h = 400; // Will likely be shrinked
	SDL_modelist[4]->w = 320; SDL_modelist[4]->h = 240; // Always available on any screen and any orientation
	SDL_modelist[5]->w = 320; SDL_modelist[5]->h = 200; // Always available on any screen and any orientation
	SDL_modelist[6]->w = 256; SDL_modelist[6]->h = 224; // For REminiscence
	SDL_modelist[7]->w = SDL_ANDROID_sWindowWidth * 2 / 3;
	SDL_modelist[7]->h = SDL_ANDROID_sWindowHeight * 2 / 3;
	SDL_modelist[8]->w = SDL_ANDROID_sWindowWidth / 2;
	SDL_modelist[8]->h = SDL_ANDROID_sWindowHeight / 2;
	SDL_modelist[9]->w = 480; SDL_modelist[9]->h = 320; // Virtual wide-screen mode
	SDL_modelist[10]->w = 800; SDL_modelist[10]->h = 480; // Virtual wide-screen mode
	SDL_modelist[11]->w = 544; SDL_modelist[11]->h = 332; // I have no idea where this videomode is used
#endif
	SDL_modelist[SDL_NUMMODES+1] = NULL;
	
	SDL_VideoInit_1_3(NULL, 0);
	
	/* We're done! */
	return(0);
}

SDL_Rect **ANDROID_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
	if(format->BitsPerPixel != ANDROID_BITSPERPIXEL)
		return NULL;
	return SDL_modelist;
}

SDL_Surface *ANDROID_SetVideoMode(_THIS, SDL_Surface *current,
				                          int width, int height, int bpp, Uint32 flags)
{
	SDL_PixelFormat format;
	int bpp1;
	
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "SDL_SetVideoMode(): application requested mode %dx%d OpenGL %d HW %d", width, height, flags & SDL_OPENGL, flags & SDL_HWSURFACE);
	if( ! SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return NULL;
	}

	sdl_opengl = (flags & SDL_OPENGL) ? 1 : 0;

	SDL_ANDROID_sFakeWindowWidth = width;
	SDL_ANDROID_sFakeWindowHeight = height;

	current->flags = (flags & SDL_FULLSCREEN) | (flags & SDL_OPENGL) | SDL_DOUBLEBUF | ( flags & SDL_HWSURFACE );
	current->w = width;
	current->h = height;
	current->pitch = SDL_ANDROID_sFakeWindowWidth * ANDROID_BYTESPERPIXEL;
	current->pixels = NULL;
	current->hwdata = NULL;

	HwSurfaceCount = 0;
	HwSurfaceList = NULL;
	DEBUGOUT("ANDROID_SetVideoMode() HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);
	
	if( ! sdl_opengl )
	{
		SDL_DisplayMode mode;
		SDL_RendererInfo SDL_VideoRendererInfo;
		
		SDL_SelectVideoDisplay(0);
		SDL_VideoWindow = SDL_CreateWindow("", 0, 0, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);

		SDL_memset(&mode, 0, sizeof(mode));
		mode.format = SDL_PIXELFORMAT_RGB565;
		SDL_SetWindowDisplayMode(SDL_VideoWindow, &mode);
		
		if (SDL_CreateRenderer(SDL_VideoWindow, -1, 0) < 0) {
			__android_log_print(ANDROID_LOG_INFO, "libSDL", "SDL_SetVideoMode(): Error creating renderer");
			return NULL;
		}
		SDL_GetRendererInfo(&SDL_VideoRendererInfo);
		
		current->hwdata = NULL;
		if( ! (flags & SDL_HWSURFACE) )
		{
			current->pixels = SDL_malloc(width * height * ANDROID_BYTESPERPIXEL);
			if ( ! current->pixels ) {
				__android_log_print(ANDROID_LOG_INFO, "libSDL", "Couldn't allocate buffer for requested mode");
				SDL_SetError("Couldn't allocate buffer for requested mode");
				return(NULL);
			}
			SDL_memset(current->pixels, 0, width * height * ANDROID_BYTESPERPIXEL);
			current->hwdata = (struct private_hwdata *)SDL_CreateTexture(SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, width, height);
			if( !current->hwdata ) {
				__android_log_print(ANDROID_LOG_INFO, "libSDL", "Couldn't allocate texture for SDL_CurrentVideoSurface");
				SDL_free(current->pixels);
				current->pixels = NULL;
				SDL_OutOfMemory();
				return(NULL);
			}
			if( SDL_ANDROID_SmoothVideo )
				SDL_SetTextureScaleMode((SDL_Texture *)current->hwdata, SDL_SCALEMODE_SLOW);

			// Register main video texture to be recreated when needed
			HwSurfaceCount++;
			HwSurfaceList = SDL_realloc( HwSurfaceList, HwSurfaceCount * sizeof(SDL_Surface *) );
			HwSurfaceList[HwSurfaceCount-1] = current;
			DEBUGOUT("ANDROID_SetVideoMode() HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);
		}
		glViewport(0, 0, SDL_ANDROID_sRealWindowWidth, SDL_ANDROID_sRealWindowHeight);
		glOrthof(0.0, (GLfloat) SDL_ANDROID_sWindowWidth, (GLfloat) SDL_ANDROID_sWindowHeight, 0.0, 0.0, 1.0);
	}

	/* Allocate the new pixel format for the screen */
  SDL_memset(&format, 0, sizeof(format));
	SDL_PixelFormatEnumToMasks( SDL_PIXELFORMAT_RGB565, &bpp1,
								&format.Rmask, &format.Gmask,
								&format.Bmask, &format.Amask );
	format.BitsPerPixel = bpp1;
	format.BytesPerPixel = ANDROID_BYTESPERPIXEL;

	if ( ! SDL_ReallocFormat(current, ANDROID_BITSPERPIXEL, format.Rmask, format.Gmask, format.Bmask, format.Amask) ) {
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Couldn't allocate new pixel format for requested mode");
		SDL_SetError("Couldn't allocate new pixel format for requested mode");
		return(NULL);
	}

	/* Set up the new mode framebuffer */
	SDL_CurrentVideoSurface = current;

	/* We're done */
	return(current);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void ANDROID_VideoQuit(_THIS)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
	}

	if( ! sdl_opengl )
	{
		DEBUGOUT("ANDROID_VideoQuit() in HwSurfaceCount %d HwSurfaceList %p video %p",
             HwSurfaceCount, HwSurfaceList, SDL_CurrentVideoSurface);
		HwSurfaceCount = 0;
		if(HwSurfaceList)
			SDL_free(HwSurfaceList);
		HwSurfaceList = NULL;
		DEBUGOUT("ANDROID_VideoQuit() out HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);

		if( SDL_CurrentVideoSurface )
		{
			if( SDL_CurrentVideoSurface->hwdata )
				SDL_DestroyTexture((struct SDL_Texture *)SDL_CurrentVideoSurface->hwdata);
			if( SDL_CurrentVideoSurface->pixels )
				SDL_free(SDL_CurrentVideoSurface->pixels);
			SDL_CurrentVideoSurface->pixels = NULL;
		}
		SDL_CurrentVideoSurface = NULL;

		//if(SDL_VideoWindow)
		//	SDL_DestroyWindow(SDL_VideoWindow);
		SDL_VideoWindow = NULL;
	}

	SDL_ANDROID_sFakeWindowWidth = 0;
	SDL_ANDROID_sFakeWindowWidth = 0;

	int i;
	
	/* Free video mode lists */
	for ( i=0; i<SDL_NUMMODES; ++i ) {
		if ( SDL_modelist[i] != NULL ) {
			SDL_free(SDL_modelist[i]);
			SDL_modelist[i] = NULL;
		}
	}
}

void ANDROID_PumpEvents(_THIS)
{
	SDL_ANDROID_PumpEvents();
}

static int ANDROID_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}

	if ( ! (surface->w && surface->h) )
		return(-1);

	DEBUGOUT("ANDROID_AllocHWSurface() surface %p w %d h %d", surface, surface->w, surface->h);
	Uint32 format = SDL_PIXELFORMAT_RGBA5551; // 1-bit alpha for color key, every surface will have colorkey so it's easier for us
	if( surface->format->Amask )
	{
		SDL_PixelFormat format1;
		int bpp;
		format = SDL_PIXELFORMAT_RGBA4444;
		DEBUGOUT("ANDROID_AllocHWSurface() SDL_PIXELFORMAT_RGBA4444");
	    SDL_memset(&format1, 0, sizeof(format1));
		SDL_PixelFormatEnumToMasks( format, &bpp,
									&format1.Rmask, &format1.Gmask,
									&format1.Bmask, &format1.Amask );
		if( surface->format->BitsPerPixel != bpp ||
			surface->format->Rmask != format1.Rmask ||
			surface->format->Gmask != format1.Gmask ||
			surface->format->Bmask != format1.Bmask ||
			surface->format->Amask != format1.Amask )
			return(-1); // Do not allow alpha-surfaces with format other than RGBA4444 (it will be pain to lock/copy them)
	}
	else
	{
		DEBUGOUT("ANDROID_AllocHWSurface() SDL_PIXELFORMAT_RGBA5551");
		// HW-accel surface should be RGB565
		if( !( SDL_CurrentVideoSurface->format->BitsPerPixel == surface->format->BitsPerPixel &&
			SDL_CurrentVideoSurface->format->Rmask == surface->format->Rmask &&
			SDL_CurrentVideoSurface->format->Gmask == surface->format->Gmask &&
			SDL_CurrentVideoSurface->format->Bmask == surface->format->Bmask &&
			SDL_CurrentVideoSurface->format->Amask == surface->format->Amask ) )
			return(-1);
	}

	surface->pitch = surface->w * surface->format->BytesPerPixel;
	surface->pixels = SDL_malloc(surface->h * surface->w * surface->format->BytesPerPixel);
	if ( surface->pixels == NULL ) {
		SDL_OutOfMemory();
		return(-1);
	}
	SDL_memset(surface->pixels, 0, surface->h*surface->pitch);

	surface->hwdata = (struct private_hwdata *)SDL_CreateTexture(format, SDL_TEXTUREACCESS_STATIC, surface->w, surface->h);
	if( !surface->hwdata ) {
		SDL_free(surface->pixels);
		surface->pixels = NULL;
		SDL_OutOfMemory();
		return(-1);
	}

	if( SDL_ANDROID_SmoothVideo )
		SDL_SetTextureScaleMode((SDL_Texture *)surface->hwdata, SDL_SCALEMODE_SLOW);

	if( surface->format->Amask )
	{
		SDL_SetTextureAlphaMod((struct SDL_Texture *)surface->hwdata, SDL_ALPHA_OPAQUE);
		SDL_SetTextureBlendMode((struct SDL_Texture *)surface->hwdata, SDL_BLENDMODE_BLEND);
	}
	
	surface->flags |= SDL_HWSURFACE | SDL_HWACCEL;
	

	HwSurfaceCount++;
	DEBUGOUT("ANDROID_AllocHWSurface() in HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);
	HwSurfaceList = SDL_realloc( HwSurfaceList, HwSurfaceCount * sizeof(SDL_Surface *) );
	DEBUGOUT("ANDROID_AllocHWSurface() out HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);

	HwSurfaceList[HwSurfaceCount-1] = surface;
	
	return 0;
}

static void ANDROID_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	int i;

	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return;
	}

  if( !HwSurfaceList )
  {
	  DEBUGOUT("ANDROID_FreeHWSurface() surface %p w %d h %d with list empty",
             surface, surface->w, surface->h);
    return;
  }

	if( !surface || !surface->hwdata )
		return;
	SDL_DestroyTexture((struct SDL_Texture *)surface->hwdata);

	DEBUGOUT("ANDROID_FreeHWSurface() surface %p w %d h %d in HwSurfaceCount %d HwSurfaceList %p", surface, surface->w, surface->h, HwSurfaceCount, HwSurfaceList);

	for( i = 0; i < HwSurfaceCount; i++ )
	{
		if( HwSurfaceList[i] == surface )
		{
			HwSurfaceCount--;
			memmove(HwSurfaceList + i, HwSurfaceList + i + 1, sizeof(SDL_Surface *) * (HwSurfaceCount - i) );
			HwSurfaceList = SDL_realloc( HwSurfaceList, HwSurfaceCount * sizeof(SDL_Surface *) );
			i = -1;
			DEBUGOUT("ANDROID_FreeHWSurface() in HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);
			break;
		}
	}
	if( i != -1 )
	{
		SDL_SetError("ANDROID_FreeHWSurface: cannot find freed HW surface in HwSurfaceList array");
	}
}

static int ANDROID_LockHWSurface(_THIS, SDL_Surface *surface)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}

	if( surface == SDL_CurrentVideoSurface )
	{
		// Copy pixels from pixelbuffer to video surface - this is slow!
		Uint16 * row = NULL;
		int fakeH = SDL_ANDROID_sFakeWindowHeight, fakeW = SDL_ANDROID_sFakeWindowWidth;
		int realH = SDL_ANDROID_sWindowHeight, realW = SDL_ANDROID_sWindowWidth;
		int x, y;
		if( ! SDL_CurrentVideoSurface->pixels )
		{
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			SDL_CurrentVideoSurface->pixels = SDL_malloc(SDL_ANDROID_sFakeWindowWidth * SDL_ANDROID_sFakeWindowHeight * ANDROID_BYTESPERPIXEL);
			if ( ! SDL_CurrentVideoSurface->pixels ) {
				__android_log_print(ANDROID_LOG_INFO, "libSDL", "Couldn't allocate buffer for SDL_CurrentVideoSurface");
				SDL_SetError("Couldn't allocate buffer for SDL_CurrentVideoSurface");
				return(-1);
			}
		}
		if( ! SDL_CurrentVideoSurface->hwdata )
		{
			SDL_CurrentVideoSurface->hwdata = (struct private_hwdata *)SDL_CreateTexture(SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, SDL_ANDROID_sFakeWindowWidth, SDL_ANDROID_sFakeWindowHeight);
			if( !SDL_CurrentVideoSurface->hwdata ) {
				__android_log_print(ANDROID_LOG_INFO, "libSDL", "Couldn't allocate texture for SDL_CurrentVideoSurface");
				SDL_OutOfMemory();
				return(-1);
			}
			if( SDL_ANDROID_SmoothVideo )
				SDL_SetTextureScaleMode((SDL_Texture *)SDL_CurrentVideoSurface->hwdata, SDL_SCALEMODE_SLOW);
			// Register main video texture to be recreated when needed
			HwSurfaceCount++;
			HwSurfaceList = SDL_realloc( HwSurfaceList, HwSurfaceCount * sizeof(SDL_Surface *) );
			HwSurfaceList[HwSurfaceCount-1] = SDL_CurrentVideoSurface;
			DEBUGOUT("ANDROID_SetVideoMode() HwSurfaceCount %d HwSurfaceList %p", HwSurfaceCount, HwSurfaceList);
		}

		row = SDL_stack_alloc(Uint16, SDL_ANDROID_sWindowWidth);

		for(y=0; y<fakeH; y++)
		{
			glReadPixels(0, realH - 1 - (realH * y / fakeH),
							realW, 1, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, row);
			for(x=0; x<fakeW; x++)
				((Uint16 *)SDL_CurrentVideoSurface->pixels)[ fakeW * y + x ] = row[ x * fakeW / realW ];
		}
		
		SDL_stack_free(row);
	}

	if( !surface->hwdata )
		return(-1);

	// Extra check not necessary
	/*
	if( SDL_CurrentVideoSurface->format->BitsPerPixel == surface->format->BitsPerPixel &&
		SDL_CurrentVideoSurface->format->Rmask == surface->format->Rmask &&
		SDL_CurrentVideoSurface->format->Gmask == surface->format->Gmask &&
		SDL_CurrentVideoSurface->format->Bmask == surface->format->Bmask &&
		SDL_CurrentVideoSurface->format->Amask == surface->format->Amask )
		return(0);

	if( this->displayformatalphapixel->BitsPerPixel == surface->format->BitsPerPixel &&
		this->displayformatalphapixel->Rmask == surface->format->Rmask &&
		this->displayformatalphapixel->Gmask == surface->format->Gmask &&
		this->displayformatalphapixel->Bmask == surface->format->Bmask &&
		this->displayformatalphapixel->Amask == surface->format->Amask )
		return(0);
	return(-1);
	*/
	
	return(0);
}

static void ANDROID_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	SDL_PixelFormat format;
	Uint32 hwformat = SDL_PIXELFORMAT_RGBA5551;
	int bpp;
	SDL_Surface * converted = NULL;

	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return;
	}

	if( !surface->hwdata )
		return;
	
	if( surface->format->Amask )
		hwformat = SDL_PIXELFORMAT_RGBA4444;
		
	if( surface == SDL_CurrentVideoSurface ) // Special case
		hwformat = SDL_PIXELFORMAT_RGB565;
	
		/* Allocate the new pixel format for the screen */
    SDL_memset(&format, 0, sizeof(format));
	SDL_PixelFormatEnumToMasks( hwformat, &bpp,
								&format.Rmask, &format.Gmask,
								&format.Bmask, &format.Amask );
	format.BytesPerPixel = ANDROID_BYTESPERPIXEL;
	format.BitsPerPixel = bpp;
	
	if( format.BitsPerPixel == surface->format->BitsPerPixel &&
		format.Rmask == surface->format->Rmask &&
		format.Gmask == surface->format->Gmask &&
		format.Bmask == surface->format->Bmask &&
		format.Amask == surface->format->Amask )
	{
		DEBUGOUT("ANDROID_UnlockHWSurface() no conversion");
		converted = surface; // No need for conversion
	}
	else
	{
		Uint16 x, y;

		converted = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, format.BitsPerPixel,
											format.Rmask, format.Gmask, format.Bmask, format.Amask);
		if( !converted ) {
			SDL_OutOfMemory();
			return;
		}

		#define CONVERT_RGB565_RGBA5551( pixel ) (0x1 | ( (pixel & 0xFFC0) | ( (pixel & 0x1F) << 1 ) ))
		
		if( surface->flags & SDL_SRCCOLORKEY )
		{
			DEBUGOUT("ANDROID_UnlockHWSurface() CONVERT_RGB565_RGBA5551 + colorkey");
			for( y = 0; y < surface->h; y++ )
			{
				Uint16* src = (Uint16 *)( surface->pixels + surface->pitch * y );
				Uint16* dst = (Uint16 *)( converted->pixels + converted->pitch * y );
				Uint16 w = surface->w;
				Uint16 key = surface->format->colorkey;
				Uint16 pixel;
				for( x = 0; x < w; x++, src++, dst++ )
				{
					pixel = *src;
					*dst = (pixel == key) ? 0 : CONVERT_RGB565_RGBA5551( pixel );
				}
			}
		}
		else
		{
			DEBUGOUT("ANDROID_UnlockHWSurface() CONVERT_RGB565_RGBA5551");
			for( y = 0; y < surface->h; y++ )
			{
				Uint16* src = (Uint16 *)( surface->pixels + surface->pitch * y );
				Uint16* dst = (Uint16 *)( converted->pixels + converted->pitch * y );
				Uint16 w = surface->w;
				Uint16 pixel;
				for( x = 0; x < w; x++, src++, dst++ )
				{
					pixel = *src;
					*dst = CONVERT_RGB565_RGBA5551( pixel );
				}
			}
		}
	}

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = surface->w;
	rect.h = surface->h;
	SDL_UpdateTexture((struct SDL_Texture *)surface->hwdata, &rect, converted->pixels, converted->pitch);

	if( surface == SDL_CurrentVideoSurface ) // Special case
		SDL_RenderCopy((struct SDL_Texture *)SDL_CurrentVideoSurface->hwdata, NULL, NULL);
	
	if( converted != surface )
		SDL_FreeSurface(converted);
}

// We're only blitting HW surface to screen, no other options provided (and if you need them your app designed wrong)
int ANDROID_HWBlit(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}

	if( dst != SDL_CurrentVideoSurface || (! src->hwdata) )
	{
		//__android_log_print(ANDROID_LOG_INFO, "libSDL", "ANDROID_HWBlit(): blitting SW");
		return(src->map->sw_blit(src, srcrect, dst, dstrect));
	}
	if( src == SDL_CurrentVideoSurface )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "ANDROID_HWBlit(): reading from screen surface not supported");
		return(-1);
	}
	
	return SDL_RenderCopy((struct SDL_Texture *)src->hwdata, srcrect, dstrect);
};

static int ANDROID_CheckHWBlit(_THIS, SDL_Surface *src, SDL_Surface *dst)
{
	// This part is ignored by SDL (though it should not be)
	/*
	if( dst != SDL_CurrentVideoSurface || ! src->hwdata )
		return(-1);
	*/
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "ANDROID_CheckHWBlit()");
	src->map->hw_blit = ANDROID_HWBlit;
	src->flags |= SDL_HWACCEL;
	return(0);
};

static int ANDROID_FillHWRect(_THIS, SDL_Surface *dst, SDL_Rect *rect, Uint32 color)
{
	Uint8 r, g, b, a;

	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}

	if( dst != SDL_CurrentVideoSurface )
	{
		// TODO: hack
		current_video->info.blit_fill = 0;
		SDL_FillRect( dst, rect, color );
		current_video->info.blit_fill = 1;
		return(0);
	}
	SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
	SDL_SetRenderDrawColor( r, g, b, a );
	return SDL_RenderFillRect(rect);
};

static int ANDROID_SetHWColorKey(_THIS, SDL_Surface *surface, Uint32 key)
{
	SDL_PixelFormat format;
	SDL_Surface * converted = NULL;

	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}
	
	if( !surface->hwdata )
		return(-1);
	if( surface->format->Amask )
		return(-1);

	surface->flags |= SDL_SRCCOLORKEY;

	ANDROID_UnlockHWSurface(this, surface); // Convert surface using colorkey

	SDL_SetTextureBlendMode((struct SDL_Texture *)surface->hwdata, SDL_BLENDMODE_BLEND);

	return 0;
};

static int ANDROID_SetHWAlpha(_THIS, SDL_Surface *surface, Uint8 value)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}

	if( !surface->hwdata )
		return(-1);

	surface->flags |= SDL_SRCALPHA;

	if( value == SDL_ALPHA_OPAQUE && ! (surface->flags & SDL_SRCCOLORKEY) )
		SDL_SetTextureBlendMode((struct SDL_Texture *)surface->hwdata, SDL_BLENDMODE_NONE);
	else
		SDL_SetTextureBlendMode((struct SDL_Texture *)surface->hwdata, SDL_BLENDMODE_BLEND);
	
	return SDL_SetTextureAlphaMod((struct SDL_Texture *)surface->hwdata, value);
};

static void ANDROID_FlipHWSurfaceInternal()
{
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "ANDROID_FlipHWSurface()");
	if( SDL_CurrentVideoSurface->hwdata && SDL_CurrentVideoSurface->pixels && ! ( SDL_CurrentVideoSurface->flags & SDL_HWSURFACE ) )
	{
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = SDL_CurrentVideoSurface->w;
		rect.h = SDL_CurrentVideoSurface->h;
		SDL_UpdateTexture((struct SDL_Texture *)SDL_CurrentVideoSurface->hwdata, &rect, SDL_CurrentVideoSurface->pixels, SDL_CurrentVideoSurface->pitch);
		SDL_RenderCopy((struct SDL_Texture *)SDL_CurrentVideoSurface->hwdata, &rect, &rect);
		if( SDL_ANDROID_ShowScreenUnderFinger && SDL_ANDROID_ShowScreenUnderFingerRect.w > 0 )
		{
			SDL_RenderCopy((struct SDL_Texture *)SDL_CurrentVideoSurface->hwdata, &SDL_ANDROID_ShowScreenUnderFingerRectSrc, &SDL_ANDROID_ShowScreenUnderFingerRect);
			SDL_Rect frame = SDL_ANDROID_ShowScreenUnderFingerRect;
			// For some reason this code fails - it just outputs nothing to screen
			/*
			SDL_SetRenderDrawColor(0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderFillRect(&SDL_ANDROID_ShowScreenUnderFingerRect);
			SDL_SetRenderDrawColor(255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawRect(&SDL_ANDROID_ShowScreenUnderFingerRectSrc);
			SDL_SetRenderDrawColor(0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawRect(&frame);
			*/
			// Do it old-fashioned way with direct GL calls
			glPushMatrix();
			glLoadIdentity();
			glOrthox( 0, SDL_ANDROID_sFakeWindowWidth * 0x10000, SDL_ANDROID_sFakeWindowHeight * 0x10000, 0, 0, 1 * 0x10000 );
			glColor4x(0, 0, 0, 0x10000);
			glEnableClientState(GL_VERTEX_ARRAY);
			glColor4x(0, 0, 0, 0x10000);
			GLshort vertices[] = {	frame.x, frame.y,
									frame.x + frame.w, frame.y,
									frame.x + frame.w, frame.y + frame.h,
									frame.x, frame.y + frame.h };
			glVertexPointer(2, GL_SHORT, 0, vertices);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
			glDisableClientState(GL_VERTEX_ARRAY);
			glPopMatrix();
			glFlush();
		}
	}
};

static int ANDROID_FlipHWSurface(_THIS, SDL_Surface *surface)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return -1;
	}

	if(!SDL_CurrentVideoSurface)
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s without main video surface!", __PRETTY_FUNCTION__);
		return -1;
	}

	ANDROID_FlipHWSurfaceInternal();

	SDL_ANDROID_CallJavaSwapBuffers();

	return(0);
}

static void ANDROID_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return;
	}

	if(!SDL_CurrentVideoSurface)
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s without main video surface!", __PRETTY_FUNCTION__);
		return;
	}

	ANDROID_FlipHWSurfaceInternal();

	SDL_ANDROID_CallJavaSwapBuffers();
}

void ANDROID_GL_SwapBuffers(_THIS)
{
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "ANDROID_GL_SwapBuffers");
	if( !SDL_ANDROID_InsideVideoThread() )
	{
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "Error: calling %s not from the main thread!", __PRETTY_FUNCTION__);
		return;
	}

	SDL_ANDROID_CallJavaSwapBuffers();
};

int ANDROID_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	return(1);
}

void SDL_ANDROID_VideoContextLost()
{
	if( ! sdl_opengl )
	{
		int i;
		for( i = 0; i < HwSurfaceCount; i++ )
		{
			SDL_DestroyTexture((struct SDL_Texture *)HwSurfaceList[i]->hwdata);
			HwSurfaceList[i]->hwdata = NULL;
		}
	}
};

void SDL_ANDROID_VideoContextRecreated()
{
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "Sending SDL_VIDEORESIZE event %dx%d", SDL_ANDROID_sFakeWindowWidth, SDL_ANDROID_sFakeWindowHeight);
	//SDL_PrivateResize(SDL_ANDROID_sFakeWindowWidth, SDL_ANDROID_sFakeWindowHeight);
	if ( SDL_ProcessEvents[SDL_VIDEORESIZE] == SDL_ENABLE ) {
		SDL_Event event;
		event.type = SDL_VIDEORESIZE;
		event.resize.w = SDL_ANDROID_sFakeWindowWidth;
		event.resize.h = SDL_ANDROID_sFakeWindowHeight;
		if ( (SDL_EventOK == NULL) || (*SDL_EventOK)(&event) ) {
			SDL_PushEvent(&event);
		}
	}

	if( ! sdl_opengl )
	{
		int i;
		SDL_SelectRenderer(SDL_VideoWindow); // Re-apply glOrtho() and blend modes
		// Re-apply our custom 4:3 screen aspect ratio
		glViewport(0, 0, SDL_ANDROID_sRealWindowWidth, SDL_ANDROID_sRealWindowHeight);
		glOrthof(0.0, (GLfloat) SDL_ANDROID_sWindowWidth, (GLfloat) SDL_ANDROID_sWindowHeight, 0.0, 0.0, 1.0);
		for( i = 0; i < HwSurfaceCount; i++ )
		{
			// Allocate HW texture
			Uint32 format = SDL_PIXELFORMAT_RGBA5551; // 1-bit alpha for color key, every surface will have colorkey so it's easier for us
			if( HwSurfaceList[i]->format->Amask )
				format = SDL_PIXELFORMAT_RGBA4444;
			if( HwSurfaceList[i] == SDL_CurrentVideoSurface )
				format = SDL_PIXELFORMAT_RGB565;
			HwSurfaceList[i]->hwdata = (struct private_hwdata *)SDL_CreateTexture(format, SDL_TEXTUREACCESS_STATIC, HwSurfaceList[i]->w, HwSurfaceList[i]->h);
			if( !HwSurfaceList[i]->hwdata )
			{
				SDL_OutOfMemory();
				return;
			}
			if( SDL_ANDROID_SmoothVideo )
				SDL_SetTextureScaleMode((SDL_Texture *)HwSurfaceList[i]->hwdata, SDL_SCALEMODE_SLOW);
			ANDROID_UnlockHWSurface(NULL, HwSurfaceList[i]); // Re-fill texture with graphics
		}
	}
};

static void* ANDROID_GL_GetProcAddress(_THIS, const char *proc)
{
	void * func = dlsym(glLibraryHandle, proc);
	//__android_log_print(ANDROID_LOG_INFO, "libSDL", "ANDROID_GL_GetProcAddress(\"%s\"): %p", proc, func);
	return func;
};

// Multithreaded video - this will free up some CPU time while GPU renders video inside SDL_Flip()
enum videoThreadCmd_t { CMD_INIT, CMD_SETVIDEOMODE, CMD_QUIT, CMD_UPDATERECTS, CMD_FLIP };
typedef struct
{
	SDL_mutex * mutex;
	SDL_cond * cond;
	SDL_cond * cond2;
	int execute;
	int threadReady;
	enum videoThreadCmd_t cmd;
	SDL_VideoDevice *_this;
	SDL_PixelFormat *vformat;
	SDL_Surface *current;
	int width;
	int height;
	int bpp;
	Uint32 flags;
	
	int retcode;
	SDL_Surface * retcode2;
} videoThread_t;
static videoThread_t videoThread;

extern void SDL_ANDROID_MultiThreadedVideoLoopInit();
extern void SDL_ANDROID_MultiThreadedVideoLoop();

void SDL_ANDROID_MultiThreadedVideoLoopInit()
{
	videoThread.mutex = SDL_CreateMutex();
	videoThread.cond = SDL_CreateCond();
	videoThread.cond2 = SDL_CreateCond();
	videoThread.execute = 0;
}

void SDL_ANDROID_MultiThreadedVideoLoop()
{
	while(1)
	{
		int signalNeeded = 0;
		int swapBuffersNeeded = 0;
		SDL_mutexP(videoThread.mutex);
		videoThread.threadReady = 1;
		SDL_CondSignal(videoThread.cond2);
		SDL_CondWaitTimeout(videoThread.cond, videoThread.mutex, 1000);
		if( videoThread.execute )
		{
			videoThread.threadReady = 0;
			switch( videoThread.cmd )
			{
				case CMD_INIT:
					videoThread.retcode = ANDROID_VideoInit(videoThread._this, videoThread.vformat);
					break;
				case CMD_SETVIDEOMODE:
					videoThread.retcode2 = ANDROID_SetVideoMode(videoThread._this, videoThread.current,
							videoThread.width, videoThread.height, videoThread.bpp, videoThread.flags);
					break;
				case CMD_QUIT:
					ANDROID_VideoQuit(videoThread._this);
					break;
				case CMD_UPDATERECTS:
					ANDROID_FlipHWSurfaceInternal();
					swapBuffersNeeded = 1;
					break;
				case CMD_FLIP:
					ANDROID_FlipHWSurfaceInternal();
					swapBuffersNeeded = 1;
					break;
			}
			videoThread.execute = 0;
			signalNeeded = 1;
		}
		SDL_mutexV(videoThread.mutex);
		if( signalNeeded )
			SDL_CondSignal(videoThread.cond2);
		if( swapBuffersNeeded )
		{
			SDL_ANDROID_CallJavaSwapBuffers();
		}
	}
}

int ANDROID_VideoInitMT(_THIS, SDL_PixelFormat *vformat)
{
	SDL_mutexP(videoThread.mutex);
	while( ! videoThread.threadReady )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	videoThread.cmd = CMD_INIT;
	videoThread._this = this;
	videoThread.vformat = vformat;
	videoThread.execute = 1;
	SDL_CondSignal(videoThread.cond);
	while( videoThread.execute )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	int ret = videoThread.retcode;
	SDL_mutexV(videoThread.mutex);
	return ret;
}

SDL_Surface *ANDROID_SetVideoModeMT(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags)
{
	if( flags & SDL_OPENGL || flags & SDL_HWSURFACE )
	{
		return NULL;
	}
	SDL_mutexP(videoThread.mutex);
	while( ! videoThread.threadReady )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	videoThread.cmd = CMD_SETVIDEOMODE;
	videoThread._this = this;
	videoThread.current = current;
	videoThread.width = width;
	videoThread.height = height;
	videoThread.bpp = bpp;
	videoThread.flags = flags;
	videoThread.execute = 1;
	SDL_CondSignal(videoThread.cond);
	while( videoThread.execute )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	SDL_Surface * ret = videoThread.retcode2;
	SDL_mutexV(videoThread.mutex);
	return ret;
}

void ANDROID_VideoQuitMT(_THIS)
{
	SDL_mutexP(videoThread.mutex);
	while( ! videoThread.threadReady )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	videoThread.cmd = CMD_QUIT;
	videoThread._this = this;
	videoThread.execute = 1;
	SDL_CondSignal(videoThread.cond);
	while( videoThread.execute )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	SDL_mutexV(videoThread.mutex);
}

void ANDROID_UpdateRectsMT(_THIS, int numrects, SDL_Rect *rects)
{
	SDL_mutexP(videoThread.mutex);
	while( ! videoThread.threadReady )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	videoThread.cmd = CMD_UPDATERECTS;
	videoThread._this = this;
	videoThread.execute = 1;
	SDL_CondSignal(videoThread.cond);
	while( videoThread.execute )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	SDL_mutexV(videoThread.mutex);
}

int ANDROID_FlipHWSurfaceMT(_THIS, SDL_Surface *surface)
{
	SDL_mutexP(videoThread.mutex);
	while( ! videoThread.threadReady )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	videoThread.cmd = CMD_FLIP;
	videoThread._this = this;
	videoThread.execute = 1;
	SDL_CondSignal(videoThread.cond);
	while( videoThread.execute )
		SDL_CondWaitTimeout(videoThread.cond2, videoThread.mutex, 1000);
	SDL_mutexV(videoThread.mutex);
	return 0;
}
