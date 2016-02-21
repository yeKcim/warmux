/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.


    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*
    drawbitmapfiler.cpp
    

    Copyright Markus Mertama
*/

#include"drawbitmapfilter.h"

using namespace DrawBitmapFilter;
   
LOCAL_C	void DrawScanLineFilter(TUint16* aTarget, const TUint32* aSource,
 TInt aTargetLen, TInt aScale, TInt aSrcPitch, TInt aSteps)
	{
	TInt delta = aScale;
	TInt sourcePos = 0;
	TInt len = aTargetLen;
	TInt srcIndex = 0;		
	

	for(TInt i = 0; i < len; i++)
		{
		TInt steps = 0;		
				
		sourcePos += delta;
		
		while(sourcePos >> 16 > 0)
			{	
			++steps;
			++srcIndex;
			sourcePos -= 1 << 16;
			};
			
		TUint32 px;	
		TUint32 r = 0;
		TUint32 g = 0;
		TUint32 b = 0;
		
		TInt multip = 1;
		
		if(steps > 1)
			{
			px = aSource[srcIndex - 1];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			px = aSource[srcIndex + 1];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			multip = 2;
			}
			
		if(aSteps > 1)
			{
			px = aSource[srcIndex - aSrcPitch];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			px = aSource[srcIndex + aSrcPitch];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			multip <<= 1;
			}
		
		px = aSource[srcIndex];	
		
		TUint32 r0 = ((px & 0xF80000) >> 19);
		TUint32 g0 = ((px & 0xFC00) >> 10);
		TUint32 b0 = ((px & 0xF8) >> 3);
		
	
		switch(multip)
			{
			case 1:
				break;
			case 2:
				r0 <<= 1;
				g0 <<= 1;
				b0 <<= 1;
				r0 += r;
				g0 += g;
				b0 += b;
				r0 >>= 2;
				g0 >>= 2;
				b0 >>= 2;	
				break;
			case 4:
				r0 <<= 2;
				g0 <<= 2;
				b0 <<= 2;
				r0 += r;
				g0 += g;
				b0 += b;
				r0 >>= 3;
				g0 >>= 3;
				b0 >>= 3;	
				break;
			}
		
		px = r0 << 11 | g0 << 5 | b0;
			
	 	aTarget[i] = px;
		}
	}

void DrawBitmapFilter::Draw(TUint16* aTarget, const TUint32* aSrc,
 	const TSize& aTargetSize, const TSize& aSrcSize,
  	TInt aTargetStride, TInt aSrcStride)
	{
	TInt i;

	TInt sourcePos = 0; 


	ASSERT(aTargetSize.iHeight > 0);
	ASSERT(aTargetSize.iWidth > 0);
	
	ASSERT(aSrcSize.iHeight > 1);
	ASSERT(aSrcSize.iWidth > 1);

	
  	const TUint32* source = aSrc;
  	const TInt sourcePitch = aSrcStride >> 2;
  	
  	  	
  	TUint16* target = aTarget;
  	const TInt targetPitch = aTargetStride >> 1;
  	
  	const TInt sh = ((aSrcSize.iHeight - 1) << 16) / aTargetSize.iHeight; 
  	const TInt sw = ((aSrcSize.iWidth - 1) << 16) /aTargetSize.iWidth; 

	const TInt sourceDelta = sh;

	for(i = 0; i < aTargetSize.iHeight ; i++)  	
		{				
		TInt steps = 0;
		
		sourcePos += sourceDelta;
		
		while(sourcePos >> 16 > 0) //this does source sampling
			{	
			++steps;				   	
			source += sourcePitch;
			sourcePos -= 1 << 16;  //jump remainder
			}	
		
		DrawScanLineFilter(target, source, aTargetSize.iWidth, sw, sourcePitch, steps);	
	
		target += targetPitch;		

		}
		
	}   
	
LOCAL_C void DrawScanLineFilter(TUint32* aTarget, const TUint32* aSource,
 TInt aTargetLen, TInt aScale, TInt aSrcPitch, TInt aSteps)
	{
	TInt delta = aScale;
	TInt sourcePos = 0;
	TInt len = aTargetLen;
	TInt srcIndex = 0;		
	

	for(TInt i = 0; i < len; i++)
		{
		TInt steps = 0;		
				
		sourcePos += delta;
		
		while(sourcePos >> 16 > 0)
			{	
			++steps;
			++srcIndex;
			sourcePos -= 1 << 16;
			};
			
		TUint32 px;	
		TUint32 r = 0;
		TUint32 g = 0;
		TUint32 b = 0;
		
		TInt multip = 1;
		
		if(steps > 1)
			{
			px = aSource[srcIndex - 1];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			px = aSource[srcIndex + 1];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			multip = 2;
			}
			
		if(aSteps > 1)
			{
			px = aSource[srcIndex - aSrcPitch];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			px = aSource[srcIndex + aSrcPitch];
			r += ((px & 0xF80000) >> 19);
			g += ((px & 0xFC00) >> 10);
			b += ((px & 0xF8) >> 3);
			
			multip <<= 1;
			}
		
		px = aSource[srcIndex];	
		
		TUint32 r0 = ((px & 0xF80000) >> 19);
		TUint32 g0 = ((px & 0xFC00) >> 10);
		TUint32 b0 = ((px & 0xF8) >> 3);
		
	
		switch(multip)
			{
			case 1:
				break;
			case 2:
				r0 <<= 1;
				g0 <<= 1;
				b0 <<= 1;
				r0 += r;
				g0 += g;
				b0 += b;
				r0 >>= 2;
				g0 >>= 2;
				b0 >>= 2;	
				break;
			case 4:
				r0 <<= 2;
				g0 <<= 2;
				b0 <<= 2;
				r0 += r;
				g0 += g;
				b0 += b;
				r0 >>= 3;
				g0 >>= 3;
				b0 >>= 3;	
				break;
			}
		
		px = r0 << 11 | g0 << 5 | b0;
			
	 	aTarget[i] = px;
		}
	}

void DrawBitmapFilter::Draw(TUint32* aTarget, const TUint32* aSrc,
 	const TSize& aTargetSize, const TSize& aSrcSize,
  	TInt aTargetStride, TInt aSrcStride)
	{
	TInt i;

	TInt sourcePos = 0; 


	ASSERT(aTargetSize.iHeight > 0);
	ASSERT(aTargetSize.iWidth > 0);
	
	ASSERT(aSrcSize.iHeight > 1);
	ASSERT(aSrcSize.iWidth > 1);

	
  	const TUint32* source = aSrc;
  	const TInt sourcePitch = aSrcStride >> 2;
  	
  	  	
  	TUint32* target = aTarget;
  	const TInt targetPitch = aTargetStride >> 2;
  	
  	const TInt sh = ((aSrcSize.iHeight - 1) << 16) / aTargetSize.iHeight; 
  	const TInt sw = ((aSrcSize.iWidth - 1) << 16) /aTargetSize.iWidth; 

	const TInt sourceDelta = sh;

	for(i = 0; i < aTargetSize.iHeight ; i++)  	
		{				
		TInt steps = 0;
		
		sourcePos += sourceDelta;
		
		while(sourcePos >> 16 > 0) //this does source sampling
			{	
			++steps;				   	
			source += sourcePitch;
			sourcePos -= 1 << 16;  //jump remainder
			}	
		
		DrawScanLineFilter(target, source, aTargetSize.iWidth, sw, sourcePitch, steps);	
	
		target += targetPitch;		

		}
		
	} 	 

