/*
 * virtualcursor.cpp
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */


#include <w32std.h>
#include "virtualcursor.h"


const TInt KMaxMove = 10;      
    
TVirtualCursor::TVirtualCursor() :  iInc(0, 0), iIsOn(EFalse), iIsMove(EFalse), iCBmp(NULL)
    {   
    }
    
 
void TVirtualCursor::Move(TInt aX, TInt aY)
    {
    if(aX > 0 && iInc.iX > 0)
            ++iInc.iX;
    else if(aX < 0 && iInc.iX < 0)
            --iInc.iX;
    else
        iInc.iX = aX;

    if(aY > 0 && iInc.iY > 0)
            ++iInc.iY;
    else if(aY < 0 && iInc.iY < 0)
            --iInc.iY;
    else
            iInc.iY = aY;
    
    iInc.iX = Min(KMaxMove, iInc.iX); 
    
    iInc.iX = Max(-KMaxMove, iInc.iX);
    
    iInc.iY = Min(KMaxMove, iInc.iY);
    
    iInc.iY =Max(-KMaxMove, iInc.iY);
    
    const TPoint pos = iPos + iInc;
    if(iRect.Contains(pos))
        {
        iPos = pos;
        }
    else
        {
        iInc = TPoint(0, 0);    
        }
    }
    
    
void TVirtualCursor::ToggleOn()
    {
    iIsOn = !iIsOn;
    }
    

void TVirtualCursor::ToggleMove()
    {
    iIsMove = !iIsMove;
    }

TBool TVirtualCursor::IsOn() const
    {
    return iIsOn;
    }

TBool TVirtualCursor::IsMove() const
    {
    return iIsMove;
    }

void TVirtualCursor::Set(const TRect& aRect, CFbsBitmap* aBmp, CFbsBitmap* aAlpha)
    {
    iRect = aRect;
    iCBmp = aBmp;
    iAlpha = aAlpha;
    }
    
        
void TVirtualCursor::MakeEvent(TPointerEvent::TType aType, TWsEvent& aEvent, const TPoint& aBasePos) const
    {
    aEvent.SetType(EEventPointer),
    aEvent.SetTimeNow();
    TPointerEvent& pointer = *aEvent.Pointer(); 
    pointer.iType = aType;
    pointer.iPosition = iPos;
    pointer.iParentPosition = aBasePos;
    }
    
    
void TVirtualCursor::Draw(CBitmapContext& aGc, const TRect& /*aTargetRect*/, const TSize& /*aSize*/)
    {
    if(iIsOn && iCBmp != NULL)
        {
        const TRect rect(TPoint(0, 0), iCBmp->SizeInPixels());
        aGc.AlphaBlendBitmaps(iPos, iCBmp, rect, iAlpha, TPoint(0, 0));
        }
    
    }   
