/*
 * blitter.h
 *
 *  Created on: 21.2.2011
 *      Author: mertama
 */

#ifndef BLITTER_H_
#define BLITTER_H_


#include <sdlepocapi.h>

NONSHARABLE_CLASS(CBlitter) : public CBase, public MBlitter
 {
public:
     virtual void Release() {}
 };

typedef CBlitter* (*CreateExtBlitterL) (RWindow& aWindow, TInt aFlags);


#endif /* BLITTER_H_ */
