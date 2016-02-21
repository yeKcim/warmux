/*
 * Summary: globals private header
 * Description: internal global variables and thread handling for
 *                    those variables is handled by this module.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LIBXML2_GLOBALS_PRIVATE_H
#define LIBXML2_GLOBALS_PRIVATE_H

#include <libxml2_globals.h>

#ifdef __cplusplus
extern "C" {
#endif

XMLPUBFUN xmlGlobalStatePtr XMLCALL xeGetTLS();

XMLPUBFUN void XMLCALL xmlSetOOM();

XMLPUBFUN void XMLCALL xmlResetOOM();

XMLPUBFUN void  XMLCALL xmlCleanupGlobalData();

XMLPUBFUN xmlGlobalStatePtr  XMLCALL xmlCreateAndInitializeGlobalState();

/* Internal to Libxml2 dll functions - start */

void XMLCALL xeSetTLS(void* ptr);
unsigned int XMLCALL xeStackLimitAddress();
void XMLCALL xeCleanupDOMStringConverter();

#define  CHECK_ERRNO   xeCheckErrno()
void XMLCALL xeCheckErrno();

#define snprintf symbian_snprintf
#define vsnprintf symbian_vsnprintf
int XMLCALL symbian_vsnprintf(char *str, size_t size, const char *format, va_list ap);
int XMLCALL symbian_snprintf(char *str, size_t size, const char *format, ...);

void  XMLCALL free_debug(void* mem);

//Global const data
typedef struct _xeGlobalConstData xeGlobalConstData;
struct _xeGlobalConstData
{
    const char* const xeParserVersion;
};
extern const xeGlobalConstData xeGlobalConsts;
const xeGlobalConstData* xeGetGlobalConsts();
#define xeParserVersion (xeGlobalConsts.xeParserVersion)


xmlParserInputPtr XMLCALL xmlDefaultExternalEntityLoader(const char *URL, const char *ID, xmlParserCtxtPtr ctxt);

/* Internal to Libxml2 dll functions - end */

#ifdef __cplusplus
}
#endif

#endif /*LIBXML2_GLOBALS_PRIVATE_H*/
