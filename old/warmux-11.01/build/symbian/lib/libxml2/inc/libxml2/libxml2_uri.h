/*
 * Summary: library of generic URI related routines
 * Description: library of generic URI related routines
 *              Implements RFC 2396
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

#ifndef XML_URI_H
#define XML_URI_H

#include <stdapis/libxml2/libxml2_xmlstring.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlURI:
 *
 * A parsed URI reference. This is a struct containing the various fields
 * as described in RFC 2396 but separated for further processing.
 */
typedef struct _xmlURI xmlURI;
typedef xmlURI *xmlURIPtr;
struct _xmlURI {
    char *scheme;       /* the URI scheme */
    char *opaque;       /* opaque part */
    char *authority;    /* the authority part */
    char *server;       /* the server part */
    char *user;         /* the user part */
    int port;           /* the port number */
    char *path;         /* the path string */
    char *query;        /* the query string */
    char *fragment;     /* the fragment identifier */
    int  cleanup;       /* parsing potentially unclean URI */
};

/*
 * This function is in tree.h:
 * xmlChar *    xmlNodeGetBase  (xmlDocPtr doc,
 *                               xmlNodePtr cur);
 */
XMLPUBFUN xmlURIPtr XMLCALL
                xmlCreateURI            (void);
XMLPUBFUN xmlChar * XMLCALL
                xmlBuildURI             (const xmlChar *URI,
                                         const xmlChar *base);
XMLPUBFUN xmlURIPtr XMLCALL
                xmlParseURI             (const char *str);
XMLPUBFUN int XMLCALL
                xmlParseURIReference    (xmlURIPtr uri,
                                         const char *str);
XMLPUBFUN xmlChar * XMLCALL
                xmlSaveUri              (xmlURIPtr uri);

#ifndef XMLENGINE_EXCLUDE_FILE_FUNC
XMLPUBFUN void XMLCALL
                xmlPrintURI             (FILE *stream,
                                         xmlURIPtr uri);
#endif

XMLPUBFUN xmlChar * XMLCALL
                xmlURIEscapeStr         (const xmlChar *str,
                                         const xmlChar *list);
XMLPUBFUN char * XMLCALL
                xmlURIUnescapeString    (const char *str,
                                         int len,
                                         char *target);
XMLPUBFUN int XMLCALL
                xmlNormalizeURIPath     (char *path);
XMLPUBFUN xmlChar * XMLCALL
                xmlURIEscape            (const xmlChar *str);
XMLPUBFUN void XMLCALL
                xmlFreeURI              (xmlURIPtr uri);
XMLPUBFUN xmlChar* XMLCALL
                xmlCanonicPath          (const xmlChar *path);

#ifdef __cplusplus
}
#endif
#endif /* XML_URI_H */

