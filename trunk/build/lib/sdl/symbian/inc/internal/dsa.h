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

#ifndef __DSA_H__
#define __DSA_H__

#include <e32base.h>
#include <w32std.h>
#include "epoc_sdl.h"
#include "sdlepocapi.h"

class CDsa;

NONSHARABLE_CLASS(TOverlay) 
	{
	public:
		MOverlay* iOverlay;
		TInt iPriority;
	};

typedef void (*TCopyFunction)(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);




NONSHARABLE_CLASS(CDsa) : public CBase
	{
	public:
		enum
		{
		ERequestUpdate = EpocSdlEnv::ELastService,
		ELastDsaRequest
		};
		
		
		inline TBool IsDsaAvailable() const;
		inline TBool IsHwScreenSurface() const;
        inline const TRect& ScreenRect() const;
        
        inline TBool IsUpdating() const;
        
        TThreadId OwnerThread() const;
   	
		static CDsa* CreateL(RWsSession& aSession);
	
		~CDsa();
   		 	
   		TUint8* LockHwSurface();
   		void UnlockHwSurface();
   		TInt AllocSurface(TBool aHwSurface, const TSize& aSize, TDisplayMode aMode);
   		inline TDisplayMode DisplayMode() const;
   		
   		TInt SetPalette(TInt aFirst, TInt aCount, TUint32* aPalette);
   		void LockPalette(TBool aLock);
   		TBool AddUpdateRect(const TUint8* aBits, const TRect& aUpdateRect, const TRect& aRect);
   		void UpdateSurface();
   		void SetOrientation(CSDL::TOrientationMode aMode);
   		
   		TSize WindowSize() const;
   //		void SetSuspend();
   	
   	
   	//	TBool Stopped() const;
   		RWsSession& Session();
   		RWindow* Window();
   		CDsa* CreateGlesDsaL();
   		
   		
   	//	void DoStop();
   		
   //		void SetStarted();
   		
   		void CreateZoomerL(const TSize& aSize);
   		void SetBlitter(MBlitter* aBlitter);
   		
   		TInt AppendOverlay(MOverlay& aOverlay, TInt aPriority);
   		TInt RemoveOverlay(MOverlay& aOverlay);
   		
   		TPoint WindowCoordinates(const TPoint& aPoint) const;
   	
   		virtual void Free();
   		
   		//TInt RedrawRequest();

   		static TRect CalcResizeRect(const TSize& aBound, const TSize& aRatio);
   		
   		virtual void DisableDraw(TBool aDisable);
   		
   		//CSDL::TOrientationMode Orientation() const;
   		
   //		TInt iCreates;

   	public:
		virtual void ConstructL(RWindow& aWindow, CWsScreenDevice& aDevice);
		virtual void SetUpdating(TBool aUpdate);
		
		virtual TUint8* LockSurface() = 0;
		virtual void UnlockHWSurfaceRequestComplete() = 0;
		virtual void UnlockSurface() = 0;
		virtual void Update() = 0;
	//	virtual void Resume() = 0;
		
		virtual void Stop();
		
   
   	private:
		virtual void CreateSurfaceL() = 0;
		virtual void Wipe() = 0;
		


	private:
		void ClipCopy(TUint8* aTarget, const TUint8* aSource, const TRect& aUpdateRect, const TRect& aSourceRect) const;
		static void Copy256(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopySlow(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopyMem(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void Copy256Flip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopyMemFlip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopySlowFlip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void Copy256Reversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopySlowReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopyMemReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void Copy256FlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopySlowFlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopyMemFlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);

		void SetCopyFunction();
	
	
   	
	protected:

		inline const TSize& SwSize() const;
		inline const TRect& HwRect() const;
		inline TBool IsDrawDisabled() const;
		inline MBlitter* Blitter() const;
	//	virtual CBitmapContext* Gc() = 0;
		
		void DrawOverlays(CBitmapContext& aGc) const;	
		CDsa(RWsSession& aSession);
		void SetTargetRect();
		void Start();
		
	
#ifdef _DEBUG		
		void DsaLockOnD(TInt aLine);
		void DsaLockOffD(TInt aLine);
#else
		void DsaLockOn();
		void DsaLockOff();
#endif		
	
	protected:
		enum
			{
			ERunning 				= 0x1,
			EUpdating  				= 0x2,
			EPaletteLocked 			= 0x4,
			EUsePalette				= 0x8,
			EOrientation90			= 0x10,
			EOrientation180			= 0x20,
			EOrientationFlags 		= 0x30,
			EOrientationChanged 	= 0x40,
		//	ESdlThreadSuspend		= 0x100,
		//	ESdlThreadExplicitStop 	= 0x200,
			EResizeRequest			= 0x400,
			EUseBlit                = 0x800,
			ENoZoomOut              = 0x1000,
			EDisableDraw            = 0x2000,
			EHwSurface              = 0x4000,
//			EStarted                = 0x2000
			};
	

		
	private:	
	
		TInt iStateFlags;	
		RWsSession& iSession;	
        TRect iScreenRect;	

		TInt iTargetBpp;
		TInt iSourceBpp;
      	TDisplayMode iSourceMode;
        TDisplayMode iTargetMode;
        TUint32* iLut256;
        TCopyFunction iCopyFunction;
        TUint8* iTargetAddr;
        TCopyFunction iCFTable[12];
        TInt iNewFlags;
        TSize iSwSize;
        MBlitter* iBlitter;
        TRect iTargetRect;
        RArray<TOverlay> iOverlays;
        RWindow* iWindow;
        TThreadId iOwnerThread;
        
        RFastLock iDsaLock;
        
        friend class TDsa;
	};
	
inline TDisplayMode CDsa::DisplayMode() const
	{
	return iTargetMode;
	}

inline const TRect& CDsa::HwRect() const
	{
	return iTargetRect;
	}
	
inline TBool CDsa::IsUpdating() const
    {
    return iStateFlags & EUpdating;
    }

inline const TSize& CDsa::SwSize() const
	{
	return iSwSize;
	}		
	
inline TBool CDsa::IsDsaAvailable() const
	{
	return (iStateFlags & ERunning) /*&& !(iStateFlags & EWaitDeviceChange)*/;
	}
	
inline TThreadId CDsa::OwnerThread() const
    {
    return iOwnerThread;
    }
	
inline const TRect& CDsa::ScreenRect() const
	{
	return iTargetRect;//iScreenRect;		
	}

inline MBlitter* CDsa::Blitter() const
    {
    return iBlitter;
    }

inline TBool CDsa::IsDrawDisabled() const
    {
    return iStateFlags & EDisableDraw;
    }

inline TBool CDsa::IsHwScreenSurface() const
    {
    return iStateFlags & EHwSurface;
    }



#endif	
		
////////////////////////////////////////////////////////////////////////////////////////////////

