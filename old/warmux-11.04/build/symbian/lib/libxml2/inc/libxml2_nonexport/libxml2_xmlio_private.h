/*
 * Summary: valid private header
 * Description: internal interface for the I/O interfaces used by the parser
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

#ifndef LIBXML2_TREE_PRIVATE_H
#define LIBXML2_TREE_PRIVATE_H

#include <libxml2_xmlio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Internal to Libxml2 dll functions - start */

void* XMLCALL xmlFileOpen_real(const char *filename);

#ifdef LIBXML_OUTPUT_ENABLED
int XMLCALL    xmlFileWrite    (void * context, const char * buffer, int len);
#endif /* LIBXML_OUTPUT_ENABLED */

/* Internal to Libxml2 dll functions - end */

#ifdef __cplusplus
}
#endif

#endif /*LIBXML2_TREE_PRIVATE_H*/
