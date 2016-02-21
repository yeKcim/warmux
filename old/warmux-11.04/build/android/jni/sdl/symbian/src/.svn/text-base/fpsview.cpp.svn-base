/*
 * fpsview.cpp
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */

#include <fbs.h>
#include "fpsview.h"


 CFpsView::CFpsView(const TPoint& aPos, CFbsBitmap* aNumbers) : 
 iPos(aPos), iNumbers(aNumbers), iFps(0), iFpsCount(0)
     {}
 
 CFpsView::~CFpsView()
     {
     if(iTicker)
         iTicker->Cancel();
     delete iTicker;
     delete iNumbers;
     }
 
void CFpsView::ConstructL()
    {
    iTicker = CPeriodic::NewL(CActive::EPriorityLow);
    const TTimeIntervalMicroSeconds32 second(1000000);
    iTicker->Start(second, second, TCallBack(Tick, this));
    }

void CFpsView::Draw(CBitmapContext& aGc, const TRect& aTargetRect, const TSize& aSize)
    {
    ++iFps; 
    int nro = iFpsCount;
    const TSize sz = iNumbers->SizeInPixels();
    TPoint pos(iPos.iX + sz.iWidth * iDigits, iPos.iY);
    const TInt height = (sz.iHeight / 10);
    do  
        {
        const int digit = nro % 10;
        nro /= 10;
        TRect rect(0, digit * height, sz.iWidth, digit * height + height);
        aGc.BitBlt(pos, iNumbers, rect);
        pos.iX -= sz.iWidth;
        }while(nro > 0);
    }
    
TInt CFpsView::Tick(TAny* aThis)
    {
    reinterpret_cast<CFpsView*>(aThis)->DoTick();
    return KErrNone;
    }

void CFpsView::DoTick()
    {
    iFpsCount = iFps;  
    iDigits = 0;
    do
       {
       iFps /= 10;
       ++iDigits;
       }while(iFps > 0);
    }
