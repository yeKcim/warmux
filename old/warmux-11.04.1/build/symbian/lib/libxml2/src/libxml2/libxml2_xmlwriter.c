/*
 * libxml2_xmlwriter.c: XML text writer implementation
 *
 * For license and disclaimer see the license and disclaimer of
 * libxml2.
 *
 * alfred@mickautsch.de
 * See Copyright for the status of this software.
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML

#include <string.h>

#include "xmlenglibxml.h"
#include <libxml2_globals.h>
#include <libxml2_uri.h>

#ifdef LIBXML_HTML_ENABLED
#include "libxml2_htmltree.h"
#endif

#ifdef LIBXML_WRITER_ENABLED

#include "libxml2_xmlwriter.h"

#define B64LINELEN 72
#define B64CRLF "\r\n"

/*
 * Types are kept private
 */
typedef enum {
    XML_TEXTWRITER_NONE = 0,
    XML_TEXTWRITER_NAME,
    XML_TEXTWRITER_ATTRIBUTE,
    XML_TEXTWRITER_TEXT,
    XML_TEXTWRITER_PI,
    XML_TEXTWRITER_PI_TEXT,
    XML_TEXTWRITER_CDATA,
    XML_TEXTWRITER_DTD,
    XML_TEXTWRITER_DTD_TEXT,
    XML_TEXTWRITER_DTD_ELEM,
    XML_TEXTWRITER_DTD_ELEM_TEXT,
    XML_TEXTWRITER_DTD_ATTL,
    XML_TEXTWRITER_DTD_ATTL_TEXT,
    XML_TEXTWRITER_DTD_ENTY,    /* entity */
    XML_TEXTWRITER_DTD_ENTY_TEXT,
    XML_TEXTWRITER_DTD_PENT,    /* parameter entity */
    XML_TEXTWRITER_COMMENT
} xmlTextWriterState;

typedef struct _xmlTextWriterStackEntry xmlTextWriterStackEntry;

struct _xmlTextWriterStackEntry {
    xmlChar *name;
    xmlTextWriterState state;
};

typedef struct _xmlTextWriterNsStackEntry xmlTextWriterNsStackEntry;
struct _xmlTextWriterNsStackEntry {
    xmlChar *prefix;
    xmlChar *uri;
    xmlLinkPtr elem;
};

static void xmlFreeTextWriterStackEntry(xmlLinkPtr lk);
static int xmlCmpTextWriterStackEntry(const void *data0,
                                      const void *data1);
static void xmlFreeTextWriterNsStackEntry(xmlLinkPtr lk);
static int xmlCmpTextWriterNsStackEntry(const void *data0,
                                        const void *data1);
static int xmlTextWriterWriteMemCallback(void *context,
                                         const xmlChar * str, int len);
static int xmlTextWriterCloseMemCallback(void *context);
static int xmlTextWriterWriteDocCallback(void *context,
                                         const xmlChar * str, int len);
static int xmlTextWriterCloseDocCallback(void *context);

static xmlChar *xmlTextWriterVSprintf(const char *format, va_list argptr);
static int xmlOutputBufferWriteBase64(xmlOutputBufferPtr out, int len,
                                      const unsigned char *data);
static void xmlTextWriterStartDocumentCallback(void *ctx);
static int xmlTextWriterWriteIndent(xmlTextWriterPtr writer);
static int
  xmlTextWriterHandleStateDependencies(xmlTextWriterPtr writer,
                                       xmlTextWriterStackEntry * p);

/**
 * xmlNewTextWriter:
 * @param out an xmlOutputBufferPtr
 *
 * Create a new xmlNewTextWriter structure using an xmlOutputBufferPtr
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
XMLPUBFUNEXPORT xmlTextWriterPtr
xmlNewTextWriter(xmlOutputBufferPtr out)
{
    xmlTextWriterPtr ret;

    ret = (xmlTextWriterPtr) xmlMalloc(sizeof(xmlTextWriter));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriter : out of memory!\n"));
        return NULL;
    }
    memset(ret, 0, (size_t) sizeof(xmlTextWriter));

    ret->nodes = xmlListCreate((xmlListDeallocator)
                               xmlFreeTextWriterStackEntry,
                               (xmlListDataCompare)
                               xmlCmpTextWriterStackEntry);
    if (ret->nodes == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriter : out of memory!\n"));
        xmlFree(ret);
        return NULL;
    }

    ret->nsstack = xmlListCreate((xmlListDeallocator)
                                 xmlFreeTextWriterNsStackEntry,
                                 (xmlListDataCompare)
                                 xmlCmpTextWriterNsStackEntry);
    if (ret->nsstack == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriter : out of memory!\n"));
        xmlListDelete(ret->nodes);
        xmlFree(ret);
        return NULL;
    }

    ret->out = out;
    ret->ichar = xmlStrdup(BAD_CAST " ");
    ret->qchar = '"';

    if (!ret->ichar) {
        xmlListDelete(ret->nodes);
        xmlListDelete(ret->nsstack);
        xmlFree(ret);
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriter : out of memory!\n"));
        return NULL;
    }

    return ret;
}

/**
 * xmlNewTextWriterFilename:
 * @param uri the URI of the resource for the output
 * @param compression compress the output?
 *
 * Create a new xmlNewTextWriter structure with uri as output
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
XMLPUBFUNEXPORT xmlTextWriterPtr
xmlNewTextWriterFilename(const char* uri, int compression)
{
    xmlTextWriterPtr ret;
    xmlOutputBufferPtr out;

    out = xmlOutputBufferCreateFilename(uri, NULL, compression);
    if (!out)
        goto OOM_exit;

    ret = xmlNewTextWriter(out);
    if (!ret) {
        xmlOutputBufferClose(out);
OOM_exit:
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterFilename : out of memory!\n"));
        return NULL;
    }

    ret->indent = 0;
    ret->doindent = 0;
    return ret;
}

/**
 * xmlNewTextWriterMemory:
 * @param buf xmlBufferPtr
 * @param compression compress the output?
 *
 * Create a new xmlNewTextWriter structure with buf as output
 * 
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
XMLPUBFUNEXPORT xmlTextWriterPtr
xmlNewTextWriterMemory(xmlBufferPtr buf, int compression ATTRIBUTE_UNUSED)
{
    xmlTextWriterPtr ret;
    xmlOutputBufferPtr out;


    out = xmlOutputBufferCreateIO((xmlOutputWriteCallback)
                                  xmlTextWriterWriteMemCallback,
                                  (xmlOutputCloseCallback)
                                  xmlTextWriterCloseMemCallback,
                                  (void*) buf, NULL);
    if (out == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterMemory : out of memory!\n"));
        return NULL;
    }

    ret = xmlNewTextWriter(out);
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterMemory : out of memory!\n"));
        xmlOutputBufferClose(out);
        return NULL;
    }

    return ret;
}

/**
 * xmlNewTextWriterPushParser:
 * @param ctxt xmlParserCtxtPtr to hold the new XML document tree
 * @param compression compress the output?
 *
 * Create a new xmlNewTextWriter structure with ctxt as output
 * 
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
XMLPUBFUNEXPORT xmlTextWriterPtr
xmlNewTextWriterPushParser(xmlParserCtxtPtr ctxt,
                           int compression ATTRIBUTE_UNUSED)
{
    xmlTextWriterPtr ret;
    xmlOutputBufferPtr out;

    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterPushParser : invalid context!\n"));
        return NULL;
    }

    out = xmlOutputBufferCreateIO((xmlOutputWriteCallback)
                                  xmlTextWriterWriteDocCallback,
                                  (xmlOutputCloseCallback)
                                  xmlTextWriterCloseDocCallback,
                                  (void *) ctxt, NULL);
    if (out == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterPushParser : error at xmlOutputBufferCreateIO!\n"));
        return NULL;
    }

    ret = xmlNewTextWriter(out);
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterPushParser : error at xmlNewTextWriter!\n"));
        xmlOutputBufferClose(out);
        return NULL;
    }

    ret->ctxt = ctxt;

    return ret;
}

#ifdef LIBXML_SAX1_ENABLED
/**
 * xmlNewTextWriterDoc:
 * @param doc address of a xmlDocPtr to hold the new XML document tree
 * @param compression compress the output?
 *
 * Create a new xmlNewTextWriter structure with *doc as output
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
XMLPUBFUNEXPORT xmlTextWriterPtr
xmlNewTextWriterDoc(xmlDocPtr * doc, int compression)
{
	xmlTextWriterPtr ret;
    xmlSAXHandler saxHandler;
    xmlParserCtxtPtr ctxt;

    memset(&saxHandler, '\0', sizeof(saxHandler));
    xmlSAX2InitDefaultSAXHandler(&saxHandler, 1);
    saxHandler.startDocument = xmlTextWriterStartDocumentCallback;
    saxHandler.startElement = xmlSAX2StartElement;
    saxHandler.endElement = xmlSAX2EndElement;

    ctxt = xmlCreatePushParserCtxt(&saxHandler, NULL, NULL, 0, NULL);
    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterDoc : error at xmlCreatePushParserCtxt!\n"));
        return NULL;
    }

    ctxt->myDoc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
    if (ctxt->myDoc == NULL) {
        xmlFreeParserCtxt(ctxt);
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterDoc : error at xmlNewDoc!\n"));
        return NULL;
    }

    ret = xmlNewTextWriterPushParser(ctxt, compression);
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterDoc : error at xmlNewTextWriterPushParser!\n"));
        return NULL;
    }

    xmlSetDocCompressMode(ctxt->myDoc, compression);

    if (doc != NULL)
        *doc = ctxt->myDoc;

    return ret;
}

/**
 * xmlNewTextWriterTree:
 * @param doc xmlDocPtr
 * @param node xmlNodePtr or NULL for doc->children
 * @param compression compress the output?
 *
 * Create a new xmlNewTextWriter structure with doc as output
 * starting at node
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
XMLPUBFUNEXPORT xmlTextWriterPtr
xmlNewTextWriterTree(xmlDocPtr doc, xmlNodePtr node, int compression)
{
    xmlTextWriterPtr ret;
    xmlSAXHandler saxHandler;
    xmlParserCtxtPtr ctxt;

    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterTree : invalid document tree!\n"));
        return NULL;
    }

    memset(&saxHandler, '\0', sizeof(saxHandler));
    xmlSAX2InitDefaultSAXHandler(&saxHandler, 1);
    saxHandler.startDocument = xmlTextWriterStartDocumentCallback;
    saxHandler.startElement = xmlSAX2StartElement;
    saxHandler.endElement = xmlSAX2EndElement;

    ctxt = xmlCreatePushParserCtxt(&saxHandler, NULL, NULL, 0, NULL);
    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterDoc : error at xmlCreatePushParserCtxt!\n"));
        return NULL;
    }

    ret = xmlNewTextWriterPushParser(ctxt, compression);
    if (ret == NULL) {
        xmlFreeParserCtxt(ctxt);
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlNewTextWriterDoc : error at xmlNewTextWriterPushParser!\n"));
        return NULL;
    }

    ctxt->myDoc = doc;
    ctxt->node = node;

    xmlSetDocCompressMode(doc, compression);

    return ret;
}
#endif /* LIBXML_SAX1_ENABLED */

/**
 * xmlFreeTextWriter:
 * @param writer the xmlTextWriterPtr
 *
 * Deallocate all the resources associated to the writer
 */
XMLPUBFUNEXPORT void
xmlFreeTextWriter(xmlTextWriterPtr writer)
{
    if (writer == NULL)
        return;

    if (writer->out != NULL)
        xmlOutputBufferClose(writer->out);

    if (writer->nodes != NULL)
        xmlListDelete(writer->nodes);

    if (writer->nsstack != NULL)
        xmlListDelete(writer->nsstack);

    if (writer->ctxt != NULL)
        xmlFreeParserCtxt(writer->ctxt);

    if (writer->ichar != NULL)
        xmlFree(writer->ichar);
    xmlFree(writer);
}

/**
 * xmlTextWriterStartDocument:
 * @param writer the xmlTextWriterPtr
 * @param version the xml version ("1.0") or NULL for default ("1.0")
 * @param encoding the encoding or NULL for default
 * @param standalone "yes" or "no" or NULL for default
 *
 * Start a new xml document
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartDocument(xmlTextWriterPtr writer, const char *version,
                           const char *encoding, const char *standalone)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlCharEncodingHandlerPtr encoder;

    if ((writer == NULL) || (writer->out == NULL)) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDocument : invalid writer!\n"));
        return -1;
    }

    lk = xmlListFront(writer->nodes);
    if ((lk != NULL) && (xmlLinkGetData(lk) != NULL)) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDocument : not allowed in this context!\n"));
        return -1;
    }

    encoder = NULL;
    if (encoding != NULL) {
        encoder = xmlFindCharEncodingHandler(encoding);
        if (encoder == NULL) {
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterStartDocument : out of memory!\n"));
            return -1;
        }
    }

    writer->out->encoder = encoder;
    if (encoder != NULL) {
        writer->out->conv = xmlBufferCreateSize(4000);
        xmlCharEncOutFunc(encoder, writer->out->conv, NULL);
    } else
        writer->out->conv = NULL;

    sum = 0;
    count = xmlOutputBufferWriteString(writer->out, "<?xml version=");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
    if (count < 0)
        return -1;
    sum += count;
    if (version != 0)
        count = xmlOutputBufferWriteString(writer->out, version);
    else
        count = xmlOutputBufferWriteString(writer->out, "1.0");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
    if (count < 0)
        return -1;
    sum += count;
    if (writer->out->encoder != 0) {
        count = xmlOutputBufferWriteString(writer->out, " encoding=");
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
        count =
            xmlOutputBufferWriteString(writer->out,
                                       writer->out->encoder->name);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (standalone != 0) {
        count = xmlOutputBufferWriteString(writer->out, " standalone=");
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWriteString(writer->out, standalone);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "?>\n");
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndDocument:
 * @param writer the xmlTextWriterPtr
 *
 * End an xml document. All open elements are closed
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndDocument(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterEndDocument : invalid writer!\n"));
        return -1;
    }

    sum = 0;
    while ((lk = xmlListFront(writer->nodes)) != NULL) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p == 0)
            break;
        switch (p->state) {
            case XML_TEXTWRITER_NAME:
            case XML_TEXTWRITER_ATTRIBUTE:
            case XML_TEXTWRITER_TEXT:
                count = xmlTextWriterEndElement(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_PI:
            case XML_TEXTWRITER_PI_TEXT:
                count = xmlTextWriterEndPI(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_CDATA:
                count = xmlTextWriterEndCDATA(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_DTD:
            case XML_TEXTWRITER_DTD_TEXT:
            case XML_TEXTWRITER_DTD_ELEM:
            case XML_TEXTWRITER_DTD_ELEM_TEXT:
            case XML_TEXTWRITER_DTD_ATTL:
            case XML_TEXTWRITER_DTD_ATTL_TEXT:
            case XML_TEXTWRITER_DTD_ENTY:
            case XML_TEXTWRITER_DTD_ENTY_TEXT:
            case XML_TEXTWRITER_DTD_PENT:
                count = xmlTextWriterEndDTD(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_COMMENT:
                count = xmlTextWriterEndComment(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            default:
                break;
        }
    }

    if (!writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }
    return sum;
}

/**
 * xmlTextWriterStartComment:
 * @param writer the xmlTextWriterPtr
 *
 * Start an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartComment(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartComment : invalid writer!\n"));
        return -1;
    }

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_TEXT:
                case XML_TEXTWRITER_NONE:
                    break;
                case XML_TEXTWRITER_NAME:
                    count = xmlOutputBufferWriteString(writer->out, ">");
                    if (count < 0)
                        return -1;
                    sum += count;
                    if (writer->indent) {
                        count =
                            xmlOutputBufferWriteString(writer->out, "\n");
                        if (count < 0)
                            return -1;
                        sum += count;
                    }
                    p->state = XML_TEXTWRITER_TEXT;
                    break;
                default:
                    return -1;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartElement : out of memory!\n"));
        return -1;
    }

    p->name = 0;
    p->state = XML_TEXTWRITER_COMMENT;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<!--");
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndComment:
 * @param writer the xmlTextWriterPtr
 *
 * End the current xml coment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndComment(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterEndComment : invalid writer!\n"));
        return -1;
    }

    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterEndComment : not allowed in this context!\n"));
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_COMMENT:
            count = xmlOutputBufferWriteString(writer->out, "-->");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatComment:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatComment(xmlTextWriterPtr writer,
                                const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatComment(writer, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatComment:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatComment(xmlTextWriterPtr writer,
                                 const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteVFormatComment : invalid writer!\n"));
        return -1;
    }

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteComment(writer, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteComment:
 * @param writer the xmlTextWriterPtr
 * @param content comment string
 *
 * Write an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteComment(xmlTextWriterPtr writer, const xmlChar* content)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartComment(writer);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterWriteString(writer, content);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterEndComment(writer);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartElement:
 * @param writer the xmlTextWriterPtr
 * @param name element name
 *
 * Start an xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartElement(xmlTextWriterPtr writer, const xmlChar * name)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_PI:
                case XML_TEXTWRITER_PI_TEXT:
                    return -1;
                case XML_TEXTWRITER_NONE:
                    break;
                case XML_TEXTWRITER_NAME:
                    count = xmlOutputBufferWriteString(writer->out, ">");
                    if (count < 0)
                        return -1;
                    sum += count;
                    if (writer->indent)
                        count =
                            xmlOutputBufferWriteString(writer->out, "\n");
                    p->state = XML_TEXTWRITER_TEXT;
                    break;
                default:
                    break;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartElement : out of memory!\n"));
        return -1;
    }

    p->name = xmlStrdup(name);
    if (p->name == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartElement : out of memory!\n"));
        xmlFree(p);
        return -1;
    }
    p->state = XML_TEXTWRITER_NAME;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<");
    if (count < 0)
        return -1;
    sum += count;
    count =
        xmlOutputBufferWriteString(writer->out, (const char *) p->name);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartElementNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix or NULL
 * @param name element local name
 * @param namespaceURI namespace URI or NULL
 *
 * Start an xml element with namespace support.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartElementNS(xmlTextWriterPtr writer,
                            const xmlChar * prefix, const xmlChar * name,
                            const xmlChar * namespaceURI)
{
    int count;
    int sum;
    xmlChar *buf;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    buf = 0;
    if (prefix != 0) {
        buf = xmlStrdup(prefix);
        buf = xmlStrcat(buf, BAD_CAST ":");
    }
    buf = xmlStrcat(buf, name);

    sum = 0;
    count = xmlTextWriterStartElement(writer, buf);
    xmlFree(buf);
    if (count < 0)
        return -1;
    sum += count;

    if (namespaceURI != 0) {
        buf = xmlStrdup(BAD_CAST "xmlns");
        if (prefix != 0) {
            buf = xmlStrcat(buf, BAD_CAST ":");
            buf = xmlStrcat(buf, prefix);
        }

        count = xmlTextWriterWriteAttribute(writer, buf, namespaceURI);
        xmlFree(buf);
        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterEndElement:
 * @param writer the xmlTextWriterPtr
 *
 * End the current xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndElement(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_ATTRIBUTE:
            count = xmlTextWriterEndAttribute(writer);
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_NAME:
            if (writer->indent) /* next element needs indent */
                writer->doindent = 1;
            count = xmlOutputBufferWriteString(writer->out, "/>");
            if (count < 0)
                return -1;
            sum += count;
            break;
        case XML_TEXTWRITER_TEXT:
            if ((writer->indent) && (writer->doindent)) {
                count = xmlTextWriterWriteIndent(writer);
                sum += count;
                writer->doindent = 1;
            } else
                writer->doindent = 1;
            count = xmlOutputBufferWriteString(writer->out, "</");
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out,
                                               (const char *) p->name);
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterFullEndElement:
 * @param writer the xmlTextWriterPtr
 *
 * End the current xml element. Writes an end tag even if the element is empty
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterFullEndElement(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_ATTRIBUTE:
            count = xmlTextWriterEndAttribute(writer);
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_NAME:
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_TEXT:
            count = xmlOutputBufferWriteString(writer->out, "</");
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out,
                                               (const char *) p->name);
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatRaw:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted raw xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatRaw(xmlTextWriterPtr writer, const char *format,
                            ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatRaw(writer, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatRaw:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted raw xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatRaw(xmlTextWriterPtr writer, const char *format,
                             va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteRaw(writer, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteRawLen:
 * @param writer the xmlTextWriterPtr
 * @param content text string
 * @param len length of the text string
 *
 * Write an xml text.
 * 
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteRawLen(xmlTextWriterPtr writer, const xmlChar * content,
                         int len)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteRawLen : invalid writer!\n"));
        return -1;
    }

    if ((content == NULL) && (len > 0)) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteRawLen : invalid content!\n"));
        return -1;
    }

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        count = xmlTextWriterHandleStateDependencies(writer, p);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (writer->indent)
        writer->doindent = 0;

    if (content != NULL) {
        count =
            xmlOutputBufferWrite(writer->out, len, (const char *) content);
        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterWriteRaw:
 * @param writer the xmlTextWriterPtr
 * @param content text string
 *
 * Write a raw xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteRaw(xmlTextWriterPtr writer, const xmlChar * content)
{
    return xmlTextWriterWriteRawLen(writer, content, xmlStrlen(content));
}

/**
 * xmlTextWriterWriteFormatString:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatString(xmlTextWriterPtr writer, const char *format,
                               ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatString(writer, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatString:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatString(xmlTextWriterPtr writer,
                                const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteString(writer, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteString:
 * @param writer the xmlTextWriterPtr
 * @param content text string
 *
 * Write an xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteString(xmlTextWriterPtr writer, const xmlChar * content)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    sum = 0;
    buf = (xmlChar *) content;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_NAME:
                case XML_TEXTWRITER_TEXT:
#if 0
                    buf = NULL;
                    xmlOutputBufferWriteEscape(writer->out, content, NULL);
#endif
                    buf = xmlEncodeSpecialChars(NULL, content);
                    break;
                case XML_TEXTWRITER_ATTRIBUTE:
                    buf = NULL;
                    xmlAttrSerializeTxtContent(writer->out->buffer, NULL,
                                               NULL, content);
                    break;
                default:
                    break;
            }
        }
    }

    if (buf != NULL) {
        count = xmlTextWriterWriteRaw(writer, buf);
        if (count < 0)
            return -1;
        sum += count;

        if (buf != content)     /* buf was allocated by us, so free it */
            xmlFree(buf);
    }

    return sum;
}

/**
 * xmlOutputBufferWriteBase64:
 * @param out the xmlOutputBufferPtr
 * @param data binary data
 * @param len the number of bytes to encode
 *
 * Write base64 encoded data to an xmlOutputBuffer.
 * Adapted from John Walker's base64.c (http://www.fourmilab.ch/).
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
static int
xmlOutputBufferWriteBase64(xmlOutputBufferPtr out, int len,
                           const unsigned char *data)
{
    
    unsigned char dtable[64] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i;
    int linelen;
    int count;
    int sum;

    linelen = 0;
    sum = 0;

    i = 0;
    while (1) {
        unsigned char igroup[3];
        unsigned char ogroup[4];
        int c;
        int n;

        igroup[0] = igroup[1] = igroup[2] = 0;
        for (n = 0; n < 3 && i < len; n++, i++) {
            c = data[i];
            igroup[n] = (unsigned char) c;
        }

        if (n > 0) {
            ogroup[0] = dtable[igroup[0] >> 2];
            ogroup[1] = dtable[((igroup[0] & 3) << 4) | (igroup[1] >> 4)];
            ogroup[2] =
                dtable[((igroup[1] & 0xF) << 2) | (igroup[2] >> 6)];
            ogroup[3] = dtable[igroup[2] & 0x3F];

            if (n < 3) {
                ogroup[3] = '=';
                if (n < 2) {
                    ogroup[2] = '=';
                }
            }

            if (linelen >= B64LINELEN) {
                count = xmlOutputBufferWrite(out, 2, B64CRLF);
                if (count == -1)
                    return -1;
                sum += count;
                linelen = 0;
            }
            count = xmlOutputBufferWrite(out, 4, (const char *) ogroup);
            if (count == -1)
                return -1;
            sum += count;

            linelen += 4;
        }

        if (i >= len)
            break;
    }

    return sum;
}

/**
 * xmlTextWriterWriteBase64:
 * @param writer the xmlTextWriterPtr
 * @param data binary data
 * @param start the position within the data of the first byte to encode
 * @param len the number of bytes to encode
 *
 * Write an base64 encoded xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteBase64(xmlTextWriterPtr writer, const char *data,
                         int start, int len)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            count = xmlTextWriterHandleStateDependencies(writer, p);
            if (count < 0)
                return -1;
            sum += count;
        }
    }

    if (writer->indent)
        writer->doindent = 0;

    count =
        xmlOutputBufferWriteBase64(writer->out, len,
                                   (unsigned char *) data + start);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlOutputBufferWriteBinHex:
 * @param out the xmlOutputBufferPtr
 * @param data binary data
 * @param len the number of bytes to encode
 *
 * Write hqx encoded data to an xmlOutputBuffer.
 * 
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
static int
xmlOutputBufferWriteBinHex(xmlOutputBufferPtr out,
                           int len, const unsigned char *data)
{
    int count;
    int sum;
    
    char hex[] = "0123456789ABCDEF";
    int i;

    if ((out == NULL) || ((data == 0) && (len != 0))) {
        return -1;
    }

    sum = 0;
    for (i = 0; i < len; i++) {
        count =
            xmlOutputBufferWrite(out, 1,
                                 (const char *) &hex[data[i] >> 4]);
        if (count == -1)
            return -1;
        sum += count;
        count =
            xmlOutputBufferWrite(out, 1,
                                 (const char *) &hex[data[i] & 0xF]);
        if (count == -1)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterWriteBinHex:
 * @param writer the xmlTextWriterPtr
 * @param data binary data
 * @param start the position within the data of the first byte to encode
 * @param len the number of bytes to encode
 *
 * Write a BinHex encoded xml text.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteBinHex(xmlTextWriterPtr writer, const char *data,
                         int start, int len)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            count = xmlTextWriterHandleStateDependencies(writer, p);
            if (count < 0)
                return -1;
            sum += count;
        }
    }

    if (writer->indent)
        writer->doindent = 0;

    count =
        xmlOutputBufferWriteBinHex(writer->out, len,
                                   (unsigned char *) data + start);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartAttribute:
 * @param writer the xmlTextWriterPtr
 * @param name element name
 *
 * Start an xml attribute.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartAttribute(xmlTextWriterPtr writer, const xmlChar * name)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    switch (p->state) {
        case XML_TEXTWRITER_ATTRIBUTE:
            count = xmlTextWriterEndAttribute(writer);
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_NAME:
            count = xmlOutputBufferWriteString(writer->out, " ");
            if (count < 0)
                return -1;
            sum += count;
            count =
                xmlOutputBufferWriteString(writer->out,
                                           (const char *) name);
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out, "=");
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
            if (count < 0)
                return -1;
            sum += count;
            p->state = XML_TEXTWRITER_ATTRIBUTE;
            break;
        default:
            return -1;
    }

    return sum;
}

/**
 * xmlTextWriterStartAttributeNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix or NULL
 * @param name element local name
 * @param namespaceURI namespace URI or NULL
 *
 * Start an xml attribute with namespace support.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartAttributeNS(xmlTextWriterPtr writer,
                              const xmlChar * prefix, const xmlChar * name,
                              const xmlChar * namespaceURI)
{
    int count;
    int sum;
    xmlChar *buf;
    xmlTextWriterNsStackEntry *p;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    buf = 0;
    if (prefix != 0) {
        buf = xmlStrdup(prefix);
        buf = xmlStrcat(buf, BAD_CAST ":");
    }
    buf = xmlStrcat(buf, name);

    sum = 0;
    count = xmlTextWriterStartAttribute(writer, buf);
    xmlFree(buf);
    if (count < 0)
        return -1;
    sum += count;

    if (namespaceURI != 0) {
        buf = xmlStrdup(BAD_CAST "xmlns");
        if (prefix != 0) {
            buf = xmlStrcat(buf, BAD_CAST ":");
            buf = xmlStrcat(buf, prefix);
        }

        p = (xmlTextWriterNsStackEntry *)
            xmlMalloc(sizeof(xmlTextWriterNsStackEntry));
        if (p == 0) {
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterStartAttributeNS : out of memory!\n"));
            return -1;
        }

        p->prefix = buf;
        p->uri = xmlStrdup(namespaceURI);
        if (p->uri == 0) {
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterStartAttributeNS : out of memory!\n"));
            xmlFree(p);
            return -1;
        }
        p->elem = xmlListFront(writer->nodes);

        xmlListPushFront(writer->nsstack, p);
    }

    return sum;
}

/**
 * xmlTextWriterEndAttribute:
 * @param writer the xmlTextWriterPtr
 *
 * End the current xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndAttribute(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;
    xmlTextWriterNsStackEntry *np;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        xmlListDelete(writer->nsstack);
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0) {
        xmlListDelete(writer->nsstack);
        return -1;
    }

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_ATTRIBUTE:
            p->state = XML_TEXTWRITER_NAME;

            count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
            if (count < 0) {
                xmlListDelete(writer->nsstack);
                return -1;
            }
            sum += count;

            while (!xmlListEmpty(writer->nsstack)) {
                lk = xmlListFront(writer->nsstack);
                np = (xmlTextWriterNsStackEntry *) xmlLinkGetData(lk);
                if (np != 0) {
                    count =
                        xmlTextWriterWriteAttribute(writer, np->prefix,
                                                    np->uri);
                    if (count < 0) {
                        xmlListDelete(writer->nsstack);
                        return -1;
                    }
                    sum += count;
                }

                xmlListPopFront(writer->nsstack);
            }
            break;

        default:
            xmlListClear(writer->nsstack);
            return -1;
    }

    return sum;
}

/**
 * xmlTextWriterWriteFormatAttribute:
 * @param writer the xmlTextWriterPtr
 * @param name attribute name
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted xml attribute.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatAttribute(xmlTextWriterPtr writer,
                                  const xmlChar * name, const char *format,
                                  ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatAttribute(writer, name, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatAttribute:
 * @param writer the xmlTextWriterPtr
 * @param name attribute name
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml attribute.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatAttribute(xmlTextWriterPtr writer,
                                   const xmlChar * name,
                                   const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteAttribute(writer, name, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteAttribute:
 * @param writer the xmlTextWriterPtr
 * @param name attribute name
 * @param content attribute content
 *
 * Write an xml attribute.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteAttribute(xmlTextWriterPtr writer, const xmlChar * name,
                            const xmlChar * content)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartAttribute(writer, name);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterWriteString(writer, content);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterEndAttribute(writer);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterWriteFormatAttributeNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix
 * @param name attribute local name
 * @param namespaceURI namespace URI
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted xml attribute.with namespace support
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatAttributeNS(xmlTextWriterPtr writer,
                                    const xmlChar * prefix,
                                    const xmlChar * name,
                                    const xmlChar * namespaceURI,
                                    const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatAttributeNS(writer, prefix, name,
                                              namespaceURI, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatAttributeNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix
 * @param name attribute local name
 * @param namespaceURI namespace URI
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml attribute.with namespace support
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatAttributeNS(xmlTextWriterPtr writer,
                                     const xmlChar * prefix,
                                     const xmlChar * name,
                                     const xmlChar * namespaceURI,
                                     const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteAttributeNS(writer, prefix, name, namespaceURI,
                                       buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteAttributeNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix
 * @param name attribute local name
 * @param namespaceURI namespace URI
 * @param content attribute content
 *
 * Write an xml attribute.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteAttributeNS(xmlTextWriterPtr writer,
                              const xmlChar * prefix, const xmlChar * name,
                              const xmlChar * namespaceURI,
                              const xmlChar * content)
{
    int count;
    int sum;
    xmlChar *buf;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    buf = 0;
    if (prefix != NULL) {
        buf = xmlStrdup(prefix);
        buf = xmlStrcat(buf, BAD_CAST ":");
    }
    buf = xmlStrcat(buf, name);

    sum = 0;
    count = xmlTextWriterWriteAttribute(writer, buf, content);
    xmlFree(buf);
    if (count < 0)
        return -1;
    sum += count;

    if (namespaceURI != NULL) {
        buf = 0;
        buf = xmlStrdup(BAD_CAST "xmlns");
        if (prefix != NULL) {
            buf = xmlStrcat(buf, BAD_CAST ":");
            buf = xmlStrcat(buf, prefix);
        }
        count = xmlTextWriterWriteAttribute(writer, buf, namespaceURI);
        xmlFree(buf);
        if (count < 0)
            return -1;
        sum += count;
    }
    return sum;
}

/**
 * xmlTextWriterWriteFormatElement:
 * @param writer the xmlTextWriterPtr
 * @param name element name
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatElement(xmlTextWriterPtr writer,
                                const xmlChar * name, const char *format,
                                ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatElement(writer, name, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatElement:
 * @param writer the xmlTextWriterPtr
 * @param name element name
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatElement(xmlTextWriterPtr writer,
                                 const xmlChar * name, const char *format,
                                 va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteElement(writer, name, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteElement:
 * @param writer the xmlTextWriterPtr
 * @param name element name
 * @param content element content
 *
 * Write an xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteElement(xmlTextWriterPtr writer, const xmlChar * name,
                          const xmlChar * content)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartElement(writer, name);
    if (count == -1)
        return -1;
    sum += count;
    count = xmlTextWriterWriteString(writer, content);
    if (count == -1)
        return -1;
    sum += count;
    count = xmlTextWriterEndElement(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterWriteFormatElementNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix
 * @param name element local name
 * @param namespaceURI namespace URI
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted xml element with namespace support.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatElementNS(xmlTextWriterPtr writer,
                                  const xmlChar * prefix,
                                  const xmlChar * name,
                                  const xmlChar * namespaceURI,
                                  const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatElementNS(writer, prefix, name,
                                            namespaceURI, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatElementNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix
 * @param name element local name
 * @param namespaceURI namespace URI
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml element with namespace support.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatElementNS(xmlTextWriterPtr writer,
                                   const xmlChar * prefix,
                                   const xmlChar * name,
                                   const xmlChar * namespaceURI,
                                   const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteElementNS(writer, prefix, name, namespaceURI,
                                     buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteElementNS:
 * @param writer the xmlTextWriterPtr
 * @param prefix namespace prefix
 * @param name element local name
 * @param namespaceURI namespace URI
 * @param content element content
 *
 * Write an xml element with namespace support.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteElementNS(xmlTextWriterPtr writer,
                            const xmlChar * prefix, const xmlChar * name,
                            const xmlChar * namespaceURI,
                            const xmlChar * content)
{
    int count;
    int sum;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    sum = 0;
    count =
        xmlTextWriterStartElementNS(writer, prefix, name, namespaceURI);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterWriteString(writer, content);
    if (count == -1)
        return -1;
    sum += count;
    count = xmlTextWriterEndElement(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartPI:
 * @param writer the xmlTextWriterPtr
 * @param target PI target
 *
 * Start an xml PI.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartPI(xmlTextWriterPtr writer, const xmlChar * target)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if ((writer == NULL) || (target == NULL) || (*target == '\0'))
        return -1;

    if (xmlStrcasecmp(target, (const xmlChar *) "xml") == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartPI : target name [Xx][Mm][Ll] is reserved for xml standardization!\n"));
        return -1;
    }

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_ATTRIBUTE:
                    count = xmlTextWriterEndAttribute(writer);
                    if (count < 0)
                        return -1;
                    sum += count;
                    /* fallthrough */
                case XML_TEXTWRITER_NAME:
                    count = xmlOutputBufferWriteString(writer->out, ">");
                    if (count < 0)
                        return -1;
                    sum += count;
                    p->state = XML_TEXTWRITER_TEXT;
                    break;
                case XML_TEXTWRITER_NONE:
                case XML_TEXTWRITER_TEXT:
                case XML_TEXTWRITER_DTD:
                    break;
                case XML_TEXTWRITER_PI:
                case XML_TEXTWRITER_PI_TEXT:
                    xmlGenericError(xmlGenericErrorContext,
                                    EMBED_ERRTXT("xmlTextWriterStartPI : nested PI!\n"));
                    return -1;
                default:
                    return -1;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartPI : out of memory!\n"));
        return -1;
    }

    p->name = xmlStrdup(target);
    if (p->name == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartPI : out of memory!\n"));
        xmlFree(p);
        return -1;
    }
    p->state = XML_TEXTWRITER_PI;

    xmlListPushFront(writer->nodes, p);

    count = xmlOutputBufferWriteString(writer->out, "<?");
    if (count < 0)
        return -1;
    sum += count;
    count =
        xmlOutputBufferWriteString(writer->out, (const char *) p->name);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndPI:
 * @param writer the xmlTextWriterPtr
 *
 * End the current xml PI.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndPI(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return 0;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return 0;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_PI:
        case XML_TEXTWRITER_PI_TEXT:
            count = xmlOutputBufferWriteString(writer->out, "?>");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatPI:
 * @param writer the xmlTextWriterPtr
 * @param target PI target
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted PI.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatPI(xmlTextWriterPtr writer, const xmlChar * target,
                           const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatPI(writer, target, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatPI:
 * @param writer the xmlTextWriterPtr
 * @param target PI target
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml PI.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatPI(xmlTextWriterPtr writer,
                            const xmlChar * target, const char *format,
                            va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWritePI(writer, target, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWritePI:
 * @param writer the xmlTextWriterPtr
 * @param target PI target
 * @param content PI content
 *
 * Write an xml PI.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWritePI(xmlTextWriterPtr writer, const xmlChar * target,
                     const xmlChar * content)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartPI(writer, target);
    if (count == -1)
        return -1;
    sum += count;
    if (content != 0) {
        count = xmlTextWriterWriteString(writer, content);
        if (count == -1)
            return -1;
        sum += count;
    }
    count = xmlTextWriterEndPI(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartCDATA:
 * @param writer the xmlTextWriterPtr
 *
 * Start an xml CDATA section.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartCDATA(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_NONE:
                case XML_TEXTWRITER_PI:
                case XML_TEXTWRITER_PI_TEXT:
                    break;
                case XML_TEXTWRITER_ATTRIBUTE:
                    count = xmlTextWriterEndAttribute(writer);
                    if (count < 0)
                        return -1;
                    sum += count;
                    /* fallthrough */
                case XML_TEXTWRITER_NAME:
                    count = xmlOutputBufferWriteString(writer->out, ">");
                    if (count < 0)
                        return -1;
                    sum += count;
                    p->state = XML_TEXTWRITER_TEXT;
                    break;
                case XML_TEXTWRITER_CDATA:
                    xmlGenericError(xmlGenericErrorContext,
                                    EMBED_ERRTXT("xmlTextWriterStartCDATA : CDATA not allowed in this context!\n"));
                    return -1;
                default:
                    return -1;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartCDATA : out of memory!\n"));
        return -1;
    }

    p->name = 0;
    p->state = XML_TEXTWRITER_CDATA;

    xmlListPushFront(writer->nodes, p);

    count = xmlOutputBufferWriteString(writer->out, "<![CDATA[");
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndCDATA:
 * @param writer the xmlTextWriterPtr
 *
 * End an xml CDATA section.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndCDATA(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_CDATA:
            count = xmlOutputBufferWriteString(writer->out, "]]>");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatCDATA:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted xml CDATA.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatCDATA(xmlTextWriterPtr writer, const char *format,
                              ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatCDATA(writer, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatCDATA:
 * @param writer the xmlTextWriterPtr
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted xml CDATA.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatCDATA(xmlTextWriterPtr writer, const char *format,
                               va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteCDATA(writer, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteCDATA:
 * @param writer the xmlTextWriterPtr
 * @param content CDATA content
 *
 * Write an xml CDATA.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteCDATA(xmlTextWriterPtr writer, const xmlChar * content)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartCDATA(writer);
    if (count == -1)
        return -1;
    sum += count;
    if (content != 0) {
        count = xmlTextWriterWriteString(writer, content);
        if (count == -1)
            return -1;
        sum += count;
    }
    count = xmlTextWriterEndCDATA(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartDTD:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 *
 * Start an xml DTD.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartDTD(xmlTextWriterPtr writer,
                      const xmlChar * name,
                      const xmlChar * pubid, const xmlChar * sysid)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL || name == NULL || *name == '\0')
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if ((lk != NULL) && (xmlLinkGetData(lk) != NULL)) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTD : DTD allowed only in prolog!\n"));
        return -1;
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTD : out of memory!\n"));
        return -1;
    }

    p->name = xmlStrdup(name);
    if (p->name == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTD : out of memory!\n"));
        xmlFree(p);
        return -1;
    }
    p->state = XML_TEXTWRITER_DTD;

    xmlListPushFront(writer->nodes, p);

    count = xmlOutputBufferWriteString(writer->out, "<!DOCTYPE ");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWriteString(writer->out, (const char *) name);
    if (count < 0)
        return -1;
    sum += count;

    if (pubid != 0) {
        if (sysid == 0) {
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterStartDTD : system identifier needed!\n"));
            return -1;
        }

        if (writer->indent)
            count = xmlOutputBufferWrite(writer->out, 1, "\n");
        else
            count = xmlOutputBufferWrite(writer->out, 1, " ");
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWriteString(writer->out, "PUBLIC ");
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;

        count =
            xmlOutputBufferWriteString(writer->out, (const char *) pubid);
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (sysid != 0) {
        if (pubid == 0) {
            if (writer->indent)
                count = xmlOutputBufferWrite(writer->out, 1, "\n");
            else
                count = xmlOutputBufferWrite(writer->out, 1, " ");
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out, "SYSTEM ");
            if (count < 0)
                return -1;
            sum += count;
        } else if (writer->indent) {
            count = xmlOutputBufferWriteString(writer->out, "\n       ");
            if (count < 0)
                return -1;
            sum += count;
        }

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;

        count =
            xmlOutputBufferWriteString(writer->out, (const char *) sysid);
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterEndDTD:
 * @param writer the xmlTextWriterPtr
 *
 * End an xml DTD.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndDTD(xmlTextWriterPtr writer)
{
    int loop;
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    loop = 1;
    while (loop) {
        lk = xmlListFront(writer->nodes);
        if (lk == NULL)
            break;
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p == 0)
            break;
        switch (p->state) {
            case XML_TEXTWRITER_DTD_TEXT:
                count = xmlOutputBufferWriteString(writer->out, "]");
                if (count < 0)
                    return -1;
                sum += count;
                /* fallthrough */
            case XML_TEXTWRITER_DTD:
                count = xmlOutputBufferWriteString(writer->out, ">");

                if (writer->indent) {
                    if (count < 0)
                        return -1;
                    sum += count;
                    count = xmlOutputBufferWriteString(writer->out, "\n");
                }

                xmlListPopFront(writer->nodes);
                break;
            case XML_TEXTWRITER_DTD_ELEM:
            case XML_TEXTWRITER_DTD_ELEM_TEXT:
                count = xmlTextWriterEndDTDElement(writer);
                break;
            case XML_TEXTWRITER_DTD_ATTL:
            case XML_TEXTWRITER_DTD_ATTL_TEXT:
                count = xmlTextWriterEndDTDAttlist(writer);
                break;
            case XML_TEXTWRITER_DTD_ENTY:
            case XML_TEXTWRITER_DTD_PENT:
            case XML_TEXTWRITER_DTD_ENTY_TEXT:
                count = xmlTextWriterEndDTDEntity(writer);
                break;
            case XML_TEXTWRITER_COMMENT:
                count = xmlTextWriterEndComment(writer);
                break;
            default:
                loop = 0;
                continue;
        }

        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterWriteFormatDTD:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a DTD with a formatted markup declarations part.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatDTD(xmlTextWriterPtr writer,
                            const xmlChar * name,
                            const xmlChar * pubid,
                            const xmlChar * sysid, const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatDTD(writer, name, pubid, sysid, format,
                                      ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatDTD:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a DTD with a formatted markup declarations part.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatDTD(xmlTextWriterPtr writer,
                             const xmlChar * name,
                             const xmlChar * pubid,
                             const xmlChar * sysid,
                             const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteDTD(writer, name, pubid, sysid, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteDTD:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 * @param subset string content of the DTD
 *
 * Write a DTD.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTD(xmlTextWriterPtr writer,
                      const xmlChar * name,
                      const xmlChar * pubid,
                      const xmlChar * sysid, const xmlChar * subset)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartDTD(writer, name, pubid, sysid);
    if (count == -1)
        return -1;
    sum += count;
    if (subset != 0) {
        count = xmlTextWriterWriteString(writer, subset);
        if (count == -1)
            return -1;
        sum += count;
    }
    count = xmlTextWriterEndDTD(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartDTDElement:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD element
 *
 * Start an xml DTD element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartDTDElement(xmlTextWriterPtr writer, const xmlChar * name)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL || name == NULL || *name == '\0')
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p != 0) {
        switch (p->state) {
            case XML_TEXTWRITER_DTD:
                count = xmlOutputBufferWriteString(writer->out, " [");
                if (count < 0)
                    return -1;
                sum += count;
                if (writer->indent) {
                    count = xmlOutputBufferWriteString(writer->out, "\n");
                    if (count < 0)
                        return -1;
                    sum += count;
                }
                p->state = XML_TEXTWRITER_DTD_TEXT;
                /* fallthrough */
            case XML_TEXTWRITER_DTD_TEXT:
            case XML_TEXTWRITER_NONE:
                break;
            default:
                return -1;
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTDElement : out of memory!\n"));
        return -1;
    }

    p->name = xmlStrdup(name);
    if (p->name == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTDElement : out of memory!\n"));
        xmlFree(p);
        return -1;
    }
    p->state = XML_TEXTWRITER_DTD_ELEM;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<!ELEMENT ");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWriteString(writer->out, (const char *) name);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndDTDElement:
 * @param writer the xmlTextWriterPtr
 *
 * End an xml DTD element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndDTDElement(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    switch (p->state) {
        case XML_TEXTWRITER_DTD_ELEM:
        case XML_TEXTWRITER_DTD_ELEM_TEXT:
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatDTDElement:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD element
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted DTD element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatDTDElement(xmlTextWriterPtr writer,
                                   const xmlChar * name,
                                   const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatDTDElement(writer, name, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatDTDElement:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD element
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted DTD element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatDTDElement(xmlTextWriterPtr writer,
                                    const xmlChar * name,
                                    const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteDTDElement(writer, name, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteDTDElement:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD element
 * @param content content of the element
 *
 * Write a DTD element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDElement(xmlTextWriterPtr writer,
                             const xmlChar * name, const xmlChar * content)
{
    int count;
    int sum;

    if (content == NULL)
        return -1;

    sum = 0;
    count = xmlTextWriterStartDTDElement(writer, name);
    if (count == -1)
        return -1;
    sum += count;

    count = xmlTextWriterWriteString(writer, content);
    if (count == -1)
        return -1;
    sum += count;

    count = xmlTextWriterEndDTDElement(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartDTDAttlist:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD ATTLIST
 *
 * Start an xml DTD ATTLIST.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartDTDAttlist(xmlTextWriterPtr writer, const xmlChar * name)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL || name == NULL || *name == '\0')
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p != 0) {
        switch (p->state) {
            case XML_TEXTWRITER_DTD:
                count = xmlOutputBufferWriteString(writer->out, " [");
                if (count < 0)
                    return -1;
                sum += count;
                if (writer->indent) {
                    count = xmlOutputBufferWriteString(writer->out, "\n");
                    if (count < 0)
                        return -1;
                    sum += count;
                }
                p->state = XML_TEXTWRITER_DTD_TEXT;
                /* fallthrough */
            case XML_TEXTWRITER_DTD_TEXT:
            case XML_TEXTWRITER_NONE:
                break;
            default:
                return -1;
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTDAttlist : out of memory!\n"));
        return -1;
    }

    p->name = xmlStrdup(name);
    if (p->name == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTDAttlist : out of memory!\n"));
        xmlFree(p);
        return -1;
    }
    p->state = XML_TEXTWRITER_DTD_ATTL;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<!ATTLIST ");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWriteString(writer->out, (const char *) name);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndDTDAttlist:
 * @param writer the xmlTextWriterPtr
 *
 * End an xml DTD attribute list.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndDTDAttlist(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    switch (p->state) {
        case XML_TEXTWRITER_DTD_ATTL:
        case XML_TEXTWRITER_DTD_ATTL_TEXT:
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatDTDAttlist:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD ATTLIST
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted DTD ATTLIST.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatDTDAttlist(xmlTextWriterPtr writer,
                                   const xmlChar * name,
                                   const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatDTDAttlist(writer, name, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatDTDAttlist:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD ATTLIST
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted DTD ATTLIST.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatDTDAttlist(xmlTextWriterPtr writer,
                                    const xmlChar * name,
                                    const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteDTDAttlist(writer, name, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteDTDAttlist:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the DTD ATTLIST
 * @param content content of the ATTLIST
 *
 * Write a DTD ATTLIST.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDAttlist(xmlTextWriterPtr writer,
                             const xmlChar * name, const xmlChar * content)
{
    int count;
    int sum;

    if (content == NULL)
        return -1;

    sum = 0;
    count = xmlTextWriterStartDTDAttlist(writer, name);
    if (count == -1)
        return -1;
    sum += count;

    count = xmlTextWriterWriteString(writer, content);
    if (count == -1)
        return -1;
    sum += count;

    count = xmlTextWriterEndDTDAttlist(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartDTDEntity:
 * @param writer the xmlTextWriterPtr
 * @param pe TRUE if this is a parameter entity, FALSE if not
 * @param name the name of the DTD ATTLIST
 *
 * Start an xml DTD ATTLIST.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterStartDTDEntity(xmlTextWriterPtr writer,
                            int pe, const xmlChar * name)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL || name == NULL || *name == '\0')
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {

        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_DTD:
                    count = xmlOutputBufferWriteString(writer->out, " [");
                    if (count < 0)
                        return -1;
                    sum += count;
                    if (writer->indent) {
                        count =
                            xmlOutputBufferWriteString(writer->out, "\n");
                        if (count < 0)
                            return -1;
                        sum += count;
                    }
                    p->state = XML_TEXTWRITER_DTD_TEXT;
                    /* fallthrough */
                case XML_TEXTWRITER_DTD_TEXT:
                case XML_TEXTWRITER_NONE:
                    break;
                default:
                    return -1;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTDElement : out of memory!\n"));
        return -1;
    }

    p->name = xmlStrdup(name);
    if (p->name == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterStartDTDElement : out of memory!\n"));
        xmlFree(p);
        return -1;
    }

    if (pe != 0)
        p->state = XML_TEXTWRITER_DTD_PENT;
    else
        p->state = XML_TEXTWRITER_DTD_ENTY;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<!ENTITY ");
    if (count < 0)
        return -1;
    sum += count;

    if (pe != 0) {
        count = xmlOutputBufferWriteString(writer->out, "% ");
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, (const char *) name);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndDTDEntity:
 * @param writer the xmlTextWriterPtr
 *
 * End an xml DTD entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterEndDTDEntity(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    switch (p->state) {
        case XML_TEXTWRITER_DTD_ENTY_TEXT:
            count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
            if (count < 0)
                return -1;
            sum += count;
        case XML_TEXTWRITER_DTD_ENTY:
        case XML_TEXTWRITER_DTD_PENT:
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatDTDInternalEntity:
 * @param writer the xmlTextWriterPtr
 * @param pe TRUE if this is a parameter entity, FALSE if not
 * @param name the name of the DTD entity
 * @param format format string (see printf)
 * @param # extra parameters for the format
 *
 * Write a formatted DTD internal entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteFormatDTDInternalEntity(xmlTextWriterPtr writer,
                                          int pe,
                                          const xmlChar * name,
                                          const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatDTDInternalEntity(writer, pe, name,
                                                    format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatDTDInternalEntity:
 * @param writer the xmlTextWriterPtr
 * @param pe TRUE if this is a parameter entity, FALSE if not
 * @param name the name of the DTD entity
 * @param format format string (see printf)
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Write a formatted DTD internal entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteVFormatDTDInternalEntity(xmlTextWriterPtr writer,
                                           int pe,
                                           const xmlChar * name,
                                           const char *format,
                                           va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL)
        return -1;

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteDTDInternalEntity(writer, pe, name, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteDTDEntity:
 * @param writer the xmlTextWriterPtr
 * @param pe TRUE if this is a parameter entity, FALSE if not
 * @param name the name of the DTD entity
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 * @param ndataid the xml notation name.
 * @param content content of the entity
 *
 * Write a DTD entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDEntity(xmlTextWriterPtr writer,
                            int pe,
                            const xmlChar * name,
                            const xmlChar * pubid,
                            const xmlChar * sysid,
                            const xmlChar * ndataid,
                            const xmlChar * content)
{
    if ((content == NULL) && (pubid == NULL) && (sysid == NULL))
        return -1;
    if ((pe != 0) && (ndataid != NULL))
        return -1;

    if ((pubid == NULL) && (sysid == NULL))
        return xmlTextWriterWriteDTDInternalEntity(writer, pe, name,
                                                   content);

    return xmlTextWriterWriteDTDExternalEntity(writer, pe, name, pubid,
                                               sysid, ndataid);
}

/**
 * xmlTextWriterWriteDTDInternalEntity:
 * @param writer the xmlTextWriterPtr
 * @param pe TRUE if this is a parameter entity, FALSE if not
 * @param name the name of the DTD entity
 * @param content content of the entity
 *
 * Write a DTD internal entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDInternalEntity(xmlTextWriterPtr writer,
                                    int pe,
                                    const xmlChar * name,
                                    const xmlChar * content)
{
    int count;
    int sum;

    if ((name == NULL) || (*name == '\0') || (content == NULL))
        return -1;

    sum = 0;
    count = xmlTextWriterStartDTDEntity(writer, pe, name);
    if (count == -1)
        return -1;
    sum += count;

    count = xmlTextWriterWriteString(writer, content);
    if (count == -1)
        return -1;
    sum += count;

    count = xmlTextWriterEndDTDEntity(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterWriteDTDExternalEntity:
 * @param writer the xmlTextWriterPtr
 * @param pe TRUE if this is a parameter entity, FALSE if not
 * @param name the name of the DTD entity
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 * @param ndataid the xml notation name.
 *
 * Write a DTD external entity. The entity must have been started with xmlTextWriterStartDTDEntity
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDExternalEntity(xmlTextWriterPtr writer,
                                    int pe,
                                    const xmlChar * name,
                                    const xmlChar * pubid,
                                    const xmlChar * sysid,
                                    const xmlChar * ndataid)
{
    int count;
    int sum;

    if (((pubid == NULL) && (sysid == NULL)))
        return -1;
    if ((pe != 0) && (ndataid != NULL))
        return -1;

    sum = 0;
    count = xmlTextWriterStartDTDEntity(writer, pe, name);
    if (count == -1)
        return -1;
    sum += count;

    count =
        xmlTextWriterWriteDTDExternalEntityContents(writer, pubid, sysid,
                                                    ndataid);
    if (count < 0)
        return -1;
    sum += count;

    count = xmlTextWriterEndDTDEntity(writer);
    if (count == -1)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterWriteDTDExternalEntityContents:
 * @param writer the xmlTextWriterPtr
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 * @param ndataid the xml notation name.
 *
 * Write the contents of a DTD external entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDExternalEntityContents(xmlTextWriterPtr writer,
                                            const xmlChar * pubid,
                                            const xmlChar * sysid,
                                            const xmlChar * ndataid)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteDTDExternalEntityContents: xmlTextWriterPtr invalid!\n"));
        return -1;
    }

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteDTDExternalEntityContents: you must call xmlTextWriterStartDTDEntity before the call to this function!\n"));
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    switch (p->state) {
        case XML_TEXTWRITER_DTD_ENTY:
            break;
        case XML_TEXTWRITER_DTD_PENT:
            if (ndataid != NULL) {
                xmlGenericError(xmlGenericErrorContext,
                                EMBED_ERRTXT("xmlTextWriterWriteDTDExternalEntityContents: notation not allowed with parameter entities!\n"));
                return -1;
            }
            break;
        default:
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterWriteDTDExternalEntityContents: you must call xmlTextWriterStartDTDEntity before the call to this function!\n"));
            return -1;
    }

    if (pubid != 0) {
        if (sysid == 0) {
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterWriteDTDExternalEntityContents: system identifier needed!\n"));
            return -1;
        }

        count = xmlOutputBufferWriteString(writer->out, " PUBLIC ");
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;

        count =
            xmlOutputBufferWriteString(writer->out, (const char *) pubid);
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (sysid != 0) {
        if (pubid == 0) {
            count = xmlOutputBufferWriteString(writer->out, " SYSTEM");
            if (count < 0)
                return -1;
            sum += count;
        }

        count = xmlOutputBufferWriteString(writer->out, " ");
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;

        count =
            xmlOutputBufferWriteString(writer->out, (const char *) sysid);
        if (count < 0)
            return -1;
        sum += count;

        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (ndataid != NULL) {
        count = xmlOutputBufferWriteString(writer->out, " NDATA ");
        if (count < 0)
            return -1;
        sum += count;

        count =
            xmlOutputBufferWriteString(writer->out,
                                       (const char *) ndataid);
        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterWriteDTDNotation:
 * @param writer the xmlTextWriterPtr
 * @param name the name of the xml notation
 * @param pubid the public identifier, which is an alternative to the system identifier
 * @param sysid the system identifier, which is the URI of the DTD
 *
 * Write a DTD entity.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterWriteDTDNotation(xmlTextWriterPtr writer,
                              const xmlChar * name,
                              const xmlChar * pubid, const xmlChar * sysid)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL || name == NULL || *name == '\0')
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p != 0) {
        switch (p->state) {
            case XML_TEXTWRITER_DTD:
                count = xmlOutputBufferWriteString(writer->out, " [");
                if (count < 0)
                    return -1;
                sum += count;
                if (writer->indent) {
                    count = xmlOutputBufferWriteString(writer->out, "\n");
                    if (count < 0)
                        return -1;
                    sum += count;
                }
                p->state = XML_TEXTWRITER_DTD_TEXT;
                /* fallthrough */
            case XML_TEXTWRITER_DTD_TEXT:
                break;
            default:
                return -1;
        }
    }

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<!NOTATION ");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWriteString(writer->out, (const char *) name);
    if (count < 0)
        return -1;
    sum += count;

    if (pubid != 0) {
        count = xmlOutputBufferWriteString(writer->out, " PUBLIC ");
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
        count =
            xmlOutputBufferWriteString(writer->out, (const char *) pubid);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (sysid != 0) {
        if (pubid == 0) {
            count = xmlOutputBufferWriteString(writer->out, " SYSTEM");
            if (count < 0)
                return -1;
            sum += count;
        }
        count = xmlOutputBufferWriteString(writer->out, " ");
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
        count =
            xmlOutputBufferWriteString(writer->out, (const char *) sysid);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, ">");
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterFlush:
 * @param writer the xmlTextWriterPtr
 *
 * Flush the output buffer.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlTextWriterFlush(xmlTextWriterPtr writer)
{
    int count;

    if (writer == NULL)
        return -1;

    if (writer->out == NULL)
        count = 0;
    else
        count = xmlOutputBufferFlush(writer->out);

    return count;
}

/**
 * misc
 */

/**
 * xmlFreeTextWriterStackEntry:
 * @param lk the xmlLinkPtr
 *
 * Free callback for the xmlList.
 */
static void
xmlFreeTextWriterStackEntry(xmlLinkPtr lk)
{
    xmlTextWriterStackEntry *p;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return;

    if (p->name != 0)
        xmlFree(p->name);
    xmlFree(p);
}

/**
 * xmlCmpTextWriterStackEntry:
 * @param data0 the first data
 * @param data1 the second data
 *
 * Compare callback for the xmlList.
 *
 * Returns -1, 0, 1
 */
static int
xmlCmpTextWriterStackEntry(const void *data0, const void *data1)
{
    xmlTextWriterStackEntry *p0;
    xmlTextWriterStackEntry *p1;

    if (data0 == data1)
        return 0;

    if (data0 == 0)
        return -1;

    if (data1 == 0)
        return 1;

    p0 = (xmlTextWriterStackEntry *) data0;
    p1 = (xmlTextWriterStackEntry *) data1;

    return xmlStrcmp(p0->name, p1->name);
}

/**
 * misc
 */

/**
 * xmlFreeTextWriterNsStackEntry:
 * @param lk the xmlLinkPtr
 *
 * Free callback for the xmlList.
 */
static void
xmlFreeTextWriterNsStackEntry(xmlLinkPtr lk)
{
    xmlTextWriterNsStackEntry *p;

    p = (xmlTextWriterNsStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return;

    if (p->prefix != 0)
        xmlFree(p->prefix);
    if (p->uri != 0)
        xmlFree(p->uri);

    xmlFree(p);
}

/**
 * xmlCmpTextWriterNsStackEntry:
 * @param data0 the first data
 * @param data1 the second data
 *
 * Compare callback for the xmlList.
 *
 * Returns -1, 0, 1
 */
static int
xmlCmpTextWriterNsStackEntry(const void *data0, const void *data1)
{
    xmlTextWriterNsStackEntry *p0;
    xmlTextWriterNsStackEntry *p1;
    int rc;

    if (data0 == data1)
        return 0;

    if (data0 == 0)
        return -1;

    if (data1 == 0)
        return 1;

    p0 = (xmlTextWriterNsStackEntry *) data0;
    p1 = (xmlTextWriterNsStackEntry *) data1;

    rc = xmlStrcmp(p0->prefix, p1->prefix);

    if (rc == 0)
        rc = p0->elem == p1->elem;

    return rc;
}

/**
 * xmlTextWriterWriteMemCallback:
 * @param context the xmlBufferPtr
 * @param str the data to write
 * @param len the length of the data
 *
 * Write callback for the xmlOutputBuffer with target xmlBuffer
 *
 * Returns -1, 0, 1
 */
static int
xmlTextWriterWriteMemCallback(void *context, const xmlChar * str, int len)
{
    xmlBufferPtr buf = (xmlBufferPtr) context;

    xmlBufferAdd(buf, str, len);

    return len;
}

/**
 * xmlTextWriterCloseMemCallback:
 * @param context the xmlBufferPtr
 *
 * Close callback for the xmlOutputBuffer with target xmlBuffer
 *
 * Returns -1, 0, 1
 */
static int
xmlTextWriterCloseMemCallback(void *context ATTRIBUTE_UNUSED)
{
    return 0;
}

/**
 * xmlTextWriterWriteDocCallback:
 * @param context the xmlBufferPtr
 * @param str the data to write
 * @param len the length of the data
 *
 * Write callback for the xmlOutputBuffer with target xmlBuffer
 *
 * Returns -1, 0, 1
 */
static int
xmlTextWriterWriteDocCallback(void *context, const xmlChar * str, int len)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) context;
    int rc;

    if ((rc = xmlParseChunk(ctxt, (const char *) str, len, 0)) != 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteDocCallback : XML error %d !\n"),
                        rc);
        return -1;
    }

    return len;
}

/**
 * xmlTextWriterCloseDocCallback:
 * @param context the xmlBufferPtr
 *
 * Close callback for the xmlOutputBuffer with target xmlBuffer
 *
 * Returns -1, 0, 1
 */
static int
xmlTextWriterCloseDocCallback(void *context)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) context;
    int rc;

    if ((rc = xmlParseChunk(ctxt, NULL, 0, 1)) != 0) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterWriteDocCallback : XML error %d !\n"),
                        rc);
        return -1;
    }

    return 0;
}

/**
 * xmlTextWriterVSprintf:
 * @param format see printf
 * @param argptr pointer to the first member of the variable argument list.
 *
 * Utility function for formatted output
 *
 * Returns a new xmlChar buffer with the data or NULL on error. This buffer must be freed.
 */
static xmlChar *
xmlTextWriterVSprintf(const char *format, va_list argptr)
{
    int size;
    int count;
    xmlChar *buf;

    size = BUFSIZ;
    buf = (xmlChar *) xmlMalloc(size);
    if (buf == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("xmlTextWriterVSprintf : out of memory!\n"));
        return NULL;
    }

    while (((count = vsnprintf((char *) buf, size, format, argptr)) < 0)
           || (count == size - 1) || (count == size) || (count > size)) {
        xmlFree(buf);
        size += BUFSIZ;
        buf = (xmlChar *) xmlMalloc(size);
        if (buf == NULL) {
            xmlGenericError(xmlGenericErrorContext,
                            EMBED_ERRTXT("xmlTextWriterVSprintf : out of memory!\n"));
            return NULL;
        }
    }

    return buf;
}

/**
 * xmlTextWriterStartDocumentCallback:
 * @param ctx the user data (XML parser context)
 *
 * called at the start of document processing.
 */
static void
xmlTextWriterStartDocumentCallback(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlDocPtr doc;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext, "SAX.startDocument()\n");
#endif
    if (ctxt->html) {
#ifdef LIBXML_HTML_ENABLED
        if (ctxt->myDoc == NULL)
            ctxt->myDoc = htmlNewDocNoDtD(NULL, NULL);
        if (ctxt->myDoc == NULL) {
            if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                ctxt->sax->error(ctxt->userData,
                                 EMBED_ERRTXT("SAX.startDocument(): out of memory\n"));
            ctxt->errNo = XML_ERR_NO_MEMORY;
            ctxt->instate = XML_PARSER_EOF;
            ctxt->disableSAX = 1;
            return;
        }
#else
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("libxml2 built without HTML support\n"));
        ctxt->errNo = XML_ERR_INTERNAL_ERROR;
        ctxt->instate = XML_PARSER_EOF;
        ctxt->disableSAX = 1;
        return;
#endif
    } else {
        doc = ctxt->myDoc;
        if (doc == NULL)
            doc = ctxt->myDoc = xmlNewDoc(ctxt->version);
        if (doc != NULL) {
            if (doc->children == NULL) {
                if (ctxt->encoding != NULL)
                    doc->encoding = xmlStrdup(ctxt->encoding);
                else
                    doc->encoding = NULL;
                doc->standalone = ctxt->standalone;
            }
        } else {
            if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                ctxt->sax->error(ctxt->userData,
                                 EMBED_ERRTXT("SAX.startDocument(): out of memory\n"));
            ctxt->errNo = XML_ERR_NO_MEMORY;
            ctxt->instate = XML_PARSER_EOF;
            ctxt->disableSAX = 1;
            return;
        }
    }
    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->URL == NULL) &&
        (ctxt->input != NULL) && (ctxt->input->filename != NULL)) {
        ctxt->myDoc->URL =
            xmlCanonicPath((const xmlChar *) ctxt->input->filename);
        if (ctxt->myDoc->URL == NULL)
            ctxt->myDoc->URL =
                xmlStrdup((const xmlChar *) ctxt->input->filename);
    }
}

/**
 * xmlTextWriterSetIndent:
 * @param writer the xmlTextWriterPtr
 * @param indent do indentation?
 *
 * Set indentation output. indent = 0 do not indentation. indent > 0 do indentation.
 *
 * Returns -1 on error or 0 otherwise.
 */
XMLPUBFUNEXPORT int
xmlTextWriterSetIndent(xmlTextWriterPtr writer, int indent)
{
    if (indent < 0)
        return -1;

    writer->indent = indent;
    writer->doindent = 1;

    return 0;
}

/**
 * xmlTextWriterSetIndentString:
 * @param writer the xmlTextWriterPtr
 * @param str the xmlChar string
 *
 * Set string indentation.
 *
 * Returns -1 on error or 0 otherwise.
 */
XMLPUBFUNEXPORT int
xmlTextWriterSetIndentString(xmlTextWriterPtr writer, const xmlChar * str)
{
    if (!str)
        return -1;

    if (writer->ichar != NULL)
        xmlFree(writer->ichar);
    writer->ichar = xmlStrdup(str);

    if (!writer->ichar)
        return -1;
    else
        return 0;
}

/**
 * xmlTextWriterWriteIndent:
 * @param writer the xmlTextWriterPtr
 *
 * Write indent string.
 *
 * Returns -1 on error or the number of strings written.
 */
static int
xmlTextWriterWriteIndent(xmlTextWriterPtr writer)
{
    int lksize;
    int i;
    int ret;

    lksize = xmlListSize(writer->nodes);
    if (lksize < 1)
        return (-1);            /* list is empty */
    for (i = 0; i < (lksize - 1); i++) {
        ret = xmlOutputBufferWriteString(writer->out,
                                         (const char *) writer->ichar);
        if (ret == -1)
            return (-1);
    }

    return (lksize - 1);
}

/**
 * xmlTextWriterHandleStateDependencies:
 * @param writer the xmlTextWriterPtr
 * @param p the xmlTextWriterStackEntry
 *
 * Write state dependent strings.
 *
 * Returns -1 on error or the number of characters written.
 */
static int
xmlTextWriterHandleStateDependencies(xmlTextWriterPtr writer,
                                     xmlTextWriterStackEntry * p)
{
    int count;
    int sum;
    char extra[3];

    if (writer == NULL)
        return -1;

    if (p == NULL)
        return 0;

    sum = 0;
    extra[0] = extra[1] = extra[2] = '\0';
    if (p != 0) {
        sum = 0;
        switch (p->state) {
            case XML_TEXTWRITER_NAME:
                extra[0] = '>';
                p->state = XML_TEXTWRITER_TEXT;
                break;
            case XML_TEXTWRITER_PI:
                extra[0] = ' ';
                p->state = XML_TEXTWRITER_PI_TEXT;
                break;
            case XML_TEXTWRITER_DTD:
                extra[0] = ' ';
                extra[1] = '[';
                p->state = XML_TEXTWRITER_DTD_TEXT;
                break;
            case XML_TEXTWRITER_DTD_ELEM:
                extra[0] = ' ';
                p->state = XML_TEXTWRITER_DTD_ELEM_TEXT;
                break;
            case XML_TEXTWRITER_DTD_ATTL:
                extra[0] = ' ';
                p->state = XML_TEXTWRITER_DTD_ATTL_TEXT;
                break;
            case XML_TEXTWRITER_DTD_ENTY:
            case XML_TEXTWRITER_DTD_PENT:
                extra[0] = ' ';
                extra[1] = writer->qchar;
                p->state = XML_TEXTWRITER_DTD_ENTY_TEXT;
                break;
            default:
                break;
        }
    }

    if (*extra != '\0') {
        count = xmlOutputBufferWriteString(writer->out, extra);
        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

#endif
