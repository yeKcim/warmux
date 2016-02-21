/*
Copyright (c) 2010, Wormux Team

Large parts of the code are from the fixed point library of Markus Trenkwalder
Copyright (c) 2007, Markus Trenkwalder

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

#ifndef FIXEDP_CLASS_H_INCLUDED
#define FIXEDP_CLASS_H_INCLUDED

#ifdef _MSC_VER
#pragma once
#endif

#include "fixed_func.h"
#include <sstream>

namespace fixedpoint {

// The template argument p in all of the following functions refers to the 
// fixed point precision (e.g. p = 8 gives 24.8 fixed point functions).
#ifdef __APPLE__
  #define SIZE_T_FIXEDPOINT_METHODS
#endif
#if __WORDSIZE == 64
  #define SIZE_T_FIXEDPOINT_METHODS
#endif

template <int p>
struct fixed_point {
	int64_t intValue;
	
	fixed_point() {}
	/*explicit*/ fixed_point(int32_t i) : intValue(((int64_t)i) << p) {}
	/*explicit*/ fixed_point(int64_t i) : intValue(i << p) {}
	/*explicit*/ fixed_point(float f) : intValue(float2fix<p>(f)) {}
	/*explicit*/ fixed_point(double f) : intValue(float2fix<p>((float)f)) {}
#if __WORDSIZE != 64
	/*explicit*/ fixed_point(long int l) : intValue(((int64_t)l) << p) {}
#endif
	/*explicit*/ fixed_point(unsigned int l) : intValue(((int64_t)l) << p) {}
#ifdef SIZE_T_FIXEDPOINT_METHODS
    /*explicit*/ fixed_point(size_t i) : intValue(((int64_t)i) << p) {}
#endif

	fixed_point& operator += (fixed_point r) { intValue += r.intValue; return *this; }
	fixed_point& operator -= (fixed_point r) { intValue -= r.intValue; return *this; }
	fixed_point& operator *= (fixed_point r) { intValue = fixmul<p>(intValue, r.intValue); return *this; }
	fixed_point& operator /= (fixed_point r) { intValue = fixdiv<p>(intValue, r.intValue); return *this; }

	fixed_point& operator *= (int32_t r) { intValue *= r; return *this; }
	fixed_point& operator /= (int32_t r) { intValue /= r; return *this; }

	fixed_point operator - () const { fixed_point x; x.intValue = -intValue; return x; }
	fixed_point operator + (fixed_point r) const { fixed_point x = *this; x += r; return x;}
	fixed_point operator - (fixed_point r) const { fixed_point x = *this; x -= r; return x;}
	fixed_point operator * (fixed_point r) const { fixed_point x = *this; x *= r; return x;}
	fixed_point operator / (fixed_point r) const { fixed_point x = *this; x /= r; return x;}

	bool operator == (fixed_point r) const { return intValue == r.intValue; }
	bool operator == (int i) const { return intValue == (((int64_t)i) << p); }
	bool operator != (fixed_point r) const { return !(*this == r); }
	bool operator <  (fixed_point r) const { return intValue < r.intValue; }
	bool operator < (int i) const { return intValue < (((int64_t)i)  << p); }
	bool operator >  (fixed_point r) const { return intValue > r.intValue; }
	bool operator > (int i) const { return intValue > (((int64_t)i)  << p); }
	bool operator <= (fixed_point r) const { return intValue <= r.intValue; }
	bool operator <= (int i) const { return intValue <= (((int64_t)i)  << p); }
	bool operator >= (fixed_point r) const { return intValue >= r.intValue; }
	bool operator >= (int i) const { return intValue >= (((int64_t)i) << p); }

	fixed_point operator + (int32_t r) const { fixed_point x = *this; x += r; return x;}
	fixed_point operator - (int32_t r) const { fixed_point x = *this; x -= r; return x;}
	fixed_point operator * (int32_t r) const { fixed_point x = *this; x *= r; return x;}
	fixed_point operator / (int32_t r) const { fixed_point x = *this; x /= r; return x;}
	fixed_point operator + (unsigned int r) const { fixed_point x = *this; x += r; return x;}
	fixed_point operator - (unsigned int r) const { fixed_point x = *this; x -= r; return x;}
	fixed_point operator * (unsigned int r) const { fixed_point x = *this; x *= r; return x;}
	fixed_point operator / (unsigned int r) const { fixed_point x = *this; x /= r; return x;}

	operator int() const { return intValue / (1<< p); }
	operator long() const { return intValue / (1<< p); }
	
	
	// Must be used explicily as we don't want to calculate with doubles!
	double toDouble() const { return (double)intValue / (double)(1 << p); }
};

// Specializations for use with plain integers
template <int p>
inline fixed_point<p> operator + (int32_t a, fixed_point<p> b)
{ return b + a; }

template <int p>
inline fixed_point<p> operator - (int32_t a, fixed_point<p> b)
{ return -b + a; }

template <int p>
inline fixed_point<p> operator * (int32_t a, fixed_point<p> b)
{ return b * a; }

template <int p>
inline fixed_point<p> operator / (int32_t a, fixed_point<p> b)
{ fixed_point<p> r(a); r /= b; return r; }



template <int p>
inline fixed_point<p> operator + (unsigned int a, fixed_point<p> b)
{ return b + a; }

template <int p>
inline fixed_point<p> operator - (unsigned int a, fixed_point<p> b)
{ return -b + a; }

template <int p>
inline fixed_point<p> operator * (unsigned int a, fixed_point<p> b)
{ return b * a; }

template <int p>
inline fixed_point<p> operator / (unsigned int a, fixed_point<p> b)
{ fixed_point<p> r(a); r /= b; return r; }

#ifdef SIZE_T_FIXEDPOINT_METHODS
template <int p>
inline fixed_point<p> operator + (size_t a, fixed_point<p> b)
{ return b + a; }

template <int p>
inline fixed_point<p> operator - (size_t a, fixed_point<p> b)
{ return -b + a; }

template <int p>
inline fixed_point<p> operator * (size_t a, fixed_point<p> b)
{ return b * a; }

template <int p>
inline fixed_point<p> operator / (size_t a, fixed_point<p> b)
{ fixed_point<p> r(a); r /= b; return r; }
#endif

template <int p>
inline fixed_point<p> round(fixed_point<p> r)
{
	fixed_point<p> result = r>0? 0.5:-0.5;
	result += r;
	result /= (1<< p);
	return result.intValue;
}

namespace detail {
	static bool isDigit(int c) {
		return c >= '0' &&  c <= '9';
	}
}

template <int p>
std::istream & operator>> (std::istream & is, fixed_point<p> & r)
{
	std::stringstream buffer;
	int next = is.peek();
	bool minus = (next == '-');
	if (minus) {
		buffer << "-";
		is.get();// skip one character
	}
	buffer << '0'; // assuming a leading zero makes it possible to parse numbers starting with a dot and parses "" as 0.
	next = is.peek();  
	while (next != is.eof() && detail::isDigit(next)) {
		char c = is.get();
		buffer << c;
		next = is.peek();
	}
	int number;
	buffer >> number;
	r = number;
	next = is.peek();
	if (next == '.') {
		is.get();// skip point character
		next = is.peek();
		fixed_point<p> factor = 1;
		fixed_point<p> ten = 10;
		while (next != is.eof() && detail::isDigit(next)) {
			factor /= ten;     
			char c;
			is >> c;
			int digit = c - '0';
			if (minus)
				r -= digit * factor; 
			else
				r += digit * factor; 
			buffer << c;
			next = is.peek();
		}
	}

	return is;
}


template <int p>
void printTo(std::ostream & os, const fixed_point<p> & r, int digits = -1)
{
	if (digits == -1)
		digits = p / 4;
	fixed_point<p> rounding_correction = 0.5;
	fixed_point<p> ten = 10;
	for (int k = 0; k < digits; k++) {
		rounding_correction = rounding_correction / ten; 
	}
	if (r < 0)
		rounding_correction = - rounding_correction;
	fixed_point<p> to_print = r + rounding_correction;

	int64_t left_of_dot = (int) r;
	os << left_of_dot;
	if (digits == 0) {
		return;
	}
	os << ".";

	to_print -= left_of_dot;
	fixed_point<p> zero = 0;

	int zeros = 0;
	if (to_print < 0) {
		to_print = - to_print;
	}
	for (int i=0; i<digits; i++) {
		to_print *= 10;
		int digit = (int)to_print;
		to_print = to_print - digit;
		if (digit == 0) {
			zeros++;
		} else {
			for (int j=0; j<zeros; j++) {
				os << "0";
			}
			zeros = 0;
			os << digit;
		}
	}
	// show at least one digit after the point:
	if (zeros == digits) {
		os << "0";
	}
}

template <int p>
std::ostream & operator<< (std::ostream & os, const fixed_point<p> & r)
{
	printTo(os, r);
	return os;
}

// math functions
// no default implementation

template <int p>
inline fixed_point<p> sin(fixed_point<p> a);

template <int p>
inline fixed_point<p> cos(fixed_point<p> a);

template <int p>
inline fixed_point<p> acos(fixed_point<p> a);

template <int p>
inline fixed_point<p> asin(fixed_point<p> a);

template <int p>
inline fixed_point<p> atan(fixed_point<p> a);

template <int p>
inline fixed_point<p> sqrt(fixed_point<p> a);

template <int p>
inline fixed_point<p> rsqrt(fixed_point<p> a);

template <int p>
inline fixed_point<p> inv(fixed_point<p> a);

template <int p>
inline fixed_point<p> abs(fixed_point<p> a)
{ 
	fixed_point<p> r; 
	r.intValue = a.intValue > 0 ? a.intValue : -a.intValue; 
	return r; 
}

template <int p>
inline fixed_point<p> fmod(fixed_point<p> numerator, fixed_point<p> denominator)
{
	return  numerator - denominator * floor(numerator / denominator) ;
}

template <int p>
inline fixed_point<p> floor(fixed_point<p> a)
{
	fixed_point<p> r = a.intValue >> p;
	return r;
}

// specializations for 16.16 format

template <>
inline fixed_point<16> sin(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixsin16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> cos(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixcos16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> acos(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixacos16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> asin(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixasin16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> atan(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixatan16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> sqrt(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixsqrt16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> rsqrt(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixrsqrt16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> inv(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixinv<16>(a.intValue);
	return r;
}

// The multiply accumulate case can be optimized.
template <int p>
inline fixed_point<p> multiply_accumulate(
	int count, 
	const fixed_point<p> *a,
	const fixed_point<p> *b)
{
	long long result = 0;
	for (int i = 0; i < count; ++i)
		result += static_cast<long long>(a[i].intValue) * b[i].intValue;
	fixed_point<p> r;
	r.intValue = static_cast<int>(result >> p);
	return r;
}

} // end namespace fixedpoint

#endif

