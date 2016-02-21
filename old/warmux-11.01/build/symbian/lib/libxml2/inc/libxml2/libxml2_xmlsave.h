/*
 * Summary: the XML document serializer
 * Description: API to save document or subtree of document
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

#ifndef XML_XMLSAVE_H
#define XML_XMLSAVE_H

#include <stdapis/libxml2/libxml2_xmlio.h>

#ifdef LIBXML_OUTPUT_ENABLED
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INDENT 60

#ifdef XMLENGINE_NODEFILTER

/**
Pointer to implementation of interface NodeFilter (as introduced in DOM Level 2)

@param aNode   Node to test
@return
    1  =  KFilterAccept  - use node
    2  =  KFilterReject  - do not use node but proceed with child nodes
    3  =  KFilterSkip    - skip node and its subtree
    4  =  KFilterSkipElementContents - [non-standard option]
*/
typedef int (XMLCALL *xmlNodeFilterFunc)(xmlNodePtr aNode);
typedef int (XMLCALL *xmlNodeFilterProxyFunc)(void* fn, xmlNodePtr aNode);

// Constansts for results that are returned by callbacks of node filter during serialization
extern const int KFilterAccept;
extern const int KFilterReject;
extern const int KFilterSkip;
extern const int KFilterSkipElementContents;

typedef struct _xmlNodeFilterData xmlNodeFilterData;

/**
 * xmlSaveTextNodeCallback:
 * @param context an Save context
 * @param aNode node to be serialized
 *
 * Callback used in serializing text nodes
 *
 * Returns NULL or pointer to string that replaces binary data
 */
typedef unsigned char* (*xmlSaveTextNodeCallback) (void * context, xmlNodePtr aNode, int* written);

struct _xmlNodeFilterData{
      void* fn;
      xmlNodeFilterProxyFunc proxyFn;
};
#endif

typedef struct _xmlSaveCtxt xmlSaveCtxt;
typedef xmlSaveCtxt *xmlSaveCtxtPtr;
struct _xmlSaveCtxt {
    void *_private;
    int type;
    int fd;
    const xmlChar *filename;
    const xmlChar *encoding;
    xmlCharEncodingHandlerPtr handler;
    xmlOutputBufferPtr buf;
    xmlDocPtr doc;
    int options;
    int level;
    int format;
    
    char indent[MAX_INDENT + 1];    /* array for indenting output */
    int indent_nr;
    int indent_size;
    xmlCharEncodingOutputFunc escape;   /* used for element content */
    xmlCharEncodingOutputFunc escapeAttr;/* used for attribute content */
    xmlSaveTextNodeCallback textNodeCallback;
    void * context; /* save context */
#ifdef XMLENGINE_NODEFILTER
    xmlNodeFilterData* nodeFilter;
#endif
};

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToFd(
                    int fd,
                    const char *encoding,
                    int options);

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToFilename(
                    const char *filename,
                    const char *encoding,
                    int options);

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToBuffer(
                    xmlBufferPtr buffer,
                    const char *encoding,
                    int options);

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToIO(
                    xmlOutputWriteCallback iowrite,
                    xmlOutputCloseCallback ioclose,
                    void *ioctx,
                    const char *encoding,
                    int options);

XMLPUBFUN long XMLCALL
        xmlSaveDoc(
                    xmlSaveCtxtPtr ctxt,
                    xmlDocPtr doc);

XMLPUBFUN long XMLCALL
        xmlSaveTree(
                    xmlSaveCtxtPtr ctxt,
                    xmlNodePtr node);
// XMLENGINE: were made exported
void xmlNsListDumpOutput(
                    xmlOutputBufferPtr buf,
                    xmlNsPtr cur);
//
XMLPUBFUN int XMLCALL
        xmlSaveFlush(
                    xmlSaveCtxtPtr ctxt);

XMLPUBFUN int XMLCALL
        xmlSaveClose        (xmlSaveCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
        xmlSaveSetEscape    (xmlSaveCtxtPtr ctxt,
                     xmlCharEncodingOutputFunc escape);
XMLPUBFUN int XMLCALL
        xmlSaveSetAttrEscape    (xmlSaveCtxtPtr ctxt,
                     xmlCharEncodingOutputFunc escape);

#ifdef __cplusplus
}
#endif

#endif /* LIBXML_OUTPUT_ENABLED */

#endif /* XML_XMLSAVE_H */
