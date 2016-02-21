/*
 * xmlenglibxml.h: internal header only used during the compilation of libxml
 *
 * See COPYRIGHT for the status of this software
 *
 * Author: breese@users.sourceforge.net
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 *
 */

/** @file
@internalComponent
@released
*/

#ifndef XML_LIBXML_H
#define XML_LIBXML_H

#include "libxml2_xmlversion.h"

/* include private headers through this header */
#include "libxml2_globals_private.h"
#include "libxml2_tree_private.h"
#include "libxml2_valid_private.h"
#include "libxml2_xmlsave_private.h"
#include "libxml2_xpathinternals_private.h"

#ifndef WITH_TRIO
#include <stdio.h>
#else
/**
 * TRIO_REPLACE_STDIO:
 *
 * This macro is defined if the trio string formatting functions are to
 * be used instead of the default stdio ones.
 */
#define TRIO_REPLACE_STDIO
#include "Libxml2_trio.h"
#endif

/*
 * internal error reporting routines, shared but not part of the API.
 */
void __xmlIOErr(int domain, int code, const char *extra);
void __xmlLoaderErr(void *ctx, const char *msg, const char *filename);

#endif /* XML_LIBXML_H */

