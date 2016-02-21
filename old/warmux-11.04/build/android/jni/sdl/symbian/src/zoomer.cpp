/*
 * zoomer.cpp
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */

#include <fbs.h>
#include "zoomer.h"
#include "drawbitmapfilter.h"

CZoomer::~CZoomer()
    {
    delete iBitmap;
    }
        
        
CZoomer* CZoomer::NewL()
    {
    CZoomer* b = new (ELeave) CZoomer();
    CleanupStack::PushL(b);
    b->iBitmap = new (ELeave) CFbsBitmap();
    CleanupStack::Pop();
    return b;
    }
    
#ifndef EColor16MAP //this do something for bw comp issue 
#define EColor16MAP ENone 
#endif  
        
TBool CZoomer::BitBlt(CBitmapContext& aGc,
    CFbsBitmap& aBmp,
    const TRect& aTargetRect,
    const TSize& aSize)
    {
    if(aSize.iHeight == aTargetRect.Size().iHeight && aSize.iWidth == aTargetRect.Size().iWidth)
        return EFalse;
    
    if(0 == iBitmap->Handle() ||
        iBitmap->SizeInPixels() != aTargetRect.Size())
        {
        iBitmap->Reset();
        if(KErrNone != iBitmap->Create(aTargetRect.Size(), EColor64K))
            return EFalse;
        }
        
    const TDisplayMode mode = aBmp.DisplayMode();   
        
    switch(mode)
        {
        case EColor16MA:
        case EColor16MU:
        case EColor16MAP:
            break;
        default:
            return EFalse;
        }
    
    aBmp.LockHeap(ETrue);
    const TUint32* source = aBmp.DataAddress();
    TUint16* target = (TUint16*) iBitmap->DataAddress();
    DrawBitmapFilter::Draw(target, source, aTargetRect.Size(),
            aSize, aTargetRect.Size().iWidth * 2, aSize.iWidth * 4);
    aBmp.UnlockHeap();
    aGc.BitBlt(TPoint(0, 0), iBitmap);
    return ETrue;
    }
