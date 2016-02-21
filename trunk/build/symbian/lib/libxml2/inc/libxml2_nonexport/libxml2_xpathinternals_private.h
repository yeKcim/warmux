/*
 * Summary: valid private header
 * Description: internal interfaces for XML Path Language implementation
 *              used to build new modules on top of XPath like XPointer and XSLT
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
// internal interfaces for XML Path Language implementation
// used to build new modules on top of XPath like XPointer and
// XSLT
//



/**
 @file
 @internalTechnology
 @released
*/

#ifndef LIBXML2_XPATHINTERNALS_PRIVATE_H
#define LIBXML2_XPATHINTERNALS_PRIVATE_H

#include <libxml2_xpathinternals.h>

#ifdef __cplusplus
extern "C" {
#endif

XMLPUBFUN xmlXPathObjectPtr XMLCALL xmlXPathCompiledEvalWithDependencies(xmlXPathCompExprPtr comp, xmlXPathContextPtr ctx, xmlNodeSetPtr deplist);

/* Internal to Libxml2 dll functions - start */

//moved from libxml2_xpath.h
void  xeXPathCleanup (void);

/* Internal to Libxml2 dll functions - end */

#ifdef __cplusplus
}
#endif

#endif /*LIBXML2_XPATHINTERNALS_PRIVATE_H*/
