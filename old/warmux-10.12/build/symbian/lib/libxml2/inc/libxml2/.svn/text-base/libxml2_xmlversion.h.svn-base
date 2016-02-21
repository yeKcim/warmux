/*
 * Summary: compile-time version informations
 * Description: compile-time version informations for the XML library
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/** @file
@publishedAll
@released
*/

#ifndef XML_VERSION_H
#define XML_VERSION_H

/*
 * modules.h contains definitions that are auto generated
 * from libxml2 property files using Ant script
 *
 * All other defines in this files are not assumed to be changed anymore
 */

#include <stdapis/libxml2/libxml2_modules.h>
#include <stdapis/libxml2/libxml2_xmlexports.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * use those to be sure nothing nasty will happen if
 * your library and includes mismatch
 */
#ifndef LIBXML2_COMPILING_MSCCDEF
XMLPUBFUN void XMLCALL xmlCheckVersion(int version);
#endif /* LIBXML2_COMPILING_MSCCDEF */

/**
 * LIBXML_DOTTED_VERSION:
 *
 * the version string like "1.2.3"
 */
#define LIBXML_DOTTED_VERSION "2.6.10"

/**
 * LIBXML_VERSION:
 *
 * the version number: 1.2.3 value is 1002003
 */
#define LIBXML_VERSION 20610

/**
 * LIBXML_VERSION_STRING:
 *
 * the version number string, 1.2.3 value is "1002003"
 */
#define LIBXML_VERSION_STRING "20610"

/**
 * LIBXML_TEST_VERSION:
 *
 * Macro to check that the libxml version in use is compatible with
 * the version the software has been compiled against
 */
#define LIBXML_TEST_VERSION xmlCheckVersion(20610);

//#ifndef VMS
//#if 0
///**
// * WITH_TRIO:
// *
// * defined if the trio support need to be configured in
// */
//#define WITH_TRIO
//#else
///**
// * WITHOUT_TRIO:
// *
// * defined if the trio support should not be configured in
// */
//#define WITHOUT_TRIO
//#endif
//#else /* VMS */
///**
// * WITH_TRIO:
// *
// * defined if the trio support need to be configured in
// */
//#define WITH_TRIO 1
//#endif /* VMS */



/**
 * LIBXML_FTP_ENABLED:
 *
 * Whether the FTP support is configured in
 */
// Excluded from XML Engine
//#define LIBXML_FTP_ENABLED


/**
 * LIBXML_HTTP_ENABLED:
 *
 * Whether the HTTP support is configured in
 */
// Excluded from XML Engine
//#define LIBXML_HTTP_ENABLED


/**
 * LIBXML_DOCB_ENABLED:
 *
 * Whether the SGML Docbook support is configured in
 */
// Excluded from XML Engine
//#define LIBXML_DOCB_ENABLED

/**
 * LIBXML_ICONV_ENABLED:
 *
 * Whether iconv support is available
 */
#if 0
#define LIBXML_ICONV_ENABLED
#endif

/**
 * LIBXML_ISO8859X_ENABLED:
 *
 * Whether ISO-8859-* support is made available in case iconv is not
 */
#if 0
#define LIBXML_ISO8859X_ENABLED
#endif


/**
 * ATTRIBUTE_UNUSED:
 *
 * Macro used to signal to GCC unused function parameters
 */
#ifdef __GNUC__
#ifdef HAVE_ANSIDECL_H
#include <ansidecl.h>
#endif
#ifndef ATTRIBUTE_UNUSED
#define ATTRIBUTE_UNUSED __attribute__((unused))
#endif
#else
#define ATTRIBUTE_UNUSED
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* XML_VERSION_H */

