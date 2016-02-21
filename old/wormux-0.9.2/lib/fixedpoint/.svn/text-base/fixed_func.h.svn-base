/*
Copyright (c) 2010, Wormux Team

Large parts of the code are from the fixed point library of Markus Trenkwalder
Copyright (c) 2007, Markus Trenkwalder

Portions taken from the Vicent 3D rendering library
Copyright (c) 2004, David Blythe, Hans Martin Will

All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.

* Neither the name of the library's copyright owner nor the names of its 
  contributors may be used to endorse or promote products derived from this 
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FIXEDP_FUNC_H_INCLUDED
#define FIXEDP_FUNC_H_INCLUDED

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _MSC_VER
#include <stdint.h>
#else
#include "stdint.h"
#endif

namespace fixedpoint {

// The template argument p in all of the following functions refers to the 
// fixed point precision (e.g. p = 8 gives 24.8 fixed point functions).

// Perform a fixed point multiplication without a 64-bit intermediate result.
// This is fast but beware of overflow!
template <int p> 
inline int64_t fixmulf(int64_t a, int64_t b)
{
	return (a * b) >> p;
}

// Perform a fixed point multiplication using a 64-bit intermediate result to
// prevent overflow problems.
template <int p>
inline int64_t fixmul(int64_t a, int64_t b)
{
	return ((a * b) >> p);
}

// Fixed point division
template <int p>
inline int fixdiv(int64_t a, int64_t b)
{
#if 1
	return ((a << p) / b);
#else	
	// The following produces the same results as the above but gcc 4.0.3 
	// generates fewer instructions (at least on the ARM processor).
	union {
		int64_t a;
		struct {
			int32_t l;
			int32_t h;
		};
	} x;

	x.l = a << p;
	x.h = a >> (sizeof(int32_t) * 8 - p);
	return (int32_t)(x.a / b);
#endif
}

namespace detail {
	inline uint32_t CountLeadingZeros(uint32_t x)
	{
		uint32_t exp = 31;
	
		if (x & 0xffff0000) { 
			exp -= 16; 
			x >>= 16; 
		}
	
		if (x & 0xff00) { 
			exp -= 8; 
			x >>= 8; 
		}
		
		if (x & 0xf0) { 
			exp -= 4; 
			x >>= 4; 
		}
	
		if (x & 0xc) { 
			exp -= 2; 
			x >>= 2; 
		}
		
		if (x & 0x2) { 
			exp -= 1; 
		}
	
		return exp;
	}
}

// q is the precision of the input
// output has 32-q bits of fraction
template <int p>
inline int64_t fixinv(int64_t a)
{
	return fixdiv<p>(1 << p, a);
}
/* TODO fast 64 bit version of:
template <int q>
inline int fixinv(int32_t a)
{
	int32_t x;

	bool sign = false;

	if (a < 0) {
		sign = true;
		a = -a;
	}

	static const uint16_t rcp_tab[] = { 
		0x8000, 0x71c7, 0x6666, 0x5d17, 0x5555, 0x4ec4, 0x4924, 0x4444
	};
		
	int32_t exp = detail::CountLeadingZeros(a);
	x = ((int32_t)rcp_tab[(a>>(28-exp))&0x7]) << 2;
	exp -= 16;

	if (exp <= 0)
		x >>= -exp;
	else
		x <<= exp;

	// two iterations of newton-raphson  x = x(2-ax) 
	x = fixmul<(32-q)>(x,((2<<(32-q)) - fixmul<q>(a,x)));
	x = fixmul<(32-q)>(x,((2<<(32-q)) - fixmul<q>(a,x)));

	if (sign)fix2float
		return -x;
	else
		return x;
}
*/

// Conversion from and to float

template <int p>
float fix2float(int64_t f)
{
	return (float)f / (1 << p);
}

template <int p>
int64_t float2fix(float f)
{
	return (int64_t)(f * (1 << p));
}

int64_t fixcos16(int64_t a);
int64_t fixsin16(int64_t a);
int64_t fixacos16(int64_t a);
int64_t fixasin16(int64_t a);
int64_t fixatan16(int64_t a);
int64_t fixrsqrt16(int64_t a);
int64_t fixsqrt16(int64_t a);

} // end namespace fixedpoint

#endif
