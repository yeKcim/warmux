/*
 * Summary: specific APIs to process HTML tree, especially serialization
 * Description: this module implements a few function needed to process
 *              tree in an HTML specific way.
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

#ifndef HTML_TREE_H
#define HTML_TREE_H

#include <stdio.h>
#include "libxml2_tree.h"
#include "libxml2_htmlparser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * HTML_TEXT_NODE:
 *
 * Macro. A text node in a HTML document is really implemented
 * the same way as a text node in an XML document.
 */
#define HTML_TEXT_NODE          XML_TEXT_NODE
/**
 * HTML_ENTITY_REF_NODE:
 *
 * Macro. An entity reference in a HTML document is really implemented
 * the same way as an entity reference in an XML document.
 */
#define HTML_ENTITY_REF_NODE    XML_ENTITY_REF_NODE
/**
 * HTML_COMMENT_NODE:
 *
 * Macro. A comment in a HTML document is really implemented
 * the same way as a comment in an XML document.
 */
#define HTML_COMMENT_NODE       XML_COMMENT_NODE
/**
 * HTML_PRESERVE_NODE:
 *
 * Macro. A preserved node in a HTML document is really implemented
 * the same way as a CDATA section in an XML document.
 */
#define HTML_PRESERVE_NODE      XML_CDATA_SECTION_NODE
/**
 * HTML_PI_NODE:
 *
 * Macro. A processing instruction in a HTML document is really implemented
 * the same way as a processing instruction in an XML document.
 */
#define HTML_PI_NODE            XML_PI_NODE

#if defined(LIBXML_HTML_ENABLED) || defined(XMLENGINE_XSLT)

XMLPUBFUN htmlDocPtr XMLCALL
        htmlNewDoc      (const xmlChar *URI,
                     const xmlChar *ExternalID);
XMLPUBFUN htmlDocPtr XMLCALL    
        htmlNewDocNoDtD     (const xmlChar *URI,
                     const xmlChar *ExternalID);
XMLPUBFUN int XMLCALL       
        htmlSetMetaEncoding (htmlDocPtr doc,
                     const xmlChar *encoding);

#ifdef LIBXML_OUTPUT_ENABLED
XMLPUBFUN void XMLCALL      
        htmlDocContentDumpOutput(xmlOutputBufferPtr buf,
                     xmlDocPtr cur,
                     const char *encoding);
XMLPUBFUN void XMLCALL      
        htmlDocContentDumpFormatOutput(xmlOutputBufferPtr buf,
                     xmlDocPtr cur,
                     const char *encoding,
                     int format);

XMLPUBFUN void XMLCALL      
        htmlNodeDumpFormatOutput(xmlOutputBufferPtr buf,
                     xmlDocPtr doc,
                     xmlNodePtr cur,
                     const char *encoding,
                     int format);

#endif /* LIBXML_OUTPUT_ENABLED */

XMLPUBFUN int XMLCALL       
        htmlIsBooleanAttr   (const xmlChar *name);

#endif /* HTML or XSLT */

#ifdef LIBXML_HTML_ENABLED

XMLPUBFUN const xmlChar * XMLCALL   
        htmlGetMetaEncoding (htmlDocPtr doc);

#ifdef LIBXML_OUTPUT_ENABLED
XMLPUBFUN void XMLCALL  
        htmlDocDumpMemory   (xmlDocPtr cur,
                     xmlChar **mem,
                     int *size);
XMLPUBFUN int XMLCALL       
        htmlDocDump     (FILE *f,
                     xmlDocPtr cur);
XMLPUBFUN int XMLCALL       
        htmlSaveFile        (const char *filename,
                     xmlDocPtr cur);
XMLPUBFUN int XMLCALL       
        htmlNodeDump        (xmlBufferPtr buf,
                     xmlDocPtr doc,
                     xmlNodePtr cur);
XMLPUBFUN void XMLCALL      
        htmlNodeDumpFile    (FILE *out,
                     xmlDocPtr doc,
                     xmlNodePtr cur);
XMLPUBFUN int XMLCALL       
        htmlNodeDumpFileFormat  (FILE *out,
                     xmlDocPtr doc,
                     xmlNodePtr cur,
                     const char *encoding,
                     int format);
XMLPUBFUN int XMLCALL       
        htmlSaveFileEnc     (const char *filename,
                     xmlDocPtr cur,
                     const char *encoding);
XMLPUBFUN int XMLCALL       
        htmlSaveFileFormat  (const char *filename,
                     xmlDocPtr cur,
                     const char *encoding,
                     int format);

XMLPUBFUN void XMLCALL
        htmlNodeDumpOutput  (xmlOutputBufferPtr buf,
                     xmlDocPtr doc,
                     xmlNodePtr cur,
                     const char *encoding);

#endif /* LIBXML_OUTPUT_ENABLED */
#endif /* LIBXML_HTML_ENABLED   */

#ifdef __cplusplus
}
#endif

#endif /* HTML_TREE_H */
