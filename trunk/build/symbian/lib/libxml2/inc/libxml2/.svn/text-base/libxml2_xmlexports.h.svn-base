/*
 * Summary: macros for marking symbols as exportable/importable.
 * Description: macros for marking symbols as exportable/importable.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Igor Zlatovic <igor@zlatkovic.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/** @file
@publishedAll
@released
*/

#ifndef XML_EXPORTS_H
#define XML_EXPORTS_H

/**
 * XMLPUBFUN, XMLPUBVAR, XMLCALL
 *
 * Macros which declare an exportable function, an exportable variable and
 * the calling convention used for functions.
 *
 * Please use an extra block for every platform/compiler combination when
 * modifying this, rather than overlong #ifdef lines. This helps
 * readability as well as the fact that different compilers on the same
 * platform might need different definitions.
 */

/**
 * XMLPUBFUN:
 *
 * Macros which declare an exportable function
 */
#define XMLPUBFUN
/**
 * XMLPUBVAR:
 *
 * Macros which declare an exportable variable
 */
#define XMLPUBVAR extern
/**
 * XMLCALL:
 *
 * Macros which declare the called convention for exported functions
 */
#define XMLCALL
#define XMLPUBFUNEXPORT __declspec(dllexport)

/** DOC_DISABLE */

/* Windows platform with MS compiler */
#if defined(_WIN32) && defined(_MSC_VER)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(IN_LIBXML) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport)
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR extern
    #endif
  #endif
  #define XMLCALL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Windows platform with Borland compiler */
#if defined(_WIN32) && defined(__BORLANDC__)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(IN_LIBXML) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport) extern
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR extern
    #endif
  #endif
  #define XMLCALL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Windows platform with GNU compiler (Mingw) */
#if defined(_WIN32) && defined(__MINGW32__)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(IN_LIBXML) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport)
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR extern
    #endif
  #endif
  #define XMLCALL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Cygwin platform, GNU compiler */
#if defined(_WIN32) && defined(__CYGWIN__)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(IN_LIBXML) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport)
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR
    #endif
  #endif
  #define XMLCALL __cdecl
#endif

/* Symbian: WINS/WINSCW/WINC/ARMI/ARMv5 */
#if defined(__SYMBIAN32__) && (__ARMCC_VERSION > 230000)
#  define XMLPUBFUN __declspec(dllimport)
#  define XMLPUBVAR __declspec(dllimport) extern
#else

#  undef  XMLPUBFUN
#ifdef IN_LIBXML
#  define XMLPUBFUN __declspec(dllexport)
#else
#  define XMLPUBFUN __declspec(dllimport)
#endif

#  undef  XMLPUBVAR
#  if defined(IN_LIBXML) 
#    define XMLPUBVAR __declspec(dllexport) extern
#  else
#    define XMLPUBVAR __declspec(dllimport) extern
#endif

#endif

/* Compatibility */
#if !defined(LIBXML_DLL_IMPORT)
#define LIBXML_DLL_IMPORT XMLPUBVAR
#endif

#endif /* XML_EXPORTS_H */

