#include "hwablitter.h"
#include <gdi.h>
#include <fbs.h>
#include <bitdev.h>
#include <aknutils.h>
#include <GLES/egl.h>
#include <GLES/gl.h>

#ifdef OPENVGSUPPORT
#include <VG/openvg.h>
#endif

#ifdef OPENVGSUPPORT
void OSPrint(CFbsBitmap& aCanvas, const TRect& aRect, const TDesC& aText)
    {
    CFbsBitmapDevice* bd = CFbsBitmapDevice::NewL(&aCanvas);
    CleanupStack::PushL(bd);
    CFbsBitGc* gc;
    User::LeaveIfError(bd->CreateContext(gc));
    CleanupStack::PushL(gc);
    gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    gc->Activate(bd);
   
    gc->SetPenStyle(CGraphicsContext::ESolidPen);
    gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc->SetPenColor(KRgbBlack);
    gc->SetBrushColor(KRgbYellow);
    gc->DrawRect(aRect);
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    gc->UseFont(font);
    const TInt h = font->BaselineOffsetInPixels();
    DrawUtils::DrawText(*gc, aText, aRect, (aRect.Height() / 2) + h, CGraphicsContext::ECenter, 0, font);
    gc->DiscardFont();
    CleanupStack::PopAndDestroy(2);
    }


void OSPrintf(CFbsBitmap& aCanvas, TRefByValue<const TDesC16> aFmt,...)
    {
    VA_LIST lst;
    VA_START(lst, aFmt);
    TBuf<512> writeBuf;
    writeBuf.FormatList(aFmt, lst);
    OSPrint(aCanvas, TRect(5, 150, 300, 200), writeBuf);
    VA_END(lst);
    }

void PrintStatus(CFbsBitmap& aCanvas, const TDesC& aText, TInt aErr)
    {
    TBuf<128> b;
    const int egError = eglGetError();
    const int vgError = vgGetError();
    b.Format(_L("%S:%d, 0x%x, 0x%x"), &aText, aErr, egError, vgError);
    OSPrint(aCanvas, TRect(5, 80, 200, 120), b);
    }
#endif //OPENVGSUPPORT

#define PANIC(err) ppPanic(err, __LINE__)

TInt ppPanic(TInt e, TInt l)
    {
    TBuf<32> b;
    b.Format(_L("HWA %d"), l);
    User::Panic(b, e);
    return 0;
    }
    

enum
	{
		KErrNoDisp = 700,
		KErrInit,
		KErrConfig,
		KErrContext,
		KErrSurface,
		KErrVgBind,
		KErrMakeCurrent,
		KErrSwapBuffers,
	};

#ifdef OPENVGSUPPORT

VGImageFormat Format(TDisplayMode aMode)
	{
		switch(aMode)
			{
				case EColor64K: 
					return VG_sRGB_565;
			//	case EColor16M: 
				case EColor16MU:
                    return VG_sXRGB_8888;
				case EColor16MA:
				    return VG_sARGB_8888;
				case EColor16MAP:
				    return VG_sARGB_8888_PRE;
			}
		PANIC(KErrNotSupported);
		return VG_IMAGE_FORMAT_INVALID;
	}

NONSHARABLE_CLASS(HWAImp) : public CBase
{
public:
	HWAImp(RWindow& aWindow);
	TBool Set(TDisplayMode aMode, const TSize& aSize);
	TInt LastError() const {return iErr;} 
	void Blit(const CFbsBitmap& aBitmap, const TPoint& aPos, TReal aVScale, TReal aHScale);
	~HWAImp();
    void DestroyContext();
private:
	TInt Construct(TInt aDepth);
private:
	RWindow& iWindow;
	EGLDisplay iEglDisplay;
	EGLConfig iEglConfig;
	EGLContext iEglContext;
	EGLSurface iEglSurface;
	TInt iErr;
	TSize iSize;
	TDisplayMode iMode;
	VGImage iCanvas;
};

HWAImp::HWAImp(RWindow& aWindow) : iWindow(aWindow)
	{}

TBool HWAImp::Set(TDisplayMode aMode, const TSize& aSize)
	{
    iErr = KErrNone;
	TBool init = EFalse;
	if(iEglDisplay == EGL_NO_DISPLAY || aMode != iMode)
		{
		const TInt bits = TDisplayModeUtils::NumDisplayModeBitsPerPixel(aMode);	
		if(iEglDisplay != EGL_NO_DISPLAY)
			DestroyContext();
		iErr = Construct(bits);
				
		if(iErr != KErrNone)
			{
		    DestroyContext();
		    return EFalse;
			}
		else
			{
			iMode = aMode;
			init = ETrue;
			}
		}
	
	if(init || aSize != iSize)
		{
	    if(iCanvas != 0)
	        vgDestroyImage(iCanvas);
		iCanvas = 0;
		
		if(!eglMakeCurrent (iEglDisplay, iEglSurface, iEglSurface, iEglContext))
		    {
		    DestroyContext();
		    iErr = KErrMakeCurrent;
		    return EFalse;
		    }
		
		if(aSize.iWidth <= 0 || aSize.iHeight <= 0)
		    {
		    DestroyContext();
		    iErr = KErrCorrupt;
		    return EFalse;
		    }
		
		if(aSize.iWidth > 4095 || aSize.iHeight > 4095)
		    { 
		    DestroyContext();
		    iErr = KErrArgument;
		    return EFalse;
		    }
		

        const VGImageFormat format = Format(iMode);
        
		iCanvas = vgCreateImage(format, aSize.iWidth, aSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);
	
		if(iCanvas == 0)
		    {
		    DestroyContext();
		    iErr = KErrBadHandle;
		    return EFalse;
		    }
		iSize = aSize;
		return ETrue;
		}
	return EFalse;
	}



int HWAImp::Construct(TInt aDepth)
    {
	iEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	
	if(iEglDisplay == EGL_NO_DISPLAY)
		return KErrNoDisp;
	
	
	if(!eglInitialize(iEglDisplay, 0, 0))
		return KErrInit;
	
    if(!eglBindAPI(EGL_OPENVG_API))
        return KErrVgBind;
    
    
	EGLint attribList [] = 
	        { 
	        EGL_RENDERABLE_TYPE,    EGL_OPENVG_BIT,
	        
	        EGL_RED_SIZE,           aDepth > 16 ? 8 : 5,
	        EGL_GREEN_SIZE,         aDepth > 16 ? 8 : 6, 
	        EGL_BLUE_SIZE,          aDepth > 16 ? 8 : 5,

	        
	        EGL_BUFFER_SIZE,        aDepth,  // color depth
	        EGL_NONE 
	        };	
		
	 /* TPtrC8 str(reinterpret_cast<const TUint8*>(eglQueryString(iEglDisplay, EGL_EXTENSIONS)));
	        if(str.Find(_L8("EGL_SYMBIAN_COMPOSITION")))
	            {
	                iErr = KErrNotSupported;
	                return EFalse;
	            }   
	        */

	
	TInt numConfigs;
	
	if (!eglChooseConfig (iEglDisplay, attribList, &iEglConfig, 1, &numConfigs))
		return  KErrConfig;
		
	iEglContext = eglCreateContext (iEglDisplay, iEglConfig, EGL_NO_CONTEXT, 0);
	
	if(iEglContext == EGL_NO_CONTEXT )
	    return KErrContext;
  
	iEglSurface = eglCreateWindowSurface (iEglDisplay, iEglConfig, &iWindow, 0);	
  
    if (iEglSurface == EGL_NO_SURFACE)
        return KErrSurface;
	

	return KErrNone;
    }	

void HWAImp::DestroyContext()
	{
    if(iCanvas != 0)
        {
        vgDestroyImage(iCanvas);
        iCanvas = 0;
        }
    if(iEglDisplay != EGL_NO_DISPLAY)
        {
        eglMakeCurrent (iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface (iEglDisplay, iEglSurface);
        eglDestroyContext (iEglDisplay, iEglContext);
        eglTerminate (iEglDisplay);
        iEglDisplay = EGL_NO_DISPLAY;
        }
    iMode = ENone;
    iSize = TSize(0, 0);
	}

HWAImp::~HWAImp()
    {
	DestroyContext();
	eglReleaseThread();
    }
/*
void Triangle()
{
VGPath  path;

VGubyte segments[] = { VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
VGfloat coords[]   = {  0.0f, 0.0f,          // VG_MOVE_TO_ABS
                        50.0f, 0.0f,        // VG_LINE_TO_REL
                        -25.0f, 25.0f       // VG_LINE_TO_REL
                    };

path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);

vgAppendPathData( path, 4, segments, coords );

VGPaint stroke, fill;

VGfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

stroke = vgCreatePaint();
vgSetPaint( stroke, VG_STROKE_PATH );

vgSetParameteri( stroke, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
vgSetParameterfv( stroke, VG_PAINT_COLOR, 4, black );


fill = vgCreatePaint();
vgSetPaint( fill, VG_FILL_PATH );

vgSetParameteri( fill, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
vgSetColor( fill, 0xFF0000FF );


vgScale( 5.0f, 5.0f );
vgTranslate( 10.0f, 10.0f );
vgRotate( 30.0f );



vgDrawPath( path, VG_STROKE_PATH );
vgDestroyPath( path );
}
*/

void HWAImp::Blit(const CFbsBitmap& aBitmap, const TPoint& aPos, TReal aVScale, TReal aHScale)
    {
	if(!eglMakeCurrent (iEglDisplay, iEglSurface, iEglSurface, iEglContext))
	    {
	    iErr = KErrMakeCurrent;
	    return;
	    }
		
	const VGImageFormat format = Format(aBitmap.DisplayMode());
	
	
	const TSize sz = aBitmap.SizeInPixels();

    aBitmap.BeginDataAccess();
    
    //read it upside down
    const TUint8* data = reinterpret_cast<const TUint8*>(aBitmap.DataAddress())
            + (sz.iHeight - 1) * aBitmap.DataStride();
    
    const VGint dataStride = -aBitmap.DataStride();
    
   	vgImageSubData(iCanvas, data, dataStride, format, 0, 0,  sz.iWidth, sz.iHeight);
   	aBitmap.EndDataAccess(ETrue);
   	
   
  	if(aPos.iX != 0 || aPos.iY != 0 || aVScale != 1.0 || aHScale != 1.0)  
  	    {
  	    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  	    vgLoadIdentity();
  	    vgTranslate(VGfloat(aPos.iX), VGfloat(aPos.iY));
  	    vgScale(VGfloat(aVScale), VGfloat(aHScale));
  	    }
  	
  	vgDrawImage(iCanvas);
  	
 // 	::Triangle();
  	
	if(!eglSwapBuffers (iEglDisplay, iEglSurface))
	    iErr = KErrSwapBuffers;
	}

#endif //OPENVGSUPPORT 

CHWABlitter::CHWABlitter(TInt aFlags) : iFlags(aFlags)
    {}

EXPORT_C CHWABlitter* CHWABlitter::NewL(RWindow& aWindow, TInt aFlags)
	{
#ifdef OPENVGSUPPORT
	CHWABlitter* b = new (ELeave) CHWABlitter(aFlags);
	CleanupStack::PushL(b);
	b->iImp = new (ELeave) HWAImp(aWindow);
	CleanupStack::Pop(b);
	return b;
#else
	return NULL;
#endif
	}
	
EXPORT_C CHWABlitter::~CHWABlitter()
{
	delete iImp;
}

EXPORT_C TBool CHWABlitter::BitBlt(CBitmapContext& /*aGc*/,
		CFbsBitmap& aBmp,
		const TRect& aTargetRect,
		const TSize& aSize)
		{
#ifdef OPENVGSUPPORT
	    __ASSERT_ALWAYS(iImp != NULL, PANIC(KErrNotReady));
	
	    iImp->Set(aBmp.DisplayMode(), aBmp.SizeInPixels());
	    
        if(iImp->LastError() == KErrNone)
            {
                
            TReal vRat = 1.0;
            TReal hRat = 1.0;
            
            TPoint pos = aTargetRect.iTl;
              
            if(!(iFlags & EPreventScale) && aTargetRect.Size() != aSize && aSize.iWidth > 0 && aSize.iHeight > 0)
                {

                const TReal tw = aTargetRect.Size().iWidth; 
                const TReal th = aTargetRect.Size().iHeight;
                const TReal sw = aSize.iWidth;
                const TReal sh = aSize.iHeight;
                vRat = 	tw / sw;
                hRat = 	th / sh; 
                if(iFlags & EPreserveRatio)
                    {
                    if(sw * hRat <= tw)
                        {
                        vRat = hRat; 
                        pos.iX += (tw - (sw * vRat)) / 2;
                        }
                    else
                        {
                        hRat = vRat;
                        pos.iY += (th - (sh * hRat)) / 2;
                        }
                    }
                }
                           
            iImp->Blit(aBmp, pos, vRat, hRat);
            
            if(iImp->LastError() == KErrNone)
                return ETrue;
            }

        PrintStatus(aBmp, _L("Err"), iImp->LastError());
            
#endif //OPENVGSUPPORT
		return EFalse;
		}
	
void CHWABlitter::Release()
    {
#ifdef OPENVGSUPPORT
    iImp->DestroyContext();
#endif //OPENVGSUPPORT
    }
