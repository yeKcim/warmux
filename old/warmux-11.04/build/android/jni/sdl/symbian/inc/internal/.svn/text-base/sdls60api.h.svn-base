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
    sdlepocapi.cpp
    
    S60 application framework integration utilities.

    Markus Mertama
    
    
    When porting SDL to S60, CSDL should make it
    very easy.- see c2doomexe sources for example. 
    
*/

#ifndef EPOC_SDL_H
#define EPOC_SDL_H

#include<e32std.h>

class CDesC8Array;
class TWsEvent;
class RWindow;
class CAknAppUi;
class RWsSession;
class CWsScreenDevice;
class CBitmapContext;
class CFbsBitmap;

//internal stuff
extern "C"
    {
    typedef int (*mainfunc1) ();
    typedef int (*mainfunc2) (int argc, char* argv[]);
    typedef int (*mainfunc3) (int argc, char* argv[], char* env[]);
    typedef void (*mainfunc4) ();
    typedef void (*mainfunc5) (int argc, char* argv[]);
    typedef void (*mainfunc6) (int argc, char* argv[], char* env[]);
    }
    
 //internal stuff       
extern "C"
    {
    int SDL_main (int argc, char* argv[]);
    }
 
//internal stuff        
NONSHARABLE_CLASS(TMainFunc)
    {
    public:
    	IMPORT_C TMainFunc();
        IMPORT_C TMainFunc(mainfunc1);
        IMPORT_C TMainFunc(mainfunc2);
        IMPORT_C TMainFunc(mainfunc3);
        IMPORT_C TMainFunc(mainfunc4);
        IMPORT_C TMainFunc(mainfunc5);
        IMPORT_C TMainFunc(mainfunc6);
        const void* operator[](TInt aIndex) const;
    private:
    	void* iMainFunc[6];
    };
    
NONSHARABLE_CLASS(MOverlay)
	{
	public:
		virtual void Draw(CBitmapContext& aGc, const TRect& aTargetRect, const TSize& aSize) = 0;
	};

NONSHARABLE_CLASS(MBlitter)
	{
	public:
		virtual TBool BitBlt(CBitmapContext& aGc, CFbsBitmap& aBmp, const TRect& aTargetRect, const TSize& aSize) = 0;	
	};

NONSHARABLE_CLASS(MSDLObserver)
	{
	public:
		enum //these events indicates the state changes in SDL 
			{
			EEventWindowReserved, //window is created, nothing else should use the window
			EEventWindowNotAvailable, //window not available, can be ignored
			EEventScreenSizeChanged, //screen size is changged, SDL_SetVideoMode be should called
			EEventKeyMapInit, //Keymap is inited and can now be rewritten
			};
		enum  //these are fixed return values that event handler sends back to SDL
			{
			EParameterNone,
			EScreenSizeChangedDefaultPalette, //after EEventScreenSizeChanged let palette change to SDL default 
			};
		virtual TInt SdlEvent(TInt aEvent, TInt aParam) = 0; //event is called in application thread	
	};
    
    
class SDL
    {
    public:
    enum TFlags //these flags set SDL modes
    	{
    	ENoFlags		   = 0x0,
    	EEnableFocusStop   = 0x1, //let SDL thread to be stopped if not on top
    	EDrawModeDSB	   = 0x2, //Default drawmode is DSA, but if you request a antitearing this flag may help 
    	EAllowImageResize  = 0x4, //image is resized if requested image does not fit to screen
    	EKeyBlock			= 0x8, //Only once key press at once allowed
    	EDrawModeDSBDoubleBuffer 		= 0xA,
    	EDrawModeDSBIncrementalUpdate 	= 0x12,
    	EAllowImageResizeKeepRatio		= 0x24,	
    	EDrawModeGdi					= 0x40,	//use BITGDI surface instead of Direct Screen Access
    	EDrawModeDSBAsync				= 0x82, //set DSB to drawn asynchrounsuly, all frames may not be viewed
     	};
    enum TOrientationMode
    	{
    	EOrientation0,
    	EOrientation90,
    	EOrientation180,
    	EOrientation270
    	};
      
    IMPORT_C static TInt Init(TInt aFlags = SDL::ENoFlags); 
    IMPORT_C static TInt UnInit(); 
    IMPORT_C static MSDLObserver* Observer(); //get current observer
    IMPORT_C static void SetObserver(MSDLObserver* aObserver);  //set observer to receive events, if NULL then observer is removed
    IMPORT_C static void SetContainerWindowL(RWindow& aWindow, RWsSession& aSession, CWsScreenDevice& aDevice); //the window where drawn happens, should be recalled (propably in application HandleResourceChange) if screen device changes
    IMPORT_C static TInt AppendWsEvent(const TWsEvent& aEvent); //give event to SDL, return error code if event buffer is full (insert in application HandleWsEvent)
	IMPORT_C static TInt GetSDLCode(TInt aScanCode);   		 //get SDL internal key code bind to given scancode, return -1 if scancode is not found, aScancode must be postive number
    IMPORT_C static TInt SetSDLCode(TInt aScanCode, TInt aSDLCode); //set internal SDL code with given key code, return replaced code
    IMPORT_C static TInt SDLCodesCount(); //return count of scancodes
	IMPORT_C static void ResetSDLCodes(); //rest all code replacement to their initial values
    IMPORT_C static void SetOrientation(TOrientationMode aMode); //rotate screen
    IMPORT_C static void Suspend();
    IMPORT_C static void Resume();
    IMPORT_C static TInt SetBlitter(MBlitter* aBlitter); //set an alternative blitting function, useful e.g. when scaling is needed (EAllowImageResize used) and default implementation is too slow. Without scaling default is fast.
    IMPORT_C static TInt AppendOverlay(MOverlay& aOverlay, TInt aPriority);
   	IMPORT_C static TInt RemoveOverlay(MOverlay& aOverlay);
   	IMPORT_C static TInt RedrawRequest();
    inline static void CallMainL(); //call SDL main() function 
    inline static void CallMainL(const CDesC8Array& iArg); //call SDL main(int argc, char* argv[]) function with given parameters
public:
    IMPORT_C void static CallMainL(const TMainFunc& aFunc, const CDesC8Array* const iArg); //internal
protected:
    SDL();
    };
    
    
inline void SDL::CallMainL()
    {
    CallMainL(TMainFunc(SDL_main), NULL);
    }
    
inline void SDL::CallMainL(const CDesC8Array& iArg)  
    {
    CallMainL(TMainFunc(SDL_main), &iArg);   
    }
    
    

        
#endif

