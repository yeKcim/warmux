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

#include "dsa.h"
#include "sdlenvdata.h"
//#include <cdsb.h>
#include <basched.h>

//extern TBuf<64> gLastError;
#ifdef _DEBUG
#define DsaLockOn() DsaLockOnD(__LINE__) 
#define DsaLockOff() DsaLockOffD(__LINE__)
#endif


LOCAL_C TInt BytesPerPixel(TDisplayMode aMode)
	{
	return ((TDisplayModeUtils::NumDisplayModeBitsPerPixel(aMode) - 1) >> 3) + 1; 
	}


/////////////////////////////////////////////////////////////////////// 

// Accessor class

NONSHARABLE_CLASS(TDsa)
   {
   public:
       inline TDsa(const CDsa& aDsa);
       inline TBool IsFlip() const;
       inline TBool IsTurn() const;
       inline const TSize& SwSize() const;
       inline void Copy(TUint32* aTarget, const TUint8* aSrc, TInt aBytes, TInt aHeight) const;
       inline TBool IsUseBlit() const;
       inline const TRect& ScreenRect() const;
   private:
       const CDsa& iDsa;
   };


inline TDsa::TDsa(const CDsa& aDsa) : iDsa(aDsa)
   {
   __ASSERT_ALWAYS(&iDsa, PANIC(KErrNotReady + 1000));
   }

inline TBool TDsa::IsTurn() const
   {
   return iDsa.iStateFlags & CDsa::EOrientation90;
   }
   
inline TBool TDsa::IsFlip() const
   {
   return iDsa.iStateFlags & CDsa::EOrientation180;
   }   
   
inline const TSize& TDsa::SwSize() const
   {
   return iDsa.SwSize();
   }
       
inline void TDsa::Copy(TUint32* aTarget, const TUint8* aSrc, TInt aBytes, TInt aHeight) const
   {
   iDsa.iCopyFunction(iDsa, aTarget, aSrc, aBytes, aHeight);
   }
   
inline TBool TDsa::IsUseBlit() const
   {
   return iDsa.iStateFlags & CDsa::EUseBlit;
   }
inline const TRect& TDsa::ScreenRect() const
   {
   return iDsa.iScreenRect;
   }

/////////////////////////////////////////////////////

NONSHARABLE_CLASS(CBitmapSurface) : public CDsa
	{
protected:
	CBitmapSurface(RWsSession& aSession);
	CFbsBitmap& Bmp();
	virtual void UpdateRequestCompleted() = 0;
	void CreateSurfaceL();
	void Free();
	~CBitmapSurface();
	virtual CBitmapContext* Gc() = 0;
private:
	TUint8* LockSurface();
	void UnlockSurface();
	void Wipe();
	void Update();
	void CompleteUpdate();
	void UnlockHWSurfaceRequestComplete();

	TBool DoBlt();
    TBool BlitterBlt(CBitmapContext& aGc, CFbsBitmap& aBmp);
private:
	CFbsBitmap* iBmp;
	TBool iUpdateWait;
	
	CFbsBitmapDevice* iOverlayDevice;
	CFbsBitGc* iOverlayGc;
	};
	
	
CBitmapSurface::CBitmapSurface(RWsSession& aSession) : CDsa(aSession)
	{
	}

CFbsBitmap& CBitmapSurface::Bmp()
    {
    return *iBmp;
    }

TBool CBitmapSurface::BlitterBlt(CBitmapContext& aGc, CFbsBitmap& aBmp)
    {
    return Blitter() && Blitter()->BitBlt(aGc, aBmp, HwRect(), SwSize()); 
    }

TBool CBitmapSurface::DoBlt()
    {
    if(!BlitterBlt(*Gc(), Bmp()))
        {
        const TDsa dsa(*this);
        if(dsa.IsUseBlit())
            {
            const TPoint topLeft =  HwRect().iTl; 
            
           const TSize sz = Bmp().SizeInPixels();
           
           __ASSERT_DEBUG(sz.iHeight > 0 && sz.iHeight > 0, PANIC(KErrCorrupt));
              
            Gc()->BitBlt(topLeft, &Bmp());
            }
        else
            Gc()->DrawBitmap(HwRect(), &Bmp());
        return ETrue;
        }
    return EFalse;
    }

void CBitmapSurface::Free()
    {
    if(iBmp != NULL)
        {
        if(IsUpdating())
            {
            SetUpdating(EFalse);
            UnlockHwSurface();
            }
        }
    delete iBmp;
    iBmp = NULL;
    
    delete iOverlayDevice;
    iOverlayDevice = NULL;
    delete iOverlayGc;
    iOverlayGc = NULL;
    }

CBitmapSurface::~CBitmapSurface()
	{
	__ASSERT_DEBUG(iBmp == NULL, PANIC(KErrNotReady));
	}
	
TUint8* CBitmapSurface::LockSurface()
	{
	__ASSERT_ALWAYS(iBmp, PANIC(KErrNotReady));
	iBmp->LockHeap();
	return reinterpret_cast<TUint8*>(iBmp->DataAddress());
	}





void CBitmapSurface::UnlockSurface()
	{
	__ASSERT_ALWAYS(iBmp, PANIC(KErrNotReady));
	iBmp->UnlockHeap();
	}

		
void CBitmapSurface::Update()
	{
    DrawOverlays(*iOverlayGc);
	//const TBool completeNeed = 
    DoBlt();
//	if(completeNeed)
//	    { 
	CompleteUpdate();	
//	    }
	}
		
void CBitmapSurface::CreateSurfaceL()
	{
    __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
	Free();
	if(iBmp == NULL || iBmp->SizeInPixels() != SwSize() || iBmp->DisplayMode() != DisplayMode())
	    {
	    delete iBmp;
	    iBmp = NULL;
	    iBmp  = new (ELeave) CFbsBitmap();
	    User::LeaveIfError(iBmp->Create(SwSize(), DisplayMode()));
	    
	    delete iOverlayDevice;
	    iOverlayDevice = NULL;
	    delete iOverlayGc;
	    iOverlayGc = NULL;
	    
	    iOverlayDevice = CFbsBitmapDevice::NewL(iBmp);    
	    User::LeaveIfError(iOverlayDevice->CreateContext(iOverlayGc));
	    iOverlayGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);

	    }
	}

/**Totally overhead implementation of Wipe :-) **/

typedef TInt (TRgb::*FconvertColor)() const;

template <typename T>
void FillLine(TUint32* aData, const TRgb& aColor, FconvertColor aConvert, TInt aLen)
    {
    T* data = reinterpret_cast<T*>(aData);   
    T color = (aColor.*aConvert)();
    for(TInt i = 0; i < aLen; i++)
        data[i] = color;
    }

void CBitmapSurface::Wipe() //dont call in drawing
	{
	if(iBmp != NULL)
	    {
	    Bmp().LockHeap();
        TUint32* data = Bmp().DataAddress();
     
        const TInt bpp = TDisplayModeUtils::NumDisplayModeBitsPerPixel(Bmp().DisplayMode());
        
        const TInt len = Bmp().DataStride() / 
            (bpp / 8);
       
        FconvertColor convertColor = 0;
        
        TRgb color(EpocSdlEnv::BgColor(), 0xFF);
        
        typedef TUint (TRgb::*FconvertColorU)() const;
        FconvertColorU cc = 0;
        
        switch(Bmp().DisplayMode())
            {
            case EGray2: // convertColor = &TRgb::Gray2; break;
            case EGray4: return;//convertColor = &TRgb::Gray4; break;
            case EGray256: convertColor = &TRgb::Gray256; break;
            case EColor16: convertColor = &TRgb::Color16; break;
            case EColor256: convertColor = &TRgb::Color256; break;
            case EColor64K: convertColor = &TRgb::Color64K; break;
            //case EColor16M: convertColor = &TRgb::Color16M; break;
            //case EColor4K: convertColor = &TRgb::Color4K; break;
            case EColor16MU: convertColor = &TRgb::Color16MU; break;
            case EColor16MA: cc = &TRgb::Color16MA; convertColor = (FconvertColor) cc;  break;     
            case EColor16MAP: cc =  &TRgb::Color16MAP;  convertColor = (FconvertColor) cc; break;
            default: 
                color = KRgbBlack;
                cc = &TRgb::Color16MA;
                convertColor = (FconvertColor) cc;
            }    
          
        for(TInt h = 0; h < Bmp().SizeInPixels().iHeight; h++)
            {
            switch(bpp)
                {
                case 8:
                    FillLine<TUint8>(data, color, convertColor, len);
                    break;
                case 16:
                    FillLine<TUint16>(data, color, convertColor, len);
                    break;
               default:
                    FillLine<TUint32>(data, color, convertColor, len);
                    break;
                }                
            data += len;
            }
	    Bmp().UnlockHeap();
	    }
	}


void CBitmapSurface::CompleteUpdate()
    {
    if(iUpdateWait)
        return;
    EpocSdlEnv::Request(CDsa::ERequestUpdate); 
    iUpdateWait = ETrue;
    }

void CBitmapSurface::UnlockHWSurfaceRequestComplete()
    {
    UpdateRequestCompleted();
    iUpdateWait = EFalse;
    }


//////////////////////////////////////////////////////////////////////

NONSHARABLE_CLASS(CDsaGles) : public CDsa
	{
public:
    CDsaGles(RWsSession& aSession);
private:
    TUint8* LockSurface();	
    void UnlockHWSurfaceRequestComplete();
    void UnlockSurface();
    void Update();
  //  void Resume();
    void CreateSurfaceL();
    void Wipe();
  //  CBitmapContext* Gc();
	};

CDsaGles::CDsaGles(RWsSession& aSession) : CDsa(aSession)
	{
	}

TUint8* CDsaGles::LockSurface()
    {
    return NULL;
    }	
		
void CDsaGles::UnlockHWSurfaceRequestComplete()
	{
	}
	
void CDsaGles::UnlockSurface()
	{
	}

void CDsaGles::Update()
    {
    }
	
/*
void CDsaGles::Resume()
	{
	}
*/	
void CDsaGles::CreateSurfaceL()
	{
	}
	
void CDsaGles::Wipe()
	{
	}

/*
CBitmapContext* CDsaGles::Gc()
	{
	return NULL;
	}
*/

//////////////////////////////////////////////////////////////////////


NONSHARABLE_CLASS(CDsaBitgdi) : public CBitmapSurface
	{
public:
	CDsaBitgdi(RWsSession& aSession);
private:
    void ConstructL(RWindow& aWindow, CWsScreenDevice& aDevice);
    CBitmapContext* Gc();
    ~CDsaBitgdi();
    void CreateSurfaceL();
    void UpdateRequestCompleted();
    void DisableDraw(TBool aDisable);
private:	 
	CFbsBitGc* iOffGc;
	CFbsDevice* iOffDevice;
	CFbsBitmap* iOffCanvas;
	 
	//CFbsBitmap* iBitGdiBmp;
	//CWindowGc* iWinGc;
	 
	RWindow* iWindow;
	TInt iHandle;
	};


CDsaBitgdi::CDsaBitgdi(RWsSession& aSession) : CBitmapSurface(aSession) 
	{
	}


CDsaBitgdi::~CDsaBitgdi()
	{
//	delete iWinGc;
	//delete iBitGdiBmp;
	
	delete iOffGc;
    delete iOffDevice; 
	delete iOffCanvas;
	}


	
void CDsaBitgdi::UpdateRequestCompleted()
	{
	if(iHandle == 0)
		return;
	/*
	if(iBitGdiBmp == NULL)
		{
		iBitGdiBmp = new CFbsBitmap();
		if(iBitGdiBmp == NULL)
			return;
		const TInt err = iBitGdiBmp->Duplicate(iHandle);
		PANIC_IF_ERROR(err);    
		}
	*/
	
	reinterpret_cast<RBackedUpWindow*>(iWindow)->UpdateScreen();
	
//	iWindow->Invalidate();

	/*
	if(iWinGc != NULL)
	    {
	    iWindow->BeginRedraw();
	
	    iWinGc->Activate(*iWindow);
	
	    const TSize sz = Bmp().SizeInPixels();           
	    __ASSERT_DEBUG(sz.iHeight > 0 && sz.iHeight > 0, PANIC(KErrCorrupt));
	         
	
	    iWinGc->BitBlt(TPoint(0, 0), iOffCanvas);
			
	    iWinGc->Deactivate();
	    iWindow->EndRedraw();
	    }
	    */
	}
	
/*
void CDsaBitgdi::Resume()
	{
	if(!IsDsaAvailable())
	    Start();
	}
*/	
	
CBitmapContext* CDsaBitgdi::Gc()
 	{
 	return iOffGc;
 	}


void CDsaBitgdi::DisableDraw(TBool aDisable)
    {
    CDsa::DisableDraw(aDisable);
    }

 void CDsaBitgdi::ConstructL(RWindow& aWindow, CWsScreenDevice& aDevice)
 	{
     __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
 	//delete iBitGdiBmp;
 	//iBitGdiBmp = NULL;
 //	delete iWinGc;
 //	iWinGc = NULL;
 	iHandle = 0;
 	
 	iWindow = &aWindow;
 	
 	delete iOffCanvas;
 	iOffCanvas = NULL;
 	
 	delete iOffDevice;
 	iOffDevice = NULL;
 	
 	delete iOffGc;
 	iOffGc = NULL;
 	
 	iOffCanvas = new (ELeave) CWsBitmap(Session());
 	 
 	TDisplayMode targetMode = iWindow->DisplayMode();         //this cause 5.x WSERV9, and in all devices it "should" be EColor16MU
 	if(targetMode == EColor16MA || targetMode == EColor16MAP)   //TAknScreenMode + THardwareState would be (?) a more proof way, if this not work..
 	        targetMode = EColor16MU;
 	
 	const TInt handle = reinterpret_cast<RBackedUpWindow*>(iWindow)->BitmapHandle();
 	User::LeaveIfError(iOffCanvas->Duplicate(handle));
 	//User::LeaveIfError(iOffCanvas->Create(iWindow->Size(), targetMode));
 	iOffDevice = CFbsBitmapDevice::NewL(iOffCanvas);	
 	
 	CDsa::ConstructL(aWindow, aDevice);
 	
    User::LeaveIfError(iOffDevice->CreateContext(iOffGc));
    iOffGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    
    /*
    if(EpocSdlEnv::Flags(CSDL::EDrawModeGdiGc) == CSDL::EDrawModeGdiGc)
        {
        User::LeaveIfError(aDevice.CreateContext(iWinGc));
        iWinGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        }    
 	*/
 	Start();
 	}
 	
void CDsaBitgdi::CreateSurfaceL()	
	{
	CBitmapSurface::CreateSurfaceL();
	iHandle = Bmp().Handle();
	}
	
	
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

NONSHARABLE_CLASS(CDirectDsa) : public CBitmapSurface, public MDirectScreenAccess
	{
public:
    CDirectDsa(RWsSession& aSession); 
private:	
	inline CDirectScreenAccess& Dsa() const;
	~CDirectDsa();
	void ConstructL(RWindow& aWindow, CWsScreenDevice& aDevice);
	void Stop();
	CBitmapContext* Gc();
//    TBool DoBlt();
    void UpdateRequestCompleted();
    void DisableDraw(TBool aDisable);
private:
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
private:
	void RestartL();
private:
    CDirectScreenAccess*  iDsa;
    TBool iRestartRequest;
	};

void CDirectDsa::UpdateRequestCompleted()
    {
    iDsa->ScreenDevice()->Update();
    }   

inline CDirectScreenAccess& CDirectDsa::Dsa() const
	{
	__ASSERT_ALWAYS(iDsa, PANIC(KErrNotReady));
	return *iDsa;
	}
	
CDirectDsa::CDirectDsa(RWsSession& aSession) : CBitmapSurface(aSession)
	{
	}
	
CBitmapContext* CDirectDsa::Gc()
	{
	return Dsa().Gc();
	}
	


void CDirectDsa::ConstructL(RWindow& aWindow, CWsScreenDevice& aDevice)
    {
    
    Stop();
    
    DsaLockOn();
    
    CDsa::ConstructL(aWindow, aDevice);
    
    if(iDsa == NULL)
        iDsa = CDirectScreenAccess::NewL(
                        Session(),
                        aDevice,
                        aWindow,
                        *this);		
    
    DsaLockOff();
    
    if(!iRestartRequest)
        {
        DisableDraw(EFalse);
        RestartL();
        }
    }	
	

CDirectDsa::~CDirectDsa()
	{
	if(iDsa != NULL)
        {
        iDsa->Cancel();
        }
    delete iDsa;
	}
	
	
void CDirectDsa::DisableDraw(TBool aDisable)
    {
    CDsa::DisableDraw(aDisable);
    if(!aDisable && iRestartRequest)
        {
        iRestartRequest = EFalse;
        TRAPD(err, RestartL());
        PANIC_IF_ERROR(err);
        }
    }

void CDirectDsa::RestartL()
    {
    __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));

    __ASSERT_ALWAYS(iDsa != NULL, PANIC(KErrNotFound));
    
   __ASSERT_ALWAYS(!IsDrawDisabled(), PANIC(KErrNotReady));
   __ASSERT_ALWAYS(!iRestartRequest, PANIC(KErrNotReady));
    

    DsaLockOn();
    

 //   DisableDraw(ETrue);
    iDsa->StartL();  

    const RRegion* r = iDsa->DrawingRegion();
    const TRect rect = r->BoundingRect();
   
    
    if( rect != TDsa(*this).ScreenRect() || rect.IsEmpty())
    	{
    	iDsa->Cancel();  
    	DisableDraw(ETrue);
    	iRestartRequest = ETrue; 
    	DsaLockOff();
    	return;	
   	 	}
   	 	
   	 	
    iDsa->Gc()->SetClippingRegion(r);   
     
    iDsa->Gc()->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    iDsa->Gc()->Clear();
    
   	SetTargetRect();
	
	Start();
	
	DsaLockOff();
	
    }

void CDirectDsa::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
/*  
    SetUpdating(EFalse);
    UnlockHwSurface(); 
	Stop();
*/
	}
	
void CDirectDsa::Restart(RDirectScreenAccess::TTerminationReasons aReason)
	{
	if(aReason == RDirectScreenAccess::ETerminateRegion && !IsDsaAvailable()/*aReason != RDirectScreenAccess::ETerminateCancel && aReason != RDirectScreenAccess::ETerminateScreenMode*/) //auto restart
		{
		TInt err = KErrNone;
		if(!IsDrawDisabled())
		    {
		    iRestartRequest = EFalse;
		    TRAP(err, RestartL());
		    }
		else
		    {
		    iRestartRequest = ETrue;
		    }
		if(err == KLeaveExit)
			{
			Stop();	
			}
		else
			{
			PANIC_IF_ERROR(err);
			}
		}
	}
	
	
void CDirectDsa::Stop()
	{
	CDsa::Stop();
	DsaLockOn();
	if(iDsa != NULL)
	    {
	    __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
	    iDsa->Cancel();
	    }
	DsaLockOff();
	}
		
    
   ///////////////////////////////////////////////////////////////////////
  
    
template<class T, class S>	
void ClipCopy(const TDsa& iDsa, TUint8* aTarget,
 					const TUint8* aSource,
 					const TRect& aUpdateRect,
 					const TRect& aSourceRect)
	{
	const S* source = reinterpret_cast<const S*>(aSource);
	const TInt lineWidth = aSourceRect.Width();
	
	source += (aUpdateRect.iTl.iY * lineWidth); 
	const TInt sourceStartOffset = aUpdateRect.iTl.iX;
	source += sourceStartOffset;
	
	T* targetPtr = reinterpret_cast<T*>(aTarget);
	
	const TInt scanLineWidth = iDsa.SwSize().iWidth;
	
	targetPtr += (aSourceRect.iTl.iY + aUpdateRect.iTl.iY ) * scanLineWidth; 
	const TInt targetStartOffset = (aUpdateRect.iTl.iX + aSourceRect.iTl.iX);
	
	targetPtr += targetStartOffset;
	
	
	const TInt height = Min(aUpdateRect.Height(), aSourceRect.Height()); 
		
	const TInt lineMove = iDsa.IsTurn() ? 1 : lineWidth;
	const TInt copyLen = aUpdateRect.Width();
	
	
	if(iDsa.IsFlip())
		{
		
		targetPtr += scanLineWidth *  (height - 1);
	
		for(TInt i = 0; i < height; i++) //source is always smaller
			{
			iDsa.Copy(reinterpret_cast<TUint32*>(targetPtr), reinterpret_cast<const TUint8*>(source), copyLen, height);
			source += lineMove;
			targetPtr -= scanLineWidth;
			}
		}
	else
		{
		
		
		for(TInt i = 0; i < height; i++) //source is always smaller
			{
			iDsa.Copy(reinterpret_cast<TUint32*>(targetPtr), reinterpret_cast<const TUint8*>(source), copyLen, height);
			source += lineMove;
			targetPtr += scanLineWidth; // >> 2;
			}
		}

	}
	


	

	
/////////////////////////////////////////////////////////////////////////////////////////////////////	


void CDsa::Free()
    {
    }

CDsa* CDsa::CreateL(RWsSession& aSession)
	{
	if(EpocSdlEnv::Flags(CSDL::EDrawModeDSB))
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	else if(EpocSdlEnv::Flags(CSDL::EDrawModeGdi))
		{
		return new (ELeave) CDsaBitgdi(aSession);
		}
    else
    	{
        return new (ELeave) CDirectDsa(aSession);
		}
	}
	
	
TSize CDsa::WindowSize() const
	{
	TSize size = iSwSize;
	if(iStateFlags & EOrientation90)
		{
		const TInt tmp = size.iWidth;
		size.iWidth = size.iHeight;
		size.iHeight = tmp;
		}
	return size;
	}


void CDsa::SetUpdating(TBool aUpdate)
	{
	if(aUpdate)
		iStateFlags |= EUpdating;
	else
		iStateFlags &= ~EUpdating;
	}


void CDsa::SetOrientation(CSDL::TOrientationMode aOrientation)
	{
	TInt flags = 0;
	switch(aOrientation)
		{
		case CSDL::EOrientation90:
			flags = EOrientation90;
			break;
		case CSDL::EOrientation180:
			flags = EOrientation180;
			break;
		case CSDL::EOrientation270:
			flags = EOrientation90 | EOrientation180;
			break;
		case CSDL::EOrientation0:
			flags = 0;
			break;
		}
	if(flags != (iStateFlags & EOrientationFlags))
		{
		iStateFlags |= EOrientationChanged;
		iNewFlags = flags; //cannot be set during drawing...
		}
	}

CDsa::~CDsa()
    {
    iDsaLock.Close();
    iOverlays.Close();
    User::Free(iLut256);
    }

         
void CDsa::ConstructL(RWindow& aWindow, CWsScreenDevice& /*aDevice*/)
    {			
    __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
    const TBool init = iDsaLock.Handle() == 0;
    
    if(init)
        iDsaLock.CreateLocal();
    
	if(iLut256 == NULL)
		iLut256 = (TUint32*) User::AllocL(256 * sizeof(TUint32));
	
	iTargetMode = aWindow.DisplayMode();
	
	if(iTargetMode == EColor16MA || iTargetMode == EColor16MAP)
	    iTargetMode = EColor16MU;
	
	
	iTargetBpp = BytesPerPixel(DisplayMode());
	
	const TRect oldRect = iScreenRect;
	
	iScreenRect = TRect(aWindow.Position(), aWindow.Size());
	
	if(!init && oldRect != iScreenRect)
	    {
	    EpocSdlEnv::ScreenSizeChanged(); 
	    TWsEvent event;
        event.SetType(EEventScreenDeviceChanged);
        event.SetTimeNow();
        EpocSdlEnv::EventQueue().Append(event);
	    }
	    
	SetTargetRect();
	iWindow = &aWindow;
    }

#ifdef _DEBUG
void CDsa::DsaLockOnD(TInt  aLine)
    {
    if(EnvUtils::IsOwnThreaded())
        {
        RDebug::Print(_L("Lock-On %d"), aLine);
        iDsaLock.Wait();
        }
    }

void CDsa::DsaLockOffD(TInt aLine)
    {
    if(EnvUtils::IsOwnThreaded())
        {
        RDebug::Print(_L("Lock-Off %d"), aLine);
        iDsaLock.Signal();
        }
    }
#else
void CDsa::DsaLockOn()
    {
    if(EnvUtils::IsOwnThreaded())
        {
        iDsaLock.Wait();
        }
    }

void CDsa::DsaLockOff()
    {
    if(EnvUtils::IsOwnThreaded())
        {
        iDsaLock.Signal();
        }
    }
#endif

void CDsa::DrawOverlays(CBitmapContext& aContext) const
	{
	const TInt last = iOverlays.Count() - 1;
	for(TInt i = last; i >= 0 ; i--)
		iOverlays[i].iOverlay->Draw(aContext, HwRect(), SwSize());
	}

TInt CDsa::AppendOverlay(MOverlay& aOverlay, TInt aPriority)
	{
	TInt i;
	for(i = 0; i < iOverlays.Count() && iOverlays[i].iPriority < aPriority; i++)
		{}
	const TOverlay overlay = {&aOverlay, aPriority};
	return iOverlays.Insert(overlay, i);
	}
	
TInt CDsa::RemoveOverlay(MOverlay& aOverlay)
	{
	TInt err = KErrNotFound;
	for(TInt i = 0; i < iOverlays.Count(); i++)
		{
		if(iOverlays[i].iOverlay == &aOverlay)
			{
			iOverlays.Remove(i);
			err =  KErrNone;
			}
		}
	return err;
	}

void CDsa::LockPalette(TBool aLock)
	{
	if(aLock)
		iStateFlags |= EPaletteLocked;
	else
		iStateFlags &= ~EPaletteLocked;
	}
TInt CDsa::SetPalette(TInt aFirst, TInt aCount, TUint32* aPalette)
	{
	if(iLut256 == NULL)
		return KErrNotFound;
	const TInt count = aCount - aFirst;
	if(count > 256)
		return KErrArgument;
	if(iStateFlags & EPaletteLocked)
		return KErrNone;
	for(TInt i = aFirst; i < count; i++) //not so busy here:-)
		{
		iLut256[i] = aPalette[i];
		}
	return KErrNone;
	}
	
	


    
CDsa::CDsa(RWsSession& aSession) : 
 	iStateFlags(0),
 	iSession(aSession)
  	
	{
	iCFTable[0] = CopyMem;
	iCFTable[1] = CopyMemFlipReversed;
	iCFTable[2] = CopyMemReversed;
	iCFTable[3] = CopyMemFlip;	
	
	iCFTable[4] = Copy256;
	iCFTable[5] = Copy256FlipReversed;
	iCFTable[6] = Copy256Reversed;
	iCFTable[7] = Copy256Flip;	
	
	
	iCFTable[8] = CopySlow;
	iCFTable[9] = CopySlowFlipReversed;
	iCFTable[10] = CopySlowReversed;
	iCFTable[11] = CopySlowFlip;	
	}
	
RWsSession& CDsa::Session()
	{
	return iSession;
	}


TUint8* CDsa::LockHwSurface()
	{
	if(!IsUpdating()) //else frame is skipped
		{
		SetUpdating(ETrue);
		return LockSurface();
		}
	return NULL; 
	}

void CDsa::UnlockHwSurface()
    {
    if(!IsUpdating())
        {
        UnlockSurface();
        iTargetAddr = NULL;
        }
    __ASSERT_ALWAYS(iTargetAddr == NULL, PANIC(KErrNotReady));
    }
	
TInt CDsa::AllocSurface(TBool aHwSurface, const TSize& aSize, TDisplayMode aMode)
	{
	DsaLockOn();
	 __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
	/*
	 * if(aHwSurface && aMode != DisplayMode())
	    {
	    DsaLockOff();
	    return KErrArgument;
	    }
	*/
	
	
	if(aHwSurface)
	    {
	    iStateFlags |= EHwSurface;
	    }
	
	iSourceMode = aMode;
	
	iSourceBpp = BytesPerPixel(aMode);
	
	const TSize size = WindowSize();
	if(aSize.iWidth > size.iWidth || aSize.iHeight > size.iHeight)
	    {
	    DsaLockOff();
		return KErrTooBig;
	    }
	
	TRAPD(err, CreateSurfaceL());
	if(err != KErrNone)
	    {
	    DsaLockOff();
		return err;
	    }
	
	if(!aHwSurface)
	    SetCopyFunction();
	
	iOwnerThread = RThread().Id();
	
	if(IsDsaAvailable())
	    Wipe();
	
	DsaLockOff();
	
	if(EnvUtils::IsOwnThreaded())
	    {
	    EpocSdlEnv::ObserverEvent(MSDLObserver::EEventWindowReserved);
	    }
	return KErrNone;
	}
	

void CDsa::CreateZoomerL(const TSize& aSize)
	{
	DsaLockOn();
	__ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
	iSwSize = aSize;
	iStateFlags |= EResizeRequest;
	CreateSurfaceL();
	SetTargetRect();
	DsaLockOff();
	}
	

/*
void SaveBmp(const TDesC& aName, const TAny* aData, TInt aLength, const TSize& aSz, TDisplayMode aMode)
	{
	CFbsBitmap* s = new CFbsBitmap();
	s->Create(aSz, aMode);
	s->LockHeap();
	TUint32* addr = s->DataAddress();
	Mem::Copy(addr, aData, aLength);
	s->UnlockHeap();
	s->Save(aName);
	s->Reset();
	delete s;
	}
	
void SaveBmp(const TDesC& aName, const TUint32* aData, const TSize& aSz)
	{
	CFbsBitmap* s = new CFbsBitmap();
	s->Create(aSz, EColor64K);
	TBitmapUtil bmp(s);
	bmp.Begin(TPoint(0, 0));
	for(TInt j = 0; j < aSz.iHeight; j++)
		{
		bmp.SetPos(TPoint(0, j));
		for(TInt i = 0; i < aSz.iWidth; i++)
			{
			bmp.SetPixel(*aData);
			aData++;
			bmp.IncXPos();
			}
		}
	bmp.End();
	s->Save(aName);
	s->Reset();
	delete s;
	}	
	
TBuf<16> FooName(TInt aFoo)
	{
	TBuf<16> b;
	b.Format(_L("C:\\data\\others\\pic%d.mbm"), aFoo);
	return b;
	}
	
*/


void CDsa::ClipCopy(TUint8* aTarget,
 					const TUint8* aSource,
 					const TRect& aUpdateRect,
 					const TRect& aSourceRect) const
 		{
 		const TDsa dsa(*this);
 		
 		switch(iSourceBpp)
 			{
 			case 1:
 				::ClipCopy<TUint32, TUint8>(dsa, aTarget, aSource, aUpdateRect, aSourceRect);
 				break;
 			case 2:
 				::ClipCopy<TUint32, TUint16>(dsa, aTarget, aSource, aUpdateRect, aSourceRect);
 				break;
 			case 4:
 				::ClipCopy<TUint32, TUint32>(dsa, aTarget, aSource, aUpdateRect, aSourceRect);
 				break;
 			}
 		}	

	
void CDsa::SetCopyFunction()
	{
	//calculate offset to correct function in iCFTable according to given parameters
	TInt function = 0;
	const TInt KCopyFunctions = 4;
	const TInt KOffsetToNative = 0;
	const TInt KOffsetTo256 = KOffsetToNative + KCopyFunctions;
	const TInt KOffsetToOtherModes = KOffsetTo256 + KCopyFunctions;
	const TInt KOffsetTo90Functions = 1;
	const TInt KOffsetTo180Functions = 2;
	
	if(iSourceMode == DisplayMode())
		function = KOffsetToNative; 		//0
	else if(iSourceMode == EColor256)
		function = KOffsetTo256;			//4
	else
		function = KOffsetToOtherModes; 	//8
	
	if(iStateFlags & EOrientation90)
		function += KOffsetTo90Functions; 	// + 1
	if(iStateFlags & EOrientation180)
		function += KOffsetTo180Functions; 	//+ 2
	
	iCopyFunction = iCFTable[function];
	
	}
	
inline void Rotate(TRect& aRect)
	{
	const TInt dx = aRect.iBr.iX - aRect.iTl.iX;
	const TInt dy = aRect.iBr.iY - aRect.iTl.iY;

	aRect.iBr.iX = aRect.iTl.iX + dy;
	aRect.iBr.iY = aRect.iTl.iY + dx;
	
	const TInt tmp = aRect.iTl.iX;
	aRect.iTl.iX = aRect.iTl.iY;
	aRect.iTl.iY = tmp;
	}
	


void CDsa::DisableDraw(TBool aDisable)
    {
    if(aDisable)
        iStateFlags |= EDisableDraw;
    else
        {
        iStateFlags &= ~EDisableDraw;
        if(IsDsaAvailable())
            {
            DsaLockOn();
            Wipe();
            DsaLockOff();
            }
        }
    }

TBool CDsa::AddUpdateRect(const TUint8* aBits, const TRect& aUpdateRect, const TRect& aRect)
	{
	DsaLockOn();
	
	if(iStateFlags & EOrientationChanged)
		{
		iStateFlags &= ~EOrientationFlags;
		iStateFlags |= iNewFlags;
		
		if(!(iStateFlags & EHwSurface))
		    SetCopyFunction();
		
		if(IsDsaAvailable())
		    Wipe();
		
		iStateFlags &= ~EOrientationChanged;
		
		DsaLockOff();
		EpocSdlEnv::ScreenSizeChanged();
	    //EpocSdlEnv::WaitDeviceChange();
	    return EFalse; //skip this frame as data is may be changed
		}
	
	__ASSERT_ALWAYS(!(iStateFlags & EHwSurface), PANIC(KErrGeneral));

	if(iStateFlags & EDisableDraw)
	    {
	    DsaLockOff();
	    return EFalse; //skip this frame
	    }
	
	if(iTargetAddr == NULL)
		{
		iTargetAddr = LockHwSurface();
		}
	else if(!IsUpdating())
	    {
	    SetUpdating(ETrue);
	    }
	
	TUint8* target = iTargetAddr;
	if(target == NULL)
	    {
	    DsaLockOff();
	    SetUpdating(EFalse);
		return EFalse;
	    }
	
	
	TRect targetRect = TRect(TPoint(0, 0), SwSize());
	
	TRect sourceRect = aRect;
	TRect updateRect = aUpdateRect;
	
	if(iStateFlags & EOrientation90)
		{
		Rotate(sourceRect);
		Rotate(updateRect);
		}
		
	if( (BytesPerPixel(iSourceMode) == 2) && (BytesPerPixel(DisplayMode()) == 4) )
		{
		TUint16* target = (TUint16*)iTargetAddr;
		TSize aSize = aRect.Size();
		TSize ss = SwSize();
	
		if( aSize.iWidth <= ss.iWidth && aSize.iHeight <= ss.iHeight )
			{
			TUint32* dst;
			dst = (TUint32*)(target
				+ ( ( ss.iHeight - aSize.iHeight ) / 2 ) * ss.iWidth
				+ ( ss.iWidth - aSize.iWidth ) / 2);
			int skip = ss.iWidth - aSize.iWidth;
	
			unsigned int h = aSize.iHeight;

			TUint16* src = (TUint16*)aBits;
			do
				{
				unsigned int w = aSize.iWidth;
				do
					{
					*dst = (*src & 0xF800)<< 8;
					*dst |= (*src & 0x07E0) << 5;
					*dst |= (*src & 0x01F) << 3; 
					dst++;
					src++;
					}
				while( --w );
				dst += skip;
				}
			while( --h );
			}
		}
	else if(iSourceMode != DisplayMode() ||  targetRect != sourceRect || targetRect != updateRect || ((iStateFlags & EOrientationFlags) != 0))
		{
		sourceRect.Intersection(targetRect); //so source always smaller or equal than target		//updateRect.Intersection(targetRect);
		ClipCopy(target, aBits, updateRect, sourceRect);
		}
	else
		{
		const TInt byteCount = aRect.Width() * aRect.Height() * iSourceBpp; //this could be stored
		Mem::Copy(target, aBits, byteCount);
		}
	DsaLockOff();
	//gLastError = _L("out addu");
	return ETrue;
	}
	
		
void CDsa::UpdateSurface()
	{
	DsaLockOn();
	if(iTargetAddr != NULL)
	    {
	    __ASSERT_ALWAYS(IsUpdating(), PANIC(KErrNotReady));
	    iTargetAddr = NULL;
	    UnlockHwSurface();	//could be faster if does not use AO, but only check status before redraw, then no context switch needed
	    SetUpdating(EFalse);
	    }
	Update();
	DsaLockOff();
	}
	
	
void CDsa::Stop()
	{
	iStateFlags &= ~ERunning;
	}
	
void CDsa::Start()
	{     
    EpocSdlEnv::ObserverEvent(MSDLObserver::EEventWindowReserved);	
     
    EpocSdlEnv::EnableDraw();
    
    EpocSdlEnv::ResumeDsa();
    
    iStateFlags |= ERunning;
      
	}

	
void CDsa::SetBlitter(MBlitter* aBlitter)
	{
	iBlitter = aBlitter;
	}
	
	
TPoint CDsa::WindowCoordinates(const TPoint& aPoint) const	
	{
	TPoint pos = aPoint - iTargetRect.iTl;
	const TSize asz = iTargetRect.Size();
	if(iStateFlags & EOrientation180)
		{
		pos.iX = asz.iWidth - pos.iX;
		pos.iY = asz.iHeight - pos.iY;	
		}	
	if(iStateFlags & EOrientation90)
		{
		pos.iX = aPoint.iY;
		pos.iY = aPoint.iX;	
		}
	if(!(iStateFlags & EUseBlit))
	    {
        pos.iX <<= 16;
        pos.iY <<= 16;
        pos.iX /= asz.iWidth; 
        pos.iY /= asz.iHeight;
        pos.iX *= iSwSize.iWidth;
        pos.iY *= iSwSize.iHeight;
        pos.iX >>= 16;
        pos.iY >>= 16;
	    }
	return pos; 	
	}

TRect CDsa::CalcResizeRect(const TSize& aBound, const TSize& aRatio)
    {
    TRect rect;
            
    const TInt dh = (aRatio.iHeight << 16) / aRatio.iWidth;

    if((aBound.iWidth * dh ) >> 16 <= aBound.iHeight)
        {
        rect.SetRect(TPoint(0, 0), TSize(aBound.iWidth, (aBound.iWidth * dh) >> 16));
        }
    else
        {
        const TInt dw = (aRatio.iWidth << 16) / aRatio.iHeight;
        rect.SetRect(TPoint(0, 0), TSize((aBound.iHeight * dw) >> 16, aBound.iHeight));
        }
    rect.Move((aBound.iWidth - rect.Size().iWidth) >> 1, (aBound.iHeight - rect.Size().iHeight) >> 1);  
    return rect;
    }
	
void CDsa::SetTargetRect()
	{
	iTargetRect = iScreenRect;
	
	if((iStateFlags & EResizeRequest) && EpocSdlEnv::Flags(CSDL::EAllowImageResizeKeepRatio))
		{	
		iTargetRect = CalcResizeRect(iScreenRect.Size(), iSwSize);
		} 
	if(!(iStateFlags & EResizeRequest))
		iSwSize = iScreenRect.Size();
	const TBool noZoomOut = 
	            (!EpocSdlEnv::Flags(CSDL::EImageResizeZoomOut)) && 
	                                    (HwRect().Size().iWidth > SwSize().iWidth && 
	                                    HwRect().Size().iHeight > SwSize().iHeight);
	          
    const TBool useBlit = (SwSize() == HwRect().Size()) || noZoomOut;

    iStateFlags &= ~(ENoZoomOut | EUseBlit);
    
    if(noZoomOut)
        {
        iStateFlags |= ENoZoomOut;
        iTargetRect.Move(-iTargetRect.iTl.iX, -iTargetRect.iTl.iY);
        iTargetRect.Move((iScreenRect.Size() - SwSize()).AsPoint());
        iTargetRect.Move(-(iTargetRect.iTl.iX / 2), -(iTargetRect.iTl.iY / 2));
        }
    else
        {
        iTargetRect.Move(iScreenRect.iTl);
        }
    
    if(useBlit)
        iStateFlags |= EUseBlit;

	}
	
RWindow* CDsa::Window()
	{
	return iWindow;
	}
	
CDsa* CDsa::CreateGlesDsaL()
	{
    __ASSERT_ALWAYS(!IsUpdating(), PANIC(KErrNotReady));
	CDsa* dsa = new (ELeave) CDsaGles(Session());
	CWsScreenDevice* dummy = NULL;
	dsa->ConstructL(*Window(), *dummy);
	Free();
	delete this;
	return dsa;
	}
		


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDsa::Copy256(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt)
	{
	TUint32* target = aTarget;
	const TUint32* endt = target + aBytes; 
	const TUint8* source = aSource;
	while(target < endt)
		{
		*target++ = aDsa.iLut256[*source++]; 
		}
	}
	
void CDsa::Copy256Reversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt)
	{
	const TUint32* target = aTarget;
	TUint32* endt = aTarget + aBytes; 
	const TUint8* source = aSource;
	while(target < endt)
		{
		*(--endt) = aDsa.iLut256[*source++]; 
		}
	}	
	
void CDsa::Copy256Flip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	TUint32* target = aTarget;
	const TUint32* endt = target + aBytes; 
	const TUint8* column = aSource;

	while(target < endt)
		{
		*target++ = aDsa.iLut256[*column];
		column += aLineLen;
		}
	}
	
void CDsa::Copy256FlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	const TUint32* target = aTarget;
	TUint32* endt = aTarget + aBytes; 
	const TUint8* column = aSource;

	while(target < endt)
		{
		*(--endt) = aDsa.iLut256[*column];
		column += aLineLen;
		}
	}		

void CDsa::CopyMem(const CDsa& /*aDsa*/, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt)
	{
	const TUint32* src = reinterpret_cast<const TUint32*>(aSource);
	Mem::Copy(aTarget, src, aBytes << 2);
	}
	
void CDsa::CopyMemFlip(const CDsa& /*aDsa*/, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	TUint32* target = aTarget;
	const TUint32* endt = target + aBytes; 
	const TUint32* column = reinterpret_cast<const TUint32*>(aSource);

	while(target < endt)
		{
		*target++ = *column;
		column += aLineLen;
		}
	}
	
void CDsa::CopyMemReversed(const CDsa& /*aDsa*/, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt)
	{
	const TUint32* target = aTarget;
	TUint32* endt = aTarget + aBytes; 
	const TUint32* source = reinterpret_cast<const TUint32*>(aSource);
	while(target < endt)
		{
		*(--endt) = *source++; 
		}
	}	
	
	
void CDsa::CopyMemFlipReversed(const CDsa& /*aDsa*/, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	const TUint32* target = aTarget;
	TUint32* endt = aTarget + aBytes; 
	const TUint32* column = reinterpret_cast<const TUint32*>(aSource);

	while(target < endt)
		{
		*(--endt) = *column;
		column += aLineLen;
		}
	}
			
	
NONSHARABLE_CLASS(MRgbCopy)
	{
	public:
	virtual void Copy(TUint32* aTarget, const TUint8* aSource, TInt aBytes) = 0;
	virtual void FlipCopy(TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen) = 0;
	virtual void CopyReversed(TUint32* aTarget, const TUint8* aSource, TInt aBytes) = 0;
	virtual void FlipCopyReversed(TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen) = 0;
	    
	};
	
template <class T>
NONSHARABLE_CLASS(TRgbCopy) : public MRgbCopy
	{
	public:
	TRgbCopy(TDisplayMode aMode);
	void* operator new(TUint aBytes, TAny* aMem);
	void Copy(TUint32* aTarget, const TUint8* aSource, TInt aBytes);
	void FlipCopy(TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
	void CopyReversed(TUint32* aTarget, const TUint8* aSource, TInt aBytes);
	void FlipCopyReversed(TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
	    
	static TUint32 Gray256(const TUint8& aPixel);
	static TUint32 Color256(const TUint8& aPixel);
	static TUint32 Color4K(const TUint16& aPixel);
	static TUint32 Color64K(const TUint16& aPixel);
	static TUint32 Color16M(const TUint32& aPixel);
	static TUint32 Color16MU(const TUint32& aPixel);
	static TUint32 Color16MA(const TUint32& aPixel);
	private:
		typedef TUint32 (*TRgbFunc) (const T& aValue);
		TRgbFunc iFunc;
	};
		
		
template <class T>		
void* TRgbCopy<T>::operator new(TUint /*aBytes*/, TAny* aMem)
	{
	return aMem;
	}
		
template <class T>
TRgbCopy<T>::TRgbCopy(TDisplayMode aMode)
	{
	switch(aMode)
		{
		case EGray256 : iFunc = (TRgbFunc) Gray256; break;
		case EColor256 : iFunc =  (TRgbFunc) Color256; break;
		case EColor4K : iFunc =  (TRgbFunc) Color4K; break;
		case EColor64K : iFunc =  (TRgbFunc) Color64K; break;
		case EColor16M : iFunc =  (TRgbFunc) Color16M; break;
		case EColor16MU : iFunc =  (TRgbFunc) Color16MU; break;
		case EColor16MA : iFunc =  (TRgbFunc) Color16MA; break;
		default:
			PANIC(KErrNotSupported);
		}
	}


template <class T>
void TRgbCopy<T>::Copy(TUint32* aTarget, const TUint8* aSource, TInt aBytes)
	{
	const T* source = reinterpret_cast<const T*>(aSource);
	TUint32* target = aTarget;
	TUint32* endt = target + aBytes;
	
	while(target < endt)
	    {
		const T value = *source++;
		*target++ = iFunc(value);//iFunc(value).Value();
		}
	}

template <class T>
void TRgbCopy<T>::CopyReversed(TUint32* aTarget, const TUint8* aSource, TInt aBytes)
    {
    const T* source = reinterpret_cast<const T*>(aSource);
    TUint32* target = aTarget;
    TUint32* endt = target + aBytes;
    
    while(target < endt)
        {
        const T value = *source++;
        *(--endt) = iFunc(value);//iFunc(value).Value();
        }
      
    }

template <class T>
void TRgbCopy<T>::FlipCopy(TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	const T* column = reinterpret_cast<const T*>(aSource);
	TUint32* target = aTarget;
	TUint32* endt = target + aBytes;
	
    while(target < endt)
        {
        *target++ = iFunc(*column);
        column += aLineLen;
        }
	}	
		
template <class T>
void TRgbCopy<T>::FlipCopyReversed(TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
    {
    const T* column = reinterpret_cast<const T*>(aSource);
    TUint32* target = aTarget;
    TUint32* endt = target + aBytes;
   
    while(target < endt)
        {
        *(--endt) = iFunc(*column);
        column += aLineLen;
        }
    
    }

template <class T> TUint32 TRgbCopy<T>::Gray256(const TUint8& aPixel)
	{
	const TUint32 px = aPixel << 16 | aPixel << 8 | aPixel;
	return px;
	}
	
template <class T> TUint32 TRgbCopy<T>::Color256(const TUint8& aPixel)
	{
	return TRgb::Color256(aPixel).Internal();
	}
	
template <class T> TUint32 TRgbCopy<T>::Color4K(const TUint16& aPixel)
	{
	TUint32 col = (aPixel & 0xF00) << 12;
	col |= (aPixel & 0xF00) << 8; 
	
	col |= (aPixel & 0x0F0) << 8;
	col |= (aPixel & 0x0F0);
	
	col |= (aPixel & 0x00F) << 4;
	col |= (aPixel & 0x00F);
	
	return col;
	}
	
template <class T> TUint32 TRgbCopy<T>::Color64K(const TUint16& aPixel)
	{
	TUint32 col = (aPixel & 0xF800)<< 8;
	col |= (aPixel & 0xE000) << 3; 
	
	col |= (aPixel & 0x07E0) << 5;
	col |= (aPixel & 0x600) >> 1;
	
	col |= (aPixel & 0x01F) << 3;
	col |= (aPixel & 0x1C) >> 2;
	
	return col;
	}
	
template <class T> TUint32 TRgbCopy<T>::Color16M(const TUint32& aPixel)
	{
	return TRgb::Color16M(aPixel).Internal();
	}
	
template <class T> TUint32 TRgbCopy<T>::Color16MU(const TUint32& aPixel)
	{
	return TRgb::Color16MU(aPixel).Internal();
	}
	
template <class T> TUint32 TRgbCopy<T>::Color16MA(const TUint32& aPixel)
	{
	return TRgb::Color16MA(aPixel).Internal();
	}

typedef TUint64 TStackMem;

LOCAL_C MRgbCopy* GetCopy(TAny* mem, TDisplayMode aMode)
	{
	if(aMode == EColor256 || aMode == EGray256)
		{
		return new (mem) TRgbCopy<TUint8>(aMode);
		}
	if(aMode == EColor4K || aMode == EColor64K)
		{
		return new (mem) TRgbCopy<TUint16>(aMode);
		}
	if(aMode == EColor16M || aMode == EColor16MU || aMode == EColor16MA)
		{
		return new (mem) TRgbCopy<TUint32>(aMode);
		}
	PANIC(KErrNotSupported);
	return NULL;
	}
	

void CDsa::CopySlowFlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	TStackMem mem = 0;
	GetCopy(&mem, aDsa.iSourceMode)->FlipCopyReversed(aTarget, aSource, aBytes, aLineLen);	
	}
	
void CDsa::CopySlowFlip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen)
	{
	TStackMem mem = 0;
	GetCopy(&mem, aDsa.iSourceMode)->FlipCopy(aTarget, aSource, aBytes, aLineLen);
	}
	
void CDsa::CopySlow(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt)
	{
	TStackMem mem = 0;
	GetCopy(&mem, aDsa.iSourceMode)->Copy(aTarget, aSource, aBytes);	
	}	

void CDsa::CopySlowReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt)
	{
	TStackMem mem = 0;
	GetCopy(&mem, aDsa.iSourceMode)->CopyReversed(aTarget, aSource, aBytes);	
	}	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////7


