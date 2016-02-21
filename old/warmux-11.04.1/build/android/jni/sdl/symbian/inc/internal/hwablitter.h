#ifndef HWABLITTER
#define HWABLITTER

#include <sdlepocapi.h>
#include <gdi.h>
#include "blitter.h"

class HWAImp;
class RWindow;


NONSHARABLE_CLASS(CHWABlitter) : public CBlitter
	{
public:
	enum{ENoScale = 0x0, EPreventScale = 0x1, EPreserveRatio = 0x2};
	IMPORT_C static CHWABlitter* NewL(RWindow& aWindow, TInt aFlags);
	IMPORT_C ~CHWABlitter();
	IMPORT_C TBool BitBlt(CBitmapContext& aGc,CFbsBitmap& aBmp, const TRect& aTargetRect, const TSize& aSize);
private:
	CHWABlitter(TInt aFlags);
	void Release();
private:
	TInt iFlags;
	HWAImp* iImp;
	};
	
	
#endif
