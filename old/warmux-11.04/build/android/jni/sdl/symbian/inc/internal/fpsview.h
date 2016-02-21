/*
 * fpsview.h
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */

#ifndef FPSVIEW_H_
#define FPSVIEW_H_

#include <sdlepocapi.h>

NONSHARABLE_CLASS(CFpsView) : public CBase, public MOverlay
    {
public:
    CFpsView(const TPoint& aPos, CFbsBitmap* aNumbers);
    ~CFpsView();
    void ConstructL();
private:
    void Draw(CBitmapContext& aGc, const TRect& aTargetRect, const TSize& aSize);
    static TInt Tick(TAny* aThis);
    void DoTick();
private:
    TPoint iPos;
    CFbsBitmap* iNumbers;
    CPeriodic* iTicker;
    TInt iFps;
    TInt iFpsCount;
    TInt iDigits;
    };


#endif /* FPSVIEW_H_ */
