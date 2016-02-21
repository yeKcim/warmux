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
    epoc sdl.cpp
    Symbian OS services for SDL

    Markus Mertama
*/

#ifndef EPOCSDL_H
#define EPOCSDL_H


#include<e32def.h>

#include<e32std.h>
#include<gdi.h>
#include<w32std.h>
#include"SDL_config_symbian.h"
#include<sdlepocapi.h>


//#undef DEBUG_TRACE_ENABLED
#define TRACE_TO_FILE
#ifdef DEBUG_TRACE_ENABLED
#define SDL_TRACE(error_str) RDebug_Print(error_str, (void*) NULL) 
#define SDL_TRACE1(error_str, param) RDebug_Print(error_str, (void*) (param)) 
#define SDL_MEM() SDL_TRACE1("Free memory=%d", Debug_AvailMem()) 
extern "C"
    {
    void RDebug_Print(char* error_str, void* param);
    void RDebug_Print_b(char* error_str, void* param);
    int Debug_AvailMem();
    }
#else
#define SDL_TRACE(error_str) 
#define SDL_TRACE1(error_str, param) 
#define SDL_MEM()
#endif

#define PANIC(x) Panic(x, __LINE__)
#define PANIC_IF_ERROR(x)  (x == KErrNone || Panic(x, __LINE__))

TInt Panic(TInt aErr, TInt aLine);

const TInt KPointerBufferSize = 32;

NONSHARABLE_CLASS(MEventQueue)
        {
        public:
            virtual TInt Append(const TWsEvent& aEvent) = 0;
           // virtual const TWsEvent& Top() = 0;
            virtual TBool HasData() = 0;
           // virtual TInt Shift() = 0;
           virtual const TWsEvent& Shift() = 0;
       	   virtual void Lock() = 0;
       	   virtual void Unlock() = 0;
        };
 

//class CEikonEnv;

typedef void (*TSdlCleanupOperation) (TAny* aThis);

NONSHARABLE_CLASS(TSdlCleanupItem)
    {
    public:
        TSdlCleanupItem(TSdlCleanupOperation aOperation, TAny* aItem, TBool aThreadCleanup = ETrue);
    public:
        TSdlCleanupOperation iOperation;
        TAny* iItem;
        TThreadId iThread;
    };

 
NONSHARABLE_CLASS(EpocSdlEnv)
    {
    public:
    enum {EDisableKeyBlocking = 100, ESetAppOrientation, ELastService};
    static void Request(TInt aService);
    static MEventQueue& EventQueue();
    static void CreateZoomer(const TSize& aRequestedSize);
   	static TSize WindowSize();
    static TDisplayMode DisplayMode();
   	static TPointerCursorMode PointerMode();
    static TBool Flags(TInt aFlag);
    static TInt Argc();
    static char** Argv();
    static TBool IsDsaAvailable();
   // static void Dsa(const TPoint& aPoint, const CFbsBitmap& aBmp);
    static TInt AllocSwSurface(const TSize& aSize, TDisplayMode aMode);
    static TInt AllocHwSurface(const TSize& aSize, TDisplayMode aMode);
    static void UnlockHwSurface();
    static TUint8* LockHwSurface(); 
    static void UpdateSwSurface();
    static TBool AddUpdateRect(TUint8* aAddress, const TRect& aUpdateRect, const TRect& aTargetRect);
    static void PanicMain(TInt aErr);
    static void PanicMain(const TDesC& aInfo, TInt aErr);
  //  static void WaitDeviceChange();
    static TInt SetPalette(TInt aFirstcolor, TInt aColorCount, TUint32* aPalette);
    static void ObserverEvent(TInt aService, TInt aParam = 0);
    static void WaitDsaAvailable();
    static void LockPalette(TBool aLock);
    static void DoResume(TBool aInternal, TBool aRendezvous = EFalse);
    static void DoSuspend(TBool aInternal);
    static TInt AppendCleanupItem(const TSdlCleanupItem& aItem);
    static void RemoveCleanupItem(TAny* aItem);
    static void CleanupItems();
 //  static void SetWaitDsa();
    static TPoint WindowCoordinates(const TPoint& aPoint);
    static void FreeSurface();
    static TInt ApplyGlesDsa();
    static RWindow* Window();
    static void SetOrientation(CSDL::TOrientationMode);
    static TBool AutoOrientation(const TSize& aSize);
    static TInt SetAppOrientation(CSDL::TAppOrientation);
    static TSize ScreenSize();
    static void PostUpdate();
    static TInt StackSize();
    static TBool IsVideoThread();
 //   static void ScreenSizeChanged();
    static void EnableDraw();
    static void DisableDraw();
    static void ResumeDsa();
    static TUint32 BgColor();
    static void RemoveThread(TInt aThreadId);
    static void AppendThread(TInt aThreadId);
    static TBool IsSuspend();
    static void ScreenSizeChanged();
    static void UpdateHwSurface();
   //  static CSDL::TAppOrientation AppOrientation();
   // static TInt SetVolume(TInt aVolume);
   // static TInt Volume();
   // static TInt MaxVolume();
    }; 
    
#endif


