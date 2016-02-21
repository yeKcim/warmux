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
   
    S60 application framework integration utilities.

    Markus Mertama
    
    When porting SDL to S60, CSDL should make it
    very easy.- see c2doomexe sources for example. 
    
*/

#ifndef EPOC_SDL_H
#define EPOC_SDL_H

#include<e32base.h>


class CDesC8Array;
class TWsEvent;
class RWindow;
class CAknAppUi;
class RWsSession;
class CWsScreenDevice;
class CBitmapContext;
class CFbsBitmap;
class TRgb;




//internal stuff
//extern "C"
//    {
    typedef int (*mainfunc1) ();
    typedef int (*mainfunc2) (int argc, char* argv[]);
    typedef int (*mainfunc3) (int argc, char* argv[], char* env[]);
    typedef void (*mainfunc4) ();
    typedef void (*mainfunc5) (int argc, char* argv[]);
    typedef void (*mainfunc6) (int argc, char* argv[], char* env[]);
//    }
    
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
    
/*Overlays add something be drawn on SDL screen Surface  e.g. mouse cursor or virtual keyboard*/
NONSHARABLE_CLASS(MOverlay)
	{
	public:
		virtual void Draw(CBitmapContext& aGc, const TRect& aTargetRect, const TSize& aSize) = 0;
	};

/*If SDL screen Surface does not fit screen its scaled - MBlitter let user use more effient alogortihms*/
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
			EEventScreenSizeChanged, //screen size is changed
			EEventSuspend, //SDL thread will be stopped next
			EEventResume, //SDL thread resumed
			EEventKeyMapInit, //Keymap is inited and can now be rewritten
			EEventMainExit,  //Main thread is about to exit
			EEventVolumeChange, //New audio volume level. Maximum is 256
			EEventScreenSurfaceCreated //SDL screen surface is created
			};

		virtual TInt SdlEvent(TInt aEvent, TInt aParam) = 0; //event is called in application thread	
		virtual TInt SdlThreadEvent(TInt aEvent, TInt aParam) = 0; //event is called in SDL thread
	};
    
    
class CSDL : public CBase
    {
    public:
    enum TFlags //these flags set SDL modes
    	{
    	ENoFlags		   = 0x0,
    	EEnableFocusStop   = 0x1, //let SDL thread to be stopped if not on top
    	EDrawModeDSB	   = 0x2, //Depricated, Default drawmode is DSA, but if you request a antitearing this flag may help 
    	EAllowImageResize  = 0x4, //image is resized if requested image does not fit to screen
    	EDrawModeDSBDoubleBuffer 		= 0xA,  //Depricated
    	EDrawModeDSBIncrementalUpdate 	= 0x12, //Depricated
    	EAllowImageResizeKeepRatio		= 0x24,	
    	EDrawModeGdi					= 0x40,	  //Depricated, use BITGDI surface instead of Direct Screen Access
    	EDrawModeDSBAsync				= 0x82,   //Depricated, set DSB to drawn asynchrounsuly, all frames may not be viewed
     	EOwnThread						= 0x1000, //always put SDL running its own thread. Has issues, plz avoid if possible :-)
     	EMainThread						= 0x2000, //always put SDL in application thread. Default
    	EImageResizeZoomOut             = 0x4000, //Zooms automatically if screen surface is smaller that given window.
        EAutoOrientation                = 0x8000, //set orientation automatically if either of dimension requests to window size, affects only if TAppOrientation is EDefault
        EDisableVolumeKeys              =0x10000, //Volumekeys does not affect to audio           
        };
    
    enum TOrientationMode
    	{
    	EOrientation0,
    	EOrientation90,
    	EOrientation180,
    	EOrientation270
    	};
    enum TParamFlags
    	{
    	ENoParamFlags	= 0x0,
   	 	ERequestResume  = 0x1, //The SDL thread is not running until a RThread::Resume is explicitly called for it 
   	 						   //also if SDL running in same thread
   	 						   //then ERequestResume has no effect
    	};
    
    enum TAppOrientation
        {
        EDefaultAppOrientation = 0,           //system default - SDL default
        ELandscapeAppOrientation,         // set system landcape, overrides EAutoOrientation
        EPortraitAppOrientation           //  set system portrait, overrides EAutoOrientation
        };
    
    IMPORT_C static CSDL* NewL(TInt aFlags = CSDL::ENoFlags); 
    IMPORT_C MSDLObserver* Observer(); //get current observer
    IMPORT_C void SetObserver(MSDLObserver* aObserver);  //set observer to receive events, if NULL then observer is removed
    IMPORT_C void SetContainerWindowL(RWindow& aWindow, RWsSession& aSession, CWsScreenDevice& aDevice); //the window where drawn happens, should be recalled (propably in application HandleResourceChange) if screen device changes
    IMPORT_C void DisableKeyBlocking(CAknAppUi& aAppUi) const;
    IMPORT_C void SetAppOrientation(CAknAppUi& aAppUi, TAppOrientation aAppOrientation); //Set application orientation 
    IMPORT_C ~CSDL();
    IMPORT_C TInt AppendWsEvent(const TWsEvent& aEvent); //give event to SDL, return error code if event buffer is full (insert in application HandleWsEvent)
	IMPORT_C TInt GetSDLCode(TInt aScanCode);   		 //get SDL internal key code bind to given scancode, return -1 if scancode is not found, aScancode must be postive number
    IMPORT_C TInt SetSDLCode(TInt aScanCode, TInt aSDLCode); //set internal SDL code with given key code, return replaced code
    IMPORT_C TInt SDLCodesCount() const; //return count of scancodes
	IMPORT_C void ResetSDLCodes(); //rest all code replacement to their initial values
    IMPORT_C void SetOrientation(TOrientationMode aMode); //rotate screen and arrow keys - does not affect to screen dimensions.
    IMPORT_C void Resume(); //Resumes SDL
    IMPORT_C void Suspend(); //Stops SDL
    IMPORT_C TInt SetBlitter(MBlitter* aBlitter); //set an alternative blitting function, useful e.g. when scaling is needed (EAllowImageResize used) and default implementation is too slow. Without scaling default is fast.
    IMPORT_C TInt AppendOverlay(MOverlay& aOverlay, TInt aPriority);
   	IMPORT_C TInt RemoveOverlay(MOverlay& aOverlay);
   	IMPORT_C TInt RedrawRequest(); //Asks surface blit refresh
   	
   	IMPORT_C void ChangeBgColor(TRgb& aColor); //in new color - out old color. The color that is used for Wipe and background if SDL screen surface does not fill whole screen
   	IMPORT_C void SuspendRequestsLC(TInt aRunFirst = CActive::EPriorityIdle); //Prevents SDL stray signals, for mixing SDL and Symbian AOs.   
    	
    inline TThreadId CallMainL(TRequestStatus& aStatus, TInt aFlags = CSDL::ENoParamFlags, TInt aStackSize = KDefaultStackSize); //call SDL main() function 
    inline TThreadId CallMainL(TRequestStatus& aStatus, const CDesC8Array& iArg, TInt aFlags = CSDL::ENoParamFlags, TInt aStackSize = KDefaultStackSize); //call SDL main(int argc, char* argv[]) function with given parameters
    inline TThreadId CallMainL(TInt aFlags = CSDL::ENoParamFlags, TInt aStackSize = KDefaultStackSize); //call SDL main() function 
    inline TThreadId CallMainL(const CDesC8Array& iArg, TInt aFlags = CSDL::ENoParamFlags, TInt aStackSize = KDefaultStackSize); //call SDL main(int argc, char* argv[]) function with given parameters
    IMPORT_C void SDLPanic(const TDesC& aInfo, TInt aErr); //Panic from SDL thread (see SDLThreadEvent)
    public:
    IMPORT_C TThreadId CallMainL(const TMainFunc& aFunc, TRequestStatus* const aStatus, const CDesC8Array* const iArg, TInt aFlags, TInt aStackSize); //internal
    IMPORT_C TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1); 
    //this is a pretty useless function, just example of using Extension_ 
    inline TInt BuildDate(TDes& aDes) const;
  //  inline TInt BitGdiCanvas() const;
    
    protected:
    IMPORT_C CSDL();
    };
    
    
inline TThreadId CSDL::CallMainL(TRequestStatus& aStatus, TInt aFlags, TInt aStackSize)
    {
    return CallMainL(TMainFunc(SDL_main), &aStatus, NULL, aFlags, aStackSize);
    }
    
inline TThreadId CSDL::CallMainL(TRequestStatus& aStatus, const CDesC8Array& iArg, TInt aFlags, TInt aStackSize)  
    {
    return CallMainL(TMainFunc(SDL_main), &aStatus, &iArg, aFlags, aStackSize);   
    }
    
    
inline TThreadId CSDL::CallMainL(TInt aFlags, TInt aStackSize)
    {
    return CallMainL(TMainFunc(SDL_main), NULL, NULL, aFlags, aStackSize);
    }
    
inline TThreadId CSDL::CallMainL(const CDesC8Array& iArg, TInt aFlags, TInt aStackSize)  
    {
    return CallMainL(TMainFunc(SDL_main), NULL, &iArg, aFlags, aStackSize);   
    }

const TInt KSDLExtensionBuildDate   = 0x5544;
//const TInt KSDLExtensionBitGdiCanvas =  0x5545;

inline TInt CSDL::BuildDate(TDes& aDes) const
    {
    TAny* null = NULL;
    return const_cast<CSDL*>(this)->Extension_(KSDLExtensionBuildDate, null, static_cast<TAny*>(&aDes));
    }




#endif


