/*
 * zoomer.h
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */

#ifndef ZOOMER_H_
#define ZOOMER_H_

#include "blitter.h"
class CBitmapContext;
class CFbsBitmap;

NONSHARABLE_CLASS(CZoomer) : public CBlitter
    {
public:
    static CZoomer* NewL();
    ~CZoomer();
    TBool BitBlt(CBitmapContext& aGc,
        CFbsBitmap& aBmp,
        const TRect& aTargetRect,
        const TSize& aSize);
private:
    CFbsBitmap* iBitmap;
    };
    

#endif /* ZOOMER_H_ */
