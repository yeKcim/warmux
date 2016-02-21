/*
 * libxml2_xmlsave.c: Implementation of the document serializer
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML

#include "xmlenglibxml.h"

#include <string.h>

#include <libxml2_globals.h>
#include <libxml2_parserinternals.h>
#include "libxml2_xmlerror2.h"
#include <libxml2_xmlsave.h>

#ifdef LIBXML_HTML_ENABLED
#include "libxml2_htmltree.h"
#endif
/************************************************************************
 *                                                                      *
 *          XHTML detection                                             *
 *                                                                      *
 ************************************************************************/
#define XHTML_STRICT_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Strict//EN"
#define XHTML_STRICT_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
#define XHTML_FRAME_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Frameset//EN"
#define XHTML_FRAME_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd"
#define XHTML_TRANS_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Transitional//EN"
#define XHTML_TRANS_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"

#define XHTML_NS_NAME BAD_CAST "http://www.w3.org/1999/xhtml"

// Constansts for results that are returned by callbacks of node filter during serialization
const int KFilterAccept = 1;
const int KFilterReject = 2;
const int KFilterSkip   = 3;
const int KFilterSkipElementContents = 4;

/**
 * xmlIsXHTML:
 * @param systemID the system identifier
 * @param publicID the public identifier
 *
 * Try to find if the document correspond to an XHTML DTD
 *
 * Returns 1 if true, 0 if not and -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlIsXHTML(const xmlChar *systemID, const xmlChar *publicID) {
    if ((systemID == NULL) && (publicID == NULL))
    return(-1);
    if (publicID != NULL) {
    if (xmlStrEqual(publicID, XHTML_STRICT_PUBLIC_ID)) return(1);
    if (xmlStrEqual(publicID, XHTML_FRAME_PUBLIC_ID)) return(1);
    if (xmlStrEqual(publicID, XHTML_TRANS_PUBLIC_ID)) return(1);
    }
    if (systemID != NULL) {
    if (xmlStrEqual(systemID, XHTML_STRICT_SYSTEM_ID)) return(1);
    if (xmlStrEqual(systemID, XHTML_FRAME_SYSTEM_ID)) return(1);
    if (xmlStrEqual(systemID, XHTML_TRANS_SYSTEM_ID)) return(1);
    }
    return(0);
}

#ifdef LIBXML_OUTPUT_ENABLED

#define TODO                                \
    xmlGenericError(xmlGenericErrorContext,             \
        "Unimplemented block at %s:%d\n",               \
            __FILE__, __LINE__);



/************************************************************************
 *                                  *
 *          Output error handlers               *
 *                                  *
 ************************************************************************/
/**
 * xmlSaveErrMemory:
 * @param extra extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlSaveErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_OUTPUT, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * xmlSaveErr:
 * @param code the error number
 * @param node the location of the error.
 * @param extra extra informations
 *
 * Handle an out of memory condition
 *
 */
static void
xmlSaveErr(int code, xmlNodePtr node, const char *extra)
{
    const char *msg = NULL;

    switch(code) {
        case XML_SAVE_NOT_UTF8:
        msg = EMBED_ERRTXT("string is not in UTF-8");
        break;
    case XML_SAVE_CHAR_INVALID:
        msg = EMBED_ERRTXT("invalid character value");
        break;
    case XML_SAVE_UNKNOWN_ENCODING:
        msg = EMBED_ERRTXT("unknown encoding %s");
        break;
    case XML_SAVE_NO_DOCTYPE:
        msg = EMBED_ERRTXT("document has no DOCTYPE");
        break;
    default:
        msg = EMBED_ERRTXT("unexpected error number");
    }
    __xmlSimpleError(XML_FROM_OUTPUT, code, node, msg, extra);
}





/************************************************************************
 *                                  *
 *          Special escaping routines           *
 *                                  *
 ************************************************************************/
static unsigned char *
xmlSerializeHexCharRef(unsigned char *out, int val) {
    unsigned char *ptr;

    *out++ = '&';
    *out++ = '#';
    *out++ = 'x';
    if (val < 0x10) ptr = out;
    else if (val < 0x100) ptr = out + 1;
    else if (val < 0x1000) ptr = out + 2;
    else if (val < 0x10000) ptr = out + 3;
    else if (val < 0x100000) ptr = out + 4;
    else ptr = out + 5;
    out = ptr + 1;
    while (val > 0) {
        //  OPTIMIZE: Isn't that
        //          bval = (val & 0xF)
        //          *ptr-- = bval + (bval < 10 ? '0'; 'A'-10)
        //        much more efficient???
    switch (val & 0xF) {
        case 0: *ptr-- = '0'; break;
        case 1: *ptr-- = '1'; break;
        case 2: *ptr-- = '2'; break;
        case 3: *ptr-- = '3'; break;
        case 4: *ptr-- = '4'; break;
        case 5: *ptr-- = '5'; break;
        case 6: *ptr-- = '6'; break;
        case 7: *ptr-- = '7'; break;
        case 8: *ptr-- = '8'; break;
        case 9: *ptr-- = '9'; break;
        case 0xA: *ptr-- = 'A'; break;
        case 0xB: *ptr-- = 'B'; break;
        case 0xC: *ptr-- = 'C'; break;
        case 0xD: *ptr-- = 'D'; break;
        case 0xE: *ptr-- = 'E'; break;
        case 0xF: *ptr-- = 'F'; break;
        default: *ptr-- = '0'; break;
    }
    val >>= 4;
    }
    *out++ = ';';
    *out = 0;
    return(out);
}


/**
 * xmlAttrSerializeTxtContent:
 * @param buf the XML buffer output
 * @param doc the document
 * @param attr the attribute node
 * @param string the text content
 *
 * Serialize text attribute values to an xml simple buffer
 *
 * (Previously this function was in xmlsave.c)
 */
XMLPUBFUNEXPORT void
xmlAttrSerializeTxtContent(xmlBufferPtr buf, xmlDocPtr doc,
                           xmlAttrPtr attr, const xmlChar * string)
{
    xmlChar *base, *cur;

    if (string == NULL)
        return;
    base = cur = (xmlChar *) string;
    while (*cur != 0) {
        if (*cur == '\n') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&#10;", 5);
            cur++;
            base = cur;
        } else if (*cur == '\r') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&#13;", 5);
            cur++;
            base = cur;
        } else if (*cur == '\t') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&#9;", 4);
            cur++;
            base = cur;
        } else if (*cur == '"') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&quot;", 6);
            cur++;
            base = cur;
        } else if (*cur == '<') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&lt;", 4);
            cur++;
            base = cur;
        } else if (*cur == '>') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&gt;", 4);
            cur++;
            base = cur;
        } else if (*cur == '&') {
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferAdd(buf, BAD_CAST "&amp;", 5);
            cur++;
            base = cur;
        } else if ((*cur >= 0x80) && ((doc == NULL) ||
                                      (doc->encoding == NULL))) {
            /*
             * We assume we have UTF-8 content.
             */
            unsigned char tmp[10];
            int val = 0, l = 1;

            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            if (*cur < 0xC0) {
                xmlSaveErr(XML_SAVE_NOT_UTF8, (xmlNodePtr) attr, NULL);
                if (doc != NULL)
                    doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");
        xmlSerializeHexCharRef(tmp, *cur);
                xmlBufferAdd(buf, (xmlChar *) tmp, -1);
                cur++;
                base = cur;
                continue;
            } else if (*cur < 0xE0) {
                val = (cur[0]) & 0x1F;
                val <<= 6;
                val |= (cur[1]) & 0x3F;
                l = 2;
            } else if (*cur < 0xF0) {
                val = (cur[0]) & 0x0F;
                val <<= 6;
                val |= (cur[1]) & 0x3F;
                val <<= 6;
                val |= (cur[2]) & 0x3F;
                l = 3;
            } else if (*cur < 0xF8) {
                val = (cur[0]) & 0x07;
                val <<= 6;
                val |= (cur[1]) & 0x3F;
                val <<= 6;
                val |= (cur[2]) & 0x3F;
                val <<= 6;
                val |= (cur[3]) & 0x3F;
                l = 4;
            }
            if ((l == 1) || (!IS_CHAR(val))) {
                xmlSaveErr(XML_SAVE_CHAR_INVALID, (xmlNodePtr) attr, NULL);
                if (doc != NULL)
                    doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");

        xmlSerializeHexCharRef(tmp, *cur);
                xmlBufferAdd(buf, (xmlChar *) tmp, -1);
                cur++;
                base = cur;
                continue;
            }
            /*
             * We could do multiple things here. Just save
             * as a char ref
             */
        xmlSerializeHexCharRef(tmp, val);
            xmlBufferAdd(buf, (xmlChar *) tmp, -1);
            cur += l;
            base = cur;
        } else {
            cur++;
        }
    }
    if (base != cur)
        xmlBufferAdd(buf, base, cur - base);
}

/**
 * xmlEscapeEntities:
 * @param out a pointer to an array of bytes to store the result
 * @param outlen the length of out
 * @param in a pointer to an array of unescaped UTF-8 bytes
 * @param inlen the length of in
 *
 * Take a block of UTF-8 chars in and escape them. Used when there is no
 * encoding specified.
 *
 * Returns 0 if success, or -1 otherwise
 * The value of inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of outlen after return is the number of octets consumed.
 */
static int
xmlEscapeEntities(unsigned char* out, int *outlen,
                 const xmlChar* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    int val;

    inend = in + (*inlen);

    while ((in < inend) && (out < outend)) {
        if (*in == '<') {
        if (outend - out < 4) break;
        *out++ = '&';
        *out++ = 'l';
        *out++ = 't';
        *out++ = ';';
        in++;
        continue;
    } else if (*in == '>') {
        if (outend - out < 4) break;
        *out++ = '&';
        *out++ = 'g';
        *out++ = 't';
        *out++ = ';';
        in++;
        continue;
    } else if (*in == '&') {
        if (outend - out < 5) break;
        *out++ = '&';
        *out++ = 'a';
        *out++ = 'm';
        *out++ = 'p';
        *out++ = ';';
        in++;
        continue;
    } else if (((*in >= 0x20) && (*in < 0x80)) ||
               (*in == '\n') || (*in == '\t')) {
        /*
         * default case, just copy !
         */
        *out++ = *in++;
        continue;
    } else if (*in >= 0x80) {
        /*
         * We assume we have UTF-8 input.
         */
        if (outend - out < 10) break;

        if (*in < 0xC0) {
        xmlSaveErr(XML_SAVE_NOT_UTF8, NULL, NULL);
        in++;
        goto error;
        } else if (*in < 0xE0) {
        if (inend - in < 2) break;
        val = (in[0]) & 0x1F;
        val <<= 6;
        val |= (in[1]) & 0x3F;
        in += 2;
        } else if (*in < 0xF0) {
        if (inend - in < 3) break;
        val = (in[0]) & 0x0F;
        val <<= 6;
        val |= (in[1]) & 0x3F;
        val <<= 6;
        val |= (in[2]) & 0x3F;
        in += 3;
        } else if (*in < 0xF8) {
        if (inend - in < 4) break;
        val = (in[0]) & 0x07;
        val <<= 6;
        val |= (in[1]) & 0x3F;
        val <<= 6;
        val |= (in[2]) & 0x3F;
        val <<= 6;
        val |= (in[3]) & 0x3F;
        in += 4;
        } else {
        xmlSaveErr(XML_SAVE_CHAR_INVALID, NULL, NULL);
        in++;
        goto error;
        }
        if (!IS_CHAR(val)) {
        xmlSaveErr(XML_SAVE_CHAR_INVALID, NULL, NULL);
        in++;
        goto error;
        }

        /*
         * We could do multiple things here. Just save as a char ref
         */
        out = xmlSerializeHexCharRef(out, val);
    } else if (IS_BYTE_CHAR(*in)) {
        if (outend - out < 6) break;
        out = xmlSerializeHexCharRef(out, *in++);
    } else {
        xmlGenericError(xmlGenericErrorContext,
        "xmlEscapeEntities : char out of range\n");
        in++;
        goto error;
    }
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return(0);
error:
    *outlen = out - outstart;
    *inlen = in - base;
    return(-1);
}

/************************************************************************
 *                                                                      *
 *          Allocation and deallocation                                 *
 *                                                                      *
 ************************************************************************/
/**
 * xmlSaveCtxtInit:
 * @param ctxt the saving context
 *
 * Initialize a saving context
 *
 * OOM: never
 */
//static

XMLPUBFUNEXPORT void
xmlSaveCtxtInit(xmlSaveCtxtPtr ctxt)
{
    int i;

    if (ctxt == NULL) return;
    if ((ctxt->encoding == NULL) && (ctxt->escape == NULL))
        ctxt->escape = xmlEscapeEntities;
    if (xmlTreeIndentString == NULL) {
        memset(&ctxt->indent[0], 0, MAX_INDENT + 1);
    } else {
        ctxt->indent_size = xmlStrlen((const xmlChar *) xmlTreeIndentString);
        ctxt->indent_nr = MAX_INDENT / ctxt->indent_size;
        for (i = 0;i < ctxt->indent_nr;i++)
            memcpy(&ctxt->indent[i * ctxt->indent_size], xmlTreeIndentString,
                    ctxt->indent_size);
            ctxt->indent[ctxt->indent_nr * ctxt->indent_size] = 0;
    }
}

/**
 * xmlFreeSaveCtxt:
 *
 * Free a saving context, destroying the ouptut in any remaining buffer
 */
static void
xmlFreeSaveCtxt(xmlSaveCtxtPtr ctxt)
{
    if (ctxt == NULL) return;
    if (ctxt->encoding != NULL)
        xmlFree((char *) ctxt->encoding);
    if (ctxt->buf != NULL)
        xmlOutputBufferClose(ctxt->buf);
    xmlFree(ctxt);
}

/**
 * xmlNewSaveCtxt:
 *
 * Create a new saving context
 *
 * Returns the new structure or NULL in case of error
 */
static xmlSaveCtxtPtr
xmlNewSaveCtxt(const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = (xmlSaveCtxtPtr) xmlMalloc(sizeof(xmlSaveCtxt));
    if (ret == NULL) {
    xmlSaveErrMemory("creating saving context");
    return ( NULL );
    }
    memset(ret, 0, sizeof(xmlSaveCtxt));
    ret->options = options;
    if (encoding != NULL) {
        ret->handler = xmlFindCharEncodingHandler(encoding);
    if (ret->handler == NULL) {
        xmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, NULL, encoding);
            xmlFreeSaveCtxt(ret);
        return(NULL);
    }
        ret->encoding = xmlStrdup((const xmlChar *)encoding);
    ret->escape = xmlEscapeEntities;
    }
    xmlSaveCtxtInit(ret);

    return(ret);
}

/************************************************************************
 *                                                                      *
 *          Dumping XML tree content to a simple buffer                 *
 *                                                                      *
 ************************************************************************/
/**
 * xmlAttrSerializeContent:
 * @param buf the XML buffer output
 * @param doc the document
 * @param attr the attribute pointer
 *
 * Serialize the attribute in the buffer
 */
static void
xmlAttrSerializeContent(xmlOutputBufferPtr buf, xmlAttrPtr attr)
{
    xmlNodePtr children;

    children = attr->children;
    while (children != NULL) {
        switch (children->type) {
            case XML_TEXT_NODE:
            xmlAttrSerializeTxtContent(buf->buffer, attr->doc,
                                   attr, children->content);
        break;
            case XML_ENTITY_REF_NODE:
                xmlBufferAdd(buf->buffer, BAD_CAST "&", 1);
                xmlBufferAdd(buf->buffer, children->name,
                             xmlStrlen(children->name));
                xmlBufferAdd(buf->buffer, BAD_CAST ";", 1);
                break;
            default:
                /* should not happen unless we have a badly built tree */
                break;
        }
        children = children->next;
    }
}

/************************************************************************
 *                                                                      *
 *          Dumping XML tree content to an I/O output buffer            *
 *                                                                      *
 ************************************************************************/

#ifdef LIBXML_HTML_ENABLED
static void
xhtmlNodeDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur);
#endif
static void xmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur);

//were static, now exported
//void xmlNodeDumpOutputInternal(xmlSaveCtxtPtr ctxt, xmlNodePtr cur);
//void xmlNsListDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur);

/**
 * xmlNsDumpOutput:
 * @param buf the XML buffer output
 * @param cur a namespace
 *
 * Dump a local Namespace definition.
 * Should be called in the context of attributes dumps.
 */
static void
xmlNsDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    if (cur == NULL) return;
    if ((cur->type == XML_LOCAL_NAMESPACE) &&
        (cur->href != NULL))
    {
        if (xmlStrEqual(cur->prefix, BAD_CAST "xml"))
            return;

            /* Within the context of an element attributes */
        if (cur->prefix != NULL) {
            xmlOutputBufferWrite(buf, 7, " xmlns:");
            xmlOutputBufferWriteString(buf, (const char *)cur->prefix);
        }else{
             xmlOutputBufferWrite(buf, 6, " xmlns");
        }
        xmlOutputBufferWrite(buf, 1, "=");
        xmlBufferWriteQuotedString(buf->buffer, cur->href);
    }
}

/**
 * xmlNsListDumpOutput:
 * @param buf the XML buffer output
 * @param cur the first namespace
 *
 * Dump a list of local Namespace definitions.
 * Should be called in the context of attributes dumps.
 */
void
xmlNsListDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    while (cur != NULL) {
        xmlNsDumpOutput(buf, cur);
        cur = cur->next;
    }
}

/**
 * xmlDtdDumpOutput:
 * @param buf the XML buffer output
 * @param dtd the pointer to the DTD
 *
 * Dump the XML document DTD, if any.
 */
static void
xmlDtdDumpOutput(xmlSaveCtxtPtr ctxt, xmlDtdPtr dtd)
{
    xmlOutputBufferPtr buf;
    int format, level;
    xmlDocPtr doc;

    if (!dtd || !ctxt || !ctxt->buf)
        return;

    buf = ctxt->buf;
    xmlOutputBufferWrite(buf, 10, "<!DOCTYPE ");
    xmlOutputBufferWriteString(buf, (const char*)dtd->name);
    if (dtd->ExternalID)
    {
        xmlOutputBufferWrite(buf, 8, " PUBLIC ");
        xmlBufferWriteQuotedString(buf->buffer, dtd->ExternalID);
        xmlOutputBufferWrite(buf, 1, " ");
        xmlBufferWriteQuotedString(buf->buffer, dtd->SystemID);
    }
    else if (dtd->SystemID)
    {
        xmlOutputBufferWrite(buf, 8, " SYSTEM ");
        xmlBufferWriteQuotedString(buf->buffer, dtd->SystemID);
    }
    if (!dtd->entities && !dtd->elements &&
        !dtd->attributes && !dtd->notations &&
        !dtd->pentities)
    {
        xmlOutputBufferWrite(buf, 1, ">");
        return;
    }
    xmlOutputBufferWrite(buf, 3, " [\n");
    format = ctxt->format;
    level = ctxt->level;
    doc = ctxt->doc;
    ctxt->format = 0;
    ctxt->level = -1;
    ctxt->doc = dtd->doc;
    xmlNodeListDumpOutput(ctxt, dtd->children);
    ctxt->format = format;
    ctxt->level = level;
    ctxt->doc = doc;
    xmlOutputBufferWrite(buf, 2, "]>");
}

/**
 * xmlAttrDumpOutput:
 * @param buf the XML buffer output
 * @param cur the attribute pointer
 *
 * Dump an XML attribute
 */
static void
xmlAttrDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    xmlOutputBufferWrite(buf, 1, " ");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
    xmlOutputBufferWrite(buf, 1, ":");
    }
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWrite(buf, 2, "=\"");
    xmlAttrSerializeContent(buf, cur);
    xmlOutputBufferWrite(buf, 1, "\"");
}

/**
 * xmlAttrListDumpOutput:
 * @param buf the XML buffer output
 * @param doc the document
 * @param cur the first attribute pointer
 * @param encoding an optional encoding string
 *
 * Dump a list of XML attributes
 */
static void
xmlAttrListDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    if (cur == NULL) return;
    while (cur != NULL) {
        xmlAttrDumpOutput(ctxt, cur);
    cur = cur->next;
    }
}



/**
 * xmlNodeListDumpOutput:
 * @param cur the first node
 *
 * Dump an XML node list, recursive behaviour, children are printed too.
 */
static void
xmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur)
{
    xmlOutputBufferPtr buf;
    LOAD_GS_SAFE_NODE(cur)

    if (cur == NULL) return;
    buf = ctxt->buf;
    while (cur != NULL)
    {
        if ((ctxt->format) && (xmlIndentTreeOutput) &&
            (cur->type == XML_ELEMENT_NODE))
        {
            xmlOutputBufferWrite(buf, ctxt->indent_size *
                                 (ctxt->level > ctxt->indent_nr ?
                      ctxt->indent_nr : ctxt->level),
                     ctxt->indent);
        }
        xmlNodeDumpOutputInternal(ctxt, cur);
        if (ctxt->format) {
            xmlOutputBufferWrite(buf, 1, "\n");
        }
        cur = cur->next;
    }
}

/**
 * xmlNodeDumpOutputInternal:
 * @param cur the current node
 *
 * Dump an XML node, recursive behaviour, children are printed too.
 */
//static // it was made exported
XMLPUBFUNEXPORT void
xmlNodeDumpOutputInternal(xmlSaveCtxtPtr ctxt, xmlNodePtr cur)
{
    int format;
//    int ret;
    xmlNodePtr tmp;
    xmlChar *start, *end;// *dataStub;
    xmlOutputBufferPtr buf;
    
#ifdef XMLENGINE_NODEFILTER
    xmlNodeFilterData* filterData = ctxt->nodeFilter;
    int skipMode = 0; // normal mode
    LOAD_GS_SAFE_NODE(cur)
    if(filterData)
        {
        switch(filterData->proxyFn(filterData->fn, cur))
            {
            
            //       Use constants.
            case 2 /* KFilterReject */:
                   return;
            case 3 /* KFilterSkip */:
                   skipMode = 1; // Skip node
                   break;
            case 4 /* KFilterSkipElementContents */:
                   skipMode = (cur->type == XML_ELEMENT_NODE) ? 2 : 0;
                   break;
            default: ;
                // DO NOTHING, though the result is bogus
            }
        }
#else
	LOAD_GS_SAFE_NODE(cur)
#endif
    if (!cur)
        return;

    buf = ctxt->buf;

    // DONE: OPTIMIZATION: quicker selection of code flow
    if(cur->type != XML_ELEMENT_NODE)
    {
        
        /* Disabled:
        if (cur->type == XML_XINCLUDE_START)
            return;
        if (cur->type == XML_XINCLUDE_END)
            return;
        */
        if (cur->type == XML_ATTRIBUTE_NODE) {
            xmlAttrDumpOutput(ctxt, (xmlAttrPtr) cur);
            return;
        }
        if (cur->type == XML_NAMESPACE_DECL) {
            xmlNsDumpOutput(buf, (xmlNsPtr) cur);
            return;
        }
        if (cur->type == XML_TEXT_NODE) {
            if (cur->content) {
			    if (TEXT_IS_DATA(cur) ){
			    	if( ctxt->textNodeCallback){
						xmlChar* output = NULL;			    
						int written = 0;
						if (buf->writecallback) {
	                            /*
	                             * second write the stuff to the I/O channel
	                             */
							if (buf->encoder != NULL) {
								written = buf->writecallback(buf->context,
									(const char *)buf->conv->content, buf->conv->use);
								if (written >= 0)
	                                    xmlBufferShrink(buf->conv, written);
	                            } else {
	                                written = buf->writecallback(buf->context,
	                                     (const char *)buf->buffer->content, buf->buffer->use);
	                                if (written >= 0)
	                                    xmlBufferShrink(buf->buffer, written);
	                            }
	                            if (written < 0) {
	                                __xmlIOErr(XML_FROM_IO, XML_IO_WRITE, NULL);
	                                buf->error = written;
	                                return;
	                            }
							buf->written += written;
						}					
						output = ctxt->textNodeCallback(ctxt->context, cur, &written);
						if(written)
						xmlOutputBufferWrite(buf, written, (const char *) output);
			    	}
				}
			    else{
			        if (cur->name != xmlStringTextNoenc){
               		    xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);	
			        }
                    else{
                        /*
                        * Disable escaping, needed for XSLT
                        */
                   		xmlOutputBufferWriteString(buf, (const char *) cur->content); 
                    }
				}
            }
            return;
        }
        if (cur->type == XML_CDATA_SECTION_NODE) {
            start = end = cur->content;
            while (*end != '\0') {
                if ((*end == ']') &&
                    (*(end + 1) == ']') &&
                    (*(end + 2) == '>'))
                {
                    end = end + 2;
                    xmlOutputBufferWrite(buf, 9, "<![CDATA[");
                    xmlOutputBufferWrite(buf, end - start, (const char *)start);
                    xmlOutputBufferWrite(buf, 3, "]]>");
                    start = end;
                }
                end++;
            }
            if (start != end) {
                xmlOutputBufferWrite(buf, 9, "<![CDATA[");
                xmlOutputBufferWriteString(buf, (const char *)start);
                xmlOutputBufferWrite(buf, 3, "]]>");
            }
            return;
        }
        if (cur->type == XML_PI_NODE) {
            // DONE: correct program flow / exclude redundant IF check
            xmlOutputBufferWrite(buf, 2, "<?");
            xmlOutputBufferWriteString(buf, (const char*)cur->name);
            if (cur->content) {
                xmlOutputBufferWrite(buf, 1, " ");
                xmlOutputBufferWriteString(buf, (const char*)cur->content);
            }
            xmlOutputBufferWrite(buf, 2, "?>");
            return;
        }
        if (cur->type == XML_COMMENT_NODE) {
            if (cur->content) {
                xmlOutputBufferWrite(buf, 4, "<!--");
                xmlOutputBufferWriteString(buf, (const char*)cur->content);
                xmlOutputBufferWrite(buf, 3, "-->");
            }
            return;
        }
        if (cur->type == XML_ENTITY_REF_NODE) {
            xmlOutputBufferWrite(buf, 1, "&");
            xmlOutputBufferWriteString(buf, (const char *)cur->name);
            xmlOutputBufferWrite(buf, 1, ";");
            return;
        }
        if (cur->type == XML_DOCUMENT_NODE ||
            cur->type == XML_DOCUMENT_FRAG_NODE)
        {
            xmlNodeListDumpOutput(ctxt, cur->children);
            return;
        }
        if (cur->type == XML_DTD_NODE) {
            xmlDtdDumpOutput(ctxt, (xmlDtdPtr) cur);
            return;
        }
        if (cur->type == XML_ELEMENT_DECL) {
            xmlDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
            return;
        }
        if (cur->type == XML_ATTRIBUTE_DECL) {
            xmlDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
            return;
        }
        if (cur->type == XML_ENTITY_DECL) {
            xmlDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
            return;
        }
        return; // defaulted action for other types of nodes
    } // end of "if not element node"
    //
    //XE_ASSERT_DEBUG(cur->type == XML_ELEMENT_NODE);
    //
    format = ctxt->format;
    if (format == 1) {
        tmp = cur->children;
        while (tmp) {
            if (tmp->type == XML_TEXT_NODE          ||
                tmp->type == XML_CDATA_SECTION_NODE ||
                tmp->type == XML_ENTITY_REF_NODE)
            {
                ctxt->format = 0;
                break;
            }
            tmp = tmp->next;
        }
    }

#ifdef XMLENGINE_NODEFILTER
    if (skipMode != 1 /* skip element */)
    {
#endif
    xmlOutputBufferWrite(buf, 1, "<");
    if (cur->ns && cur->ns->prefix) {
        xmlOutputBufferWriteString(buf, (const char*)cur->ns->prefix);
        xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char*)cur->name);
    if (cur->nsDef)
        xmlNsListDumpOutput(buf, cur->nsDef);
    if (cur->properties)
        xmlAttrListDumpOutput(ctxt, cur->properties);
#ifdef XMLENGINE_NODEFILTER
    }
#endif

#ifdef XMLENGINE_NODEFILTER
    if(skipMode != 1)
    {
    // XMLENGINE: BEGIN REPLACE CODE
    // skipMode check is inserted
    if ( (skipMode == 2 /* skip contents */
            ||
            (
                !cur->content
                 &&
                !cur->children
             )
         )
         && !xmlSaveNoEmptyTags
       )
#else // OLD CODE:
    if (!cur->content  &&
        !cur->children &&
        !xmlSaveNoEmptyTags)
#endif
    {
        xmlOutputBufferWrite(buf, 2, "/>");
        ctxt->format = format;
        return;
    }
    xmlOutputBufferWrite(buf, 1, ">");
#ifdef XMLENGINE_NODEFILTER
    } // if(skipMode != 1)
#endif

#ifdef XMLENGINE_NODEFILTER
    if(skipMode != 2){
#endif
    
    if (cur->type != XML_ELEMENT_NODE &&
        cur->content)
    {
        xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
    }

    if (cur->children) {
        if (ctxt->format)
            xmlOutputBufferWrite(buf, 1, "\n");
        if (ctxt->level >= 0)
            ctxt->level++;
        xmlNodeListDumpOutput(ctxt, cur->children);
        if (ctxt->level > 0)
            ctxt->level--;
        if ((xmlIndentTreeOutput) && (ctxt->format))
        {
            xmlOutputBufferWrite(
                buf,
                ctxt->indent_size *
                    (ctxt->level > ctxt->indent_nr
                        ? ctxt->indent_nr
                        : ctxt->level),
                ctxt->indent
             );
        }
    }
#ifdef XMLENGINE_NODEFILTER
    } // if(skipMode != 2) // not skipping children & contents
#endif

#ifdef XMLENGINE_NODEFILTER
    if(skipMode != 1){
#endif
    xmlOutputBufferWrite(buf, 2, "</");
    if (cur->ns && cur->ns->prefix) {
        xmlOutputBufferWriteString(buf, (const char*) cur->ns->prefix);
        xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char*) cur->name);
    xmlOutputBufferWrite(buf, 1, ">");
#ifdef XMLENGINE_NODEFILTER
    } //if(skipMode != 1)
#endif

    ctxt->format = format;
}

/**
 * xmlDocContentDumpOutput:
 * @param cur the document
 *
 * Dump an XML document.
 */
void
xmlDocContentDumpOutput(xmlSaveCtxtPtr ctxt, xmlDocPtr cur)
{
		
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;
#endif
    const xmlChar* oldenc = cur->encoding;
    const xmlChar* encoding = ctxt->encoding;
    xmlOutputBufferPtr buf;
    LOAD_GS_SAFE_DOC(cur)

    xmlInitParser();
    if(OOM_FLAG)
        return;

    if (ctxt->encoding)
        cur->encoding = BAD_CAST ctxt->encoding;

    buf = ctxt->buf;
    xmlOutputBufferWrite(buf, 14, "<?xml version=");
    if (cur->version != NULL)
        xmlBufferWriteQuotedString(buf->buffer, cur->version);
    else
        xmlOutputBufferWrite(buf, 5, "\"1.0\"");

    if (!ctxt->encoding) {
        if (cur->encoding)
            encoding = cur->encoding;
        else if (cur->charset != XML_CHAR_ENCODING_UTF8)
            encoding = (const xmlChar *)
        xmlGetCharEncodingName((xmlCharEncoding) cur->charset);
    }
    if (encoding) {
        xmlOutputBufferWrite(buf, 10, " encoding=");
        xmlBufferWriteQuotedString(buf->buffer, (xmlChar*) encoding);
    }
    switch (cur->standalone) {
        case 0:
            xmlOutputBufferWrite(buf, 16, " standalone=\"no\"");
            break;
        case 1:
            xmlOutputBufferWrite(buf, 17, " standalone=\"yes\"");
            break;
    }
    xmlOutputBufferWrite(buf, 3, "?>\n");

#ifdef LIBXML_HTML_ENABLED
    dtd = xmlGetIntSubset(cur);
    if (dtd) {
        is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
        if (is_xhtml < 0) is_xhtml = 0;
    }
    if (is_xhtml) {
        if (encoding)
            htmlSetMetaEncoding(cur, (const xmlChar*) ctxt->encoding);
        else
            htmlSetMetaEncoding(cur, BAD_CAST "UTF-8");
    }
#endif
    if (cur->children) {
        xmlNodePtr child = cur->children;

        while (child) {
            ctxt->level = 0;
#ifdef LIBXML_HTML_ENABLED
            if (is_xhtml){
                xhtmlNodeDumpOutput(ctxt, child);
            } else
#endif
            {
                xmlNodeDumpOutputInternal(ctxt, child);
            }
            xmlOutputBufferWrite(buf, 1, "\n");
            child = child->next;
        }
    }
    if (ctxt->encoding)
        cur->encoding = oldenc;
}

#ifdef LIBXML_HTML_ENABLED
/************************************************************************
 *                                                                      *
 *      Functions specific to XHTML serialization                       *
 *                                                                      *
 ************************************************************************/

/**
 * xhtmlIsEmpty:
 * @param node the node
 *
 * Check if a node is an empty xhtml node
 *
 * Returns 1 if the node is an empty node, 0 if not and -1 in case of error
 */
static int
xhtmlIsEmpty(xmlNodePtr node)
{
    if (node == NULL)
    return(-1);
    if (node->type != XML_ELEMENT_NODE)
    return(0);
    if ((node->ns != NULL) && (!xmlStrEqual(node->ns->href, XHTML_NS_NAME)))
    return(0);
    if (node->children != NULL)
    return(0);
    switch (node->name[0]) {
    case 'a':
        if (xmlStrEqual(node->name, BAD_CAST "area"))
        return(1);
        return(0);
    case 'b':
        if (xmlStrEqual(node->name, BAD_CAST "br"))
        return(1);
        if (xmlStrEqual(node->name, BAD_CAST "base"))
        return(1);
        if (xmlStrEqual(node->name, BAD_CAST "basefont"))
        return(1);
        return(0);
    case 'c':
        if (xmlStrEqual(node->name, BAD_CAST "col"))
        return(1);
        return(0);
    case 'f':
        if (xmlStrEqual(node->name, BAD_CAST "frame"))
        return(1);
        return(0);
    case 'h':
        if (xmlStrEqual(node->name, BAD_CAST "hr"))
        return(1);
        return(0);
    case 'i':
        if (xmlStrEqual(node->name, BAD_CAST "img"))
        return(1);
        if (xmlStrEqual(node->name, BAD_CAST "input"))
        return(1);
        if (xmlStrEqual(node->name, BAD_CAST "isindex"))
        return(1);
        return(0);
    case 'l':
        if (xmlStrEqual(node->name, BAD_CAST "link"))
        return(1);
        return(0);
    case 'm':
        if (xmlStrEqual(node->name, BAD_CAST "meta"))
        return(1);
        return(0);
    case 'p':
        if (xmlStrEqual(node->name, BAD_CAST "param"))
        return(1);
        return(0);
    }
    return(0);
}

/**
 * xhtmlAttrListDumpOutput:
 * @param cur the first attribute pointer
 *
 * Dump a list of XML attributes
 */
static void
xhtmlAttrListDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur)
{
    xmlAttrPtr xml_lang = NULL;
    xmlAttrPtr lang = NULL;
    xmlAttrPtr name = NULL;
    xmlAttrPtr id = NULL;
    xmlNodePtr parent;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    parent = cur->parent;
    while (cur != NULL) {
    if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "id")))
        id = cur;
    else
    if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "name")))
        name = cur;
    else
    if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")))
        lang = cur;
    else
    if ((cur->ns != NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")) &&
        (xmlStrEqual(cur->ns->prefix, BAD_CAST "xml")))
        xml_lang = cur;
    else if ((cur->ns == NULL) &&
         ((cur->children == NULL) ||
          (cur->children->content == NULL) ||
          (cur->children->content[0] == 0)) &&
         (htmlIsBooleanAttr(cur->name))) {
        if (cur->children != NULL)
        xmlFreeNode(cur->children);
        cur->children = xmlNewText(cur->name);
        if (cur->children != NULL)
        cur->children->parent = (xmlNodePtr) cur;
    }
        xmlAttrDumpOutput(ctxt, cur);
    cur = cur->next;
    }
    /*
     * C.8
     */
    if ((name != NULL) && (id == NULL)) {
    if ((parent != NULL) && (parent->name != NULL) &&
        ((xmlStrEqual(parent->name, BAD_CAST "a")) ||
         (xmlStrEqual(parent->name, BAD_CAST "p")) ||
         (xmlStrEqual(parent->name, BAD_CAST "div")) ||
         (xmlStrEqual(parent->name, BAD_CAST "img")) ||
         (xmlStrEqual(parent->name, BAD_CAST "map")) ||
         (xmlStrEqual(parent->name, BAD_CAST "applet")) ||
         (xmlStrEqual(parent->name, BAD_CAST "form")) ||
         (xmlStrEqual(parent->name, BAD_CAST "frame")) ||
         (xmlStrEqual(parent->name, BAD_CAST "iframe")))) {
        xmlOutputBufferWrite(buf, 5, " id=\"");
        xmlAttrSerializeContent(buf, name);
        xmlOutputBufferWrite(buf, 1, "\"");
    }
    }
    /*
     * C.7.
     */
    if ((lang != NULL) && (xml_lang == NULL)) {
    xmlOutputBufferWrite(buf, 11, " xml:lang=\"");
    xmlAttrSerializeContent(buf, lang);
    xmlOutputBufferWrite(buf, 1, "\"");
    } else
    if ((xml_lang != NULL) && (lang == NULL)) {
    xmlOutputBufferWrite(buf, 7, " lang=\"");
    xmlAttrSerializeContent(buf, xml_lang);
    xmlOutputBufferWrite(buf, 1, "\"");
    }
}

/**
 * xhtmlNodeListDumpOutput:
 * @param buf the XML buffer output
 * @param doc the XHTML document
 * @param cur the first node
 * @param level the imbrication level for indenting
 * @param format is formatting allowed
 * @param encoding an optional encoding string
 *
 * Dump an XML node list, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xhtmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur)
{
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    while (cur != NULL) {
    if ((ctxt->format) && (xmlIndentTreeOutput) &&
        (cur->type == XML_ELEMENT_NODE))
        xmlOutputBufferWrite(buf, ctxt->indent_size *
                             (ctxt->level > ctxt->indent_nr ?
                  ctxt->indent_nr : ctxt->level),
                 ctxt->indent);
        xhtmlNodeDumpOutput(ctxt, cur);
    if (ctxt->format) {
        xmlOutputBufferWrite(buf, 1, "\n");
    }
    cur = cur->next;
    }
}

/**
 * xhtmlNodeDumpOutput:
 * @param buf the XML buffer output
 * @param doc the XHTML document
 * @param cur the current node
 * @param level the imbrication level for indenting
 * @param format is formatting allowed
 * @param encoding an optional encoding string
 *
 * Dump an XHTML node, recursive behaviour, children are printed too.
 */
static void
xhtmlNodeDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur)
{
    int format;
    xmlNodePtr tmp;
    xmlChar *start, *end;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    if (cur->type == XML_XINCLUDE_START)
    return;
    if (cur->type == XML_XINCLUDE_END)
    return;
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(ctxt, (xmlDtdPtr) cur);
    return;
    }
    buf = ctxt->buf;
    if (cur->type == XML_ELEMENT_DECL) {
        xmlDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
    return;
    }
    if (cur->type == XML_ATTRIBUTE_DECL) {
        xmlDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
    return;
    }
    if (cur->type == XML_ENTITY_DECL) {
        xmlDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
    return;
    }
    if (cur->type == XML_TEXT_NODE) {
    if (cur->content != NULL) {
        if ((cur->name == xmlStringText) ||
        (cur->name != xmlStringTextNoenc)) {
                xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
        } else {
        /*
         * Disable escaping, needed for XSLT
         */
        xmlOutputBufferWriteString(buf, (const char *) cur->content);
        }
    }

    return;
    }
    if (cur->type == XML_PI_NODE) {
    if (cur->content != NULL) {
        xmlOutputBufferWrite(buf, 2, "<?");
        xmlOutputBufferWriteString(buf, (const char *)cur->name);
        if (cur->content != NULL) {
        xmlOutputBufferWrite(buf, 1, " ");
        xmlOutputBufferWriteString(buf, (const char *)cur->content);
        }
        xmlOutputBufferWrite(buf, 2, "?>");
    } else {
        xmlOutputBufferWrite(buf, 2, "<?");
        xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWrite(buf, 2, "?>");
    }
    return;
    }
    if (cur->type == XML_COMMENT_NODE) {
    if (cur->content != NULL) {
        xmlOutputBufferWrite(buf, 4, "<!--");
        xmlOutputBufferWriteString(buf, (const char *)cur->content);
        xmlOutputBufferWrite(buf, 3, "-->");
    }
    return;
    }
    if (cur->type == XML_ENTITY_REF_NODE) {
        xmlOutputBufferWrite(buf, 1, "&");
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWrite(buf, 1, ";");
    return;
    }
    if (cur->type == XML_CDATA_SECTION_NODE) {
    start = end = cur->content;
    while (*end != '\0') {
        if (*end == ']' && *(end + 1) == ']' && *(end + 2) == '>') {
        end = end + 2;
        xmlOutputBufferWrite(buf, 9, "<![CDATA[");
        xmlOutputBufferWrite(buf, end - start, (const char *)start);
        xmlOutputBufferWrite(buf, 3, "]]>");
        start = end;
        }
        end++;
    }
    if (start != end) {
        xmlOutputBufferWrite(buf, 9, "<![CDATA[");
        xmlOutputBufferWriteString(buf, (const char *)start);
        xmlOutputBufferWrite(buf, 3, "]]>");
    }
    return;
    }

    format = ctxt->format;
    if (format == 1) {
    tmp = cur->children;
    while (tmp != NULL) {
        if ((tmp->type == XML_TEXT_NODE) ||
        (tmp->type == XML_ENTITY_REF_NODE)) {
        format = 0;
        break;
        }
        tmp = tmp->next;
    }
    }
    xmlOutputBufferWrite(buf, 1, "<");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
    xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->nsDef)
        xmlNsListDumpOutput(buf, cur->nsDef);
    if ((xmlStrEqual(cur->name, BAD_CAST "html") &&
    (cur->ns == NULL) && (cur->nsDef == NULL))) {
    /*
     * 3.1.1. Strictly Conforming Documents A.3.1.1 3/
     */
    xmlOutputBufferWriteString(buf,
        " xmlns=\"http://www.w3.org/1999/xhtml\"");
    }
    if (cur->properties != NULL)
        xhtmlAttrListDumpOutput(ctxt, cur->properties);

    if ((cur->type == XML_ELEMENT_NODE) && (cur->children == NULL)) {
    if (((cur->ns == NULL) || (cur->ns->prefix == NULL)) &&
        (xhtmlIsEmpty(cur) == 1)) {
        /*
         * C.2. Empty Elements
         */
        xmlOutputBufferWrite(buf, 3, " />");
    } else {
        /*
         * C.3. Element Minimization and Empty Element Content
         */
        xmlOutputBufferWrite(buf, 3, "></");
        if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
        xmlOutputBufferWrite(buf, 1, ":");
        }
        xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWrite(buf, 1, ">");
    }
    return;
    }
    xmlOutputBufferWrite(buf, 1, ">");
    if ((cur->type != XML_ELEMENT_NODE) && (cur->content != NULL)) {
    xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
    }

    /*
     * 4.8. Script and Style elements
     */
    if ((cur->type == XML_ELEMENT_NODE) &&
    ((xmlStrEqual(cur->name, BAD_CAST "script")) ||
     (xmlStrEqual(cur->name, BAD_CAST "style"))) &&
    ((cur->ns == NULL) ||
     (xmlStrEqual(cur->ns->href, XHTML_NS_NAME)))) {
    xmlNodePtr child = cur->children;

    while (child != NULL) {
        if ((child->type == XML_TEXT_NODE) ||
        (child->type == XML_CDATA_SECTION_NODE)) {
        /*
         * Apparently CDATA escaping for style just break on IE,
         * mozilla and galeon, so ...
         */
        if (xmlStrEqual(cur->name, BAD_CAST "style") &&
            (xmlStrchr(child->content, '<') == NULL) &&
            (xmlStrchr(child->content, '>') == NULL) &&
            (xmlStrchr(child->content, '&') == NULL)) {
            int level = ctxt->level;
            int indent = ctxt->format;

            ctxt->level = 0;
            ctxt->format = 0;
            xhtmlNodeDumpOutput(ctxt, child);
            ctxt->level = level;
            ctxt->format = indent;
        } else {
            start = end = child->content;
            while (*end != '\0') {
            if (*end == ']' &&
                *(end + 1) == ']' &&
                *(end + 2) == '>') {
                end = end + 2;
                xmlOutputBufferWrite(buf, 9, "<![CDATA[");
                xmlOutputBufferWrite(buf, end - start,
                         (const char *)start);
                xmlOutputBufferWrite(buf, 3, "]]>");
                start = end;
            }
            end++;
            }
            if (start != end) {
            xmlOutputBufferWrite(buf, 9, "<![CDATA[");
            xmlOutputBufferWrite(buf, end - start,
                                 (const char *)start);
            xmlOutputBufferWrite(buf, 3, "]]>");
            }
        }
        } else {
        int level = ctxt->level;
        int indent = ctxt->format;

        ctxt->level = 0;
        ctxt->format = 0;
        xhtmlNodeDumpOutput(ctxt, child);
        ctxt->level = level;
        ctxt->format = indent;
        }
        child = child->next;
    }
    } else if (cur->children != NULL) {
    int indent = ctxt->format;

    if (format) xmlOutputBufferWrite(buf, 1, "\n");
    if (ctxt->level >= 0) ctxt->level++;
    ctxt->format = format;
    xhtmlNodeListDumpOutput(ctxt, cur->children);
    if (ctxt->level > 0) ctxt->level--;
    ctxt->format = indent;
    if ((xmlIndentTreeOutput) && (format))
        xmlOutputBufferWrite(buf, ctxt->indent_size *
                             (ctxt->level > ctxt->indent_nr ?
                  ctxt->indent_nr : ctxt->level),
                 ctxt->indent);
    }
    xmlOutputBufferWrite(buf, 2, "</");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
    xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWrite(buf, 1, ">");
}
#endif

/************************************************************************
 *                                                                      *
 *          Public entry points                                         *
 *                                                                      *
 ************************************************************************/

/**
 * xmlSaveToFd:
 * @param fd a file descriptor number
 * @param encoding the encoding name to use or NULL
 * @param options a set of xmlSaveOptions
 *
 * Create a document saving context serializing to a file descriptor
 * with the encoding and the options given.
 *
 * Returns a new serialization context or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlSaveCtxtPtr
xmlSaveToFd(int fd, const char* encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = xmlNewSaveCtxt(encoding, options);
    if (!ret)
        return ret;
    ret->buf = xmlOutputBufferCreateFd(fd, ret->handler);
    if (!ret->buf) {
        xmlFreeSaveCtxt(ret);
        return(NULL);
    }
    return(ret);
}

/**
 * xmlSaveToFilename:
 * @param filename a file name or an URL
 * @param encoding the encoding name to use or NULL
 * @param options a set of xmlSaveOptions
 *
 * Create a document saving context serializing to a filename or possibly
 * to an URL (but this is less reliable) with the encoding and the options
 * given.
 *
 * Returns a new serialization context or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlSaveCtxtPtr
xmlSaveToFilename(const char* filename, const char* encoding, int options)
{
    xmlSaveCtxtPtr ret;
    int compression = 0; 

    ret = xmlNewSaveCtxt(encoding, options);
    if (!ret)
        return ret /* NULL */;
    ret->buf = xmlOutputBufferCreateFilename(filename, ret->handler, compression);
    if (!ret->buf) {
        xmlFreeSaveCtxt(ret);
        return(NULL);
    }
    return(ret);
}

#if 0
/**
 * xmlSaveToBuffer:
 * @param buffer a buffer
 * @param encoding the encoding name to use or NULL
 * @param options a set of xmlSaveOptions
 *
 * Create a document saving context serializing to a buffer
 * with the encoding and the options given
 *
 * Returns a new serialization context or NULL in case of error.
 */
xmlSaveCtxtPtr
xmlSaveToBuffer(xmlBufferPtr buffer, const char *encoding, int options)
{
    TODO
    return(NULL);
}
#endif

/**
 * xmlSaveToIO:
 * @param iowrite an I/O write function
 * @param ioclose an I/O close function
 * @param ioctx an I/O handler
 * @param encoding the encoding name to use or NULL
 * @param options a set of xmlSaveOptions
 *
 * Create a document saving context serializing to a file descriptor
 * with the encoding and the options given
 *
 * Returns a new serialization context or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlSaveCtxtPtr
xmlSaveToIO(xmlOutputWriteCallback iowrite,
            xmlOutputCloseCallback ioclose,
            void* ioctx, const char* encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = xmlNewSaveCtxt(encoding, options);
    if (ret)
    {
        ret->buf = xmlOutputBufferCreateIO(iowrite, ioclose, ioctx, ret->handler);
        if (!ret->buf) {
            xmlFreeSaveCtxt(ret);
            ret = NULL;
        }
    }
    return(ret);
}

/**
 * xmlSaveDoc:
 * @param ctxt a document saving context
 * @param doc a document
 *
 * Save a full document to a saving context
 * The function is not fully implemented yet as it does not return the
 * byte count but 0 instead
 *
 * Returns the number of byte written or -1 in case of error
 */
XMLPUBFUNEXPORT long
xmlSaveDoc(xmlSaveCtxtPtr ctxt, xmlDocPtr doc)
{
    long ret = 0;

    xmlDocContentDumpOutput(ctxt, doc);
    return(ret);
}

/**
 * xmlSaveTree:
 * @param ctxt a document saving context
 * @param node a document
 *
 * Save a subtree starting at the node parameter to a saving context
 * The function is not fully implemented yet as it does not return the
 * byte count but 0 instead
 *
 * Returns the number of byte written or -1 in case of error
 */
XMLPUBFUNEXPORT long
xmlSaveTree(xmlSaveCtxtPtr ctxt, xmlNodePtr node)
{
    long ret = 0;

    xmlNodeDumpOutputInternal(ctxt, node);
    return(ret);
}

/**
 * xmlSaveFlush:
 * @param ctxt a document saving context
 *
 * Flush a document saving context, i.e. make sure that all bytes have
 * been output.
 *
 * Returns the number of byte written or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlSaveFlush(xmlSaveCtxtPtr ctxt)
{
    if (ctxt == NULL) return(-1);
    if (ctxt->buf == NULL) return(-1);
    return(xmlOutputBufferFlush(ctxt->buf));
}

/**
 * xmlSaveClose:
 * @param ctxt a document saving context
 *
 * Close a document saving context, i.e. make sure that all bytes have
 * been output and free the associated data.
 *
 * Returns the number of byte written or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlSaveClose(xmlSaveCtxtPtr ctxt)
{
    int ret;

    if (ctxt == NULL) return(-1);
    ret = xmlSaveFlush(ctxt);
    xmlFreeSaveCtxt(ctxt);
    return(ret);
}

/**
 * xmlSaveSetEscape:
 * @param ctxt a document saving context
 * @param escape the escaping function
 *
 * Set a custom escaping function to be used for text in element content
 *
 * Returns 0 if successful or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlSaveSetEscape(xmlSaveCtxtPtr ctxt, xmlCharEncodingOutputFunc escape)
{
    if (ctxt == NULL) return(-1);
    ctxt->escape = escape;
    return(0);
}

/**
 * xmlSaveSetAttrEscape:
 * @param ctxt a document saving context
 * @param escape the escaping function
 *
 * Set a custom escaping function to be used for text in attribute content
 *
 * Returns 0 if successful or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlSaveSetAttrEscape(xmlSaveCtxtPtr ctxt, xmlCharEncodingOutputFunc escape)
{
    if (ctxt == NULL) return(-1);
    ctxt->escapeAttr = escape;
    return(0);
}

/************************************************************************
 *                                                                      *
 *      Public entry points based on buffers                            *
 *                                                                      *
 ************************************************************************/
/**
 * xmlAttrSerializeTxtContent:
 * @param buf the XML buffer output
 * @param doc the document
 * @param attr the attribute node
 * @param string the text content
 *
 * Serialize text attribute values to an xml simple buffer
 *
 * (moved to tree.c)
 */



/**
 * xmlNodeDump:
 * @param buf the XML buffer output
 * @param doc the document
 * @param cur the current node
 * @param level the imbrication level for indenting
 * @param format is formatting allowed
 *
 * Dump an XML node, recursive behaviour,children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 *
 * Returns the number of bytes written to the buffer or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur, int level,
            int format)
{
    unsigned int use;
    int ret;
    xmlOutputBufferPtr outbuf;

    xmlInitParser();

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump : node == NULL\n");
#endif
        return (-1);
    }
    if (buf == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump : buf == NULL\n");
#endif
        return (-1);
    }
    outbuf = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (outbuf == NULL) {
        xmlSaveErrMemory("creating buffer");
        return (-1);
    }
    memset(outbuf, 0, (size_t) sizeof(xmlOutputBuffer));
    outbuf->buffer = buf;
    outbuf->encoder = NULL;
    outbuf->writecallback = NULL;
    outbuf->closecallback = NULL;
    outbuf->context = NULL;
    outbuf->written = 0;

    use = buf->use;
    xmlNodeDumpOutput(outbuf, doc, cur, level, format, NULL);
    xmlFree(outbuf);
    ret = buf->use - use;
    return (ret);
}


/**
 * xmlElemDump:
 * @param f the FILE * for the output
 * @param doc the document
 * @param cur the current node
 *
 * Dump an XML/HTML node, recursive behaviour, children are printed too.
 */
void
xmlElemDump(FILE * f, xmlDocPtr doc, xmlNodePtr cur)
{
    xmlOutputBufferPtr outbuf;

    xmlInitParser();

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlElemDump : cur == NULL\n");
#endif
        return;
    }
#ifdef DEBUG_TREE
    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        "xmlElemDump : doc == NULL\n");
    }
#endif

    outbuf = (xmlOutputBufferPtr)xmlOutputBufferCreateFile(f, NULL);
    if (!outbuf)
    {   // OOM!!!
        return;
    }

    if ((doc != NULL) && (doc->type == XML_HTML_DOCUMENT_NODE)) {
#ifdef LIBXML_HTML_ENABLED
        htmlNodeDumpOutput(outbuf, doc, cur, NULL);
#else
        xmlSaveErr(XML_ERR_INTERNAL_ERROR, cur, "HTML support not compiled in\n");
#endif /* LIBXML_HTML_ENABLED */
    } else
        xmlNodeDumpOutput(outbuf, doc, cur, 0, 1, NULL);
    xmlOutputBufferClose(outbuf);
}

/************************************************************************
 *                                                                      *
 *      Saving functions front-ends                                     *
 *                                                                      *
 ************************************************************************/

/**
 * xmlNodeDumpOutput:
 * @param buf the XML buffer output
 * @param doc the document
 * @param cur the current node
 * @param level the imbrication level for indenting
 * @param format is formatting allowed
 * @param encoding an optional encoding string
 *
 * Dump an XML node, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
XMLPUBFUNEXPORT void
xmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
                  int level, int format, const char* encoding)
{
    xmlSaveCtxt ctxt;
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;
#endif

    xmlInitParser();

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = doc;
    ctxt.buf = buf;
    ctxt.level = level;
    ctxt.format = format;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);

#ifdef LIBXML_HTML_ENABLED
    dtd = xmlGetIntSubset(doc);
    if (dtd != NULL) {
        is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
        if (is_xhtml < 0)
            is_xhtml = 0;
        if ((is_xhtml) && (cur->parent == (xmlNodePtr) doc) &&
            (cur->type == XML_ELEMENT_NODE) &&
            (xmlStrEqual(cur->name, BAD_CAST "html"))) {
            if (encoding != NULL)
                htmlSetMetaEncoding((htmlDocPtr) doc,
                                    (const xmlChar *) encoding);
            else
                htmlSetMetaEncoding((htmlDocPtr) doc, BAD_CAST "UTF-8");
        }
    }

    if (is_xhtml)
        xhtmlNodeDumpOutput(&ctxt, cur);
    else
#endif
        xmlNodeDumpOutputInternal(&ctxt, cur);
}

/**
 * xmlDocDumpFormatMemoryEnc:
 * @param out_doc Document to generate XML text from
 * @param doc_txt_ptr Memory pointer for allocated XML text
 * @param doc_txt_len Length of the generated XML text
 * @param txt_encoding Character encoding to use when generating XML text
 * @param format should formatting spaces been added
 *
 * Dump the current DOM tree into memory using the character encoding specified
 * by the caller.  Note it is up to the caller of this function to free the
 * allocated memory with xmlFree().
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */

XMLPUBFUNEXPORT void
xmlDocDumpFormatMemoryEnc(xmlDocPtr out_doc, xmlChar **doc_txt_ptr,
        int * doc_txt_len, const char * txt_encoding,
        int format)
{
    xmlSaveCtxt ctxt;
    int                         dummy = 0;
    xmlOutputBufferPtr          out_buff = NULL;
    xmlCharEncodingHandlerPtr   conv_hdlr = NULL;

    if (doc_txt_len == NULL) {
        doc_txt_len = &dummy;   /*  Continue, caller just won't get length */
    }

    if (doc_txt_ptr == NULL) {
        *doc_txt_len = 0;
        return;
    }

    *doc_txt_ptr = NULL;
    *doc_txt_len = 0;

    if (out_doc == NULL) {
        /*  No document, no output  */
        return;
    }

    /*
     *  Validate the encoding value, if provided.
     *  This logic is copied from xmlSaveFileEnc.
     */

    if (txt_encoding == NULL)
    txt_encoding = (const char *) out_doc->encoding;
    if (txt_encoding != NULL) {
    conv_hdlr = xmlFindCharEncodingHandler(txt_encoding);
    if ( conv_hdlr == NULL ) {
        xmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, (xmlNodePtr) out_doc,
               txt_encoding);
        return;
    }
    }

    if ((out_buff = xmlAllocOutputBuffer(conv_hdlr)) == NULL ) {
        xmlSaveErrMemory("creating buffer");
        return;
    }

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = out_doc;
    ctxt.buf = out_buff;
    ctxt.level = 0;
    ctxt.format = format;
    ctxt.encoding = (const xmlChar *) txt_encoding;
    xmlSaveCtxtInit(&ctxt);
    xmlDocContentDumpOutput(&ctxt, out_doc);
    xmlOutputBufferFlush(out_buff);
    if (out_buff->conv != NULL) {
    *doc_txt_len = out_buff->conv->use;
    *doc_txt_ptr = xmlStrndup(out_buff->conv->content, *doc_txt_len);
    } else {
    *doc_txt_len = out_buff->buffer->use;
    *doc_txt_ptr = xmlStrndup(out_buff->buffer->content, *doc_txt_len);
    }
    (void)xmlOutputBufferClose(out_buff);

    if ((*doc_txt_ptr == NULL) && (*doc_txt_len > 0)) {
        *doc_txt_len = 0;
        xmlSaveErrMemory("creating output");
    }

    return;
}

/**
 * xmlDocDumpMemory:
 * @param cur the document
 * @param mem OUT: the memory pointer
 * @param size OUT: the memory length
 *
 * Dump an XML document in memory and return the #xmlChar * and it's size
 * in bytes. It's up to the caller to free the memory with xmlFree().
 * The resulting byte array is zero terminated, though the last 0 is not
 * included in the returned size.
 */
XMLPUBFUNEXPORT void
xmlDocDumpMemory(xmlDocPtr cur, xmlChar**mem, int *size) {
    xmlDocDumpFormatMemoryEnc(cur, mem, size, NULL, 0);
}

/**
 * xmlDocDumpFormatMemory:
 * @param cur the document
 * @param mem OUT: the memory pointer
 * @param size OUT: the memory length
 * @param format should formatting spaces been added
 *
 *
 * Dump an XML document in memory and return the #xmlChar * and it's size.
 * It's up to the caller to free the memory with xmlFree().
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
XMLPUBFUNEXPORT void
xmlDocDumpFormatMemory(xmlDocPtr cur, xmlChar**mem, int *size, int format) {
    xmlDocDumpFormatMemoryEnc(cur, mem, size, NULL, format);
}

/**
 * xmlDocDumpMemoryEnc:
 * @param out_doc Document to generate XML text from
 * @param doc_txt_ptr Memory pointer for allocated XML text
 * @param doc_txt_len Length of the generated XML text
 * @param txt_encoding Character encoding to use when generating XML text
 *
 * Dump the current DOM tree into memory using the character encoding specified
 * by the caller.  Note it is up to the caller of this function to free the
 * allocated memory with xmlFree().
 */

XMLPUBFUNEXPORT void
xmlDocDumpMemoryEnc(xmlDocPtr out_doc, xmlChar **doc_txt_ptr,
                int * doc_txt_len, const char * txt_encoding) {
    xmlDocDumpFormatMemoryEnc(out_doc, doc_txt_ptr, doc_txt_len,
                          txt_encoding, 0);
}

/**
 * xmlDocFormatDump:
 * @param f the FILE*
 * @param cur the document
 * @param format should formatting spaces been added
 *
 * Dump an XML document to an open FILE.
 *
 * returns: the number of bytes written or -1 in case of failure.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
int
xmlDocFormatDump(FILE *f, xmlDocPtr cur, int format) {
    xmlSaveCtxt ctxt;
    xmlOutputBufferPtr buf;
    const char * encoding;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlDocDump : document == NULL\n");
#endif
    return(-1);
    }
    encoding = (const char *) cur->encoding;

    if (encoding != NULL) {
        handler = xmlFindCharEncodingHandler(encoding);
        if (handler == NULL) {
        xmlFree((char *) cur->encoding);
        cur->encoding = NULL;
        }
    }
    buf = (xmlOutputBufferPtr)xmlOutputBufferCreateFile(f, handler);

    if (buf == NULL)
    {   // OOM!!!
        return(-1);
    }
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = cur;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = format;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    xmlDocContentDumpOutput(&ctxt, cur);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * xmlDocDump:
 * @param f the FILE*
 * @param cur the document
 *
 * Dump an XML document to an open FILE.
 *
 * @return the number of bytes written or -1 in case of failure
 *          (OOM, I/O error or invalid arguments)
 */
int
xmlDocDump(FILE *f, xmlDocPtr cur) {
    return(xmlDocFormatDump (f, cur, 0));
}

/**
 * xmlSaveFileTo:
 * @param buf an output I/O buffer
 * @param cur the document
 * @param encoding the encoding if any assuming the I/O layer handles the trancoding
 *
 * Dump an XML document to an I/O buffer.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
XMLPUBFUNEXPORT int
xmlSaveFileTo(xmlOutputBufferPtr buf, xmlDocPtr cur, const char *encoding) {
    xmlSaveCtxt ctxt;
    int ret;

    if (buf == NULL) return(0);
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = cur;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = 0;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    xmlDocContentDumpOutput(&ctxt, cur);
    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * xmlSaveFormatFileTo:
 * @param buf an output I/O buffer
 * @param cur the document
 * @param encoding the encoding if any assuming the I/O layer handles the trancoding
 * @param format should formatting spaces been added
 *
 * Dump an XML document to an I/O buffer.
 * NOTE: the I/O buffer is closed as part of the call.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
XMLPUBFUNEXPORT int
xmlSaveFormatFileTo(xmlOutputBufferPtr buf, xmlDocPtr cur,
                    const char *encoding, int format)
{
    xmlSaveCtxt ctxt;
    int ret;

    if (buf == NULL)
        return (0);
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = cur;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = format;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    xmlDocContentDumpOutput(&ctxt, cur);
    ret = xmlOutputBufferClose(buf);
    return (ret);
}

/**
 * xmlSaveFormatFileEnc:
 * @param filename the filename or URL to output
 * @param cur the document being saved
 * @param encoding the name of the encoding to use or NULL.
 * @param format should formatting spaces be added.
 *
 * Dump an XML document to a file or an URL.
 *
 * Returns the number of bytes written or -1 in case of error.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
XMLPUBFUNEXPORT int
xmlSaveFormatFileEnc( const char* filename, xmlDocPtr cur,
            const char* encoding, int format ) {
    xmlSaveCtxt ctxt;
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (!cur)
        return(-1);

    if (!encoding)
        encoding = (const char*) cur->encoding;

    if (encoding) {
        handler = xmlFindCharEncodingHandler(encoding);
        if (!handler)
            return(-1);
    }

#ifdef HAVE_ZLIB_H
    if (cur->compression < 0) cur->compression = xmlGetCompressMode();
#endif
    /*
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFilename(filename, handler, cur->compression);
    if (!buf)
        return(-1); 
    memset(&ctxt, 0, sizeof(ctxt));

    ctxt.doc = cur;
    ctxt.buf = buf;
    //ctxt.level = 0;  // already zeroed by memset
    ctxt.format = format;
    ctxt.encoding = (const xmlChar*) encoding;
    xmlSaveCtxtInit(&ctxt);
    xmlDocContentDumpOutput(&ctxt, cur);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}


/**
 * xmlSaveFileEnc:
 * @param filename the filename (or URL)
 * @param cur the document
 * @param encoding the name of an encoding (or NULL)
 *
 * Dump an XML document, converting it to the given encoding
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
XMLPUBFUNEXPORT int
xmlSaveFileEnc(const char *filename, xmlDocPtr cur, const char *encoding) {
    return ( xmlSaveFormatFileEnc( filename, cur, encoding, 0 ) );
}

/**
 * xmlSaveFormatFile:
 * @param filename the filename (or URL)
 * @param cur the document
 * @param format should formatting spaces been added
 *
 * Dump an XML document to a file. Will use compression if
 * compiled in and enabled. If filename is "-" the stdout file is
 * used. If format is set then the document will be indented on output.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
XMLPUBFUNEXPORT int
xmlSaveFormatFile(const char *filename, xmlDocPtr cur, int format) {
    return ( xmlSaveFormatFileEnc( filename, cur, NULL, format ) );
}

/**
 * xmlSaveFile:
 * @param filename the filename (or URL)
 * @param cur the document
 *
 * Dump an XML document to a file. Will use compression if
 * compiled in and enabled. If filename is "-" the stdout file is
 * used.
 * returns: the number of bytes written or -1 in case of failure.
 */
XMLPUBFUNEXPORT int
xmlSaveFile(const char *filename, xmlDocPtr cur) {
    return(xmlSaveFormatFileEnc(filename, cur, NULL, 0));
}

#endif /* LIBXML_OUTPUT_ENABLED */
