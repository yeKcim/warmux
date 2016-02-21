/*
 * virtualcursor.h
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */

#ifndef VIRTUALCURSOR_H_
#define VIRTUALCURSOR_H_

#include <sdlepocapi.h>
#include <w32std.h>
 

NONSHARABLE_CLASS(TVirtualCursor) : public MOverlay
    {
    public:
        TVirtualCursor();
        void Set(const TRect& aRect, CFbsBitmap* aBmp, CFbsBitmap* aAlpha);
        void Move(TInt aX, TInt aY);
        void MakeEvent(TPointerEvent::TType aType, TWsEvent& aEvent, const TPoint& aBasePos) const;
        void ToggleOn();
        void ToggleMove();
        TBool IsOn() const;
        TBool IsMove() const;
    private:
        void Draw(CBitmapContext& aGc, const TRect& aTargetRect, const TSize& aSize);
    private:
        TRect iRect;
        TPoint iInc;
        TPoint iPos;
        TBool iIsOn;
        TBool iIsMove;
        CFbsBitmap* iCBmp;
        CFbsBitmap* iAlpha;
    };


#endif /* VIRTUALCURSOR_H_ */
