/*
 * libxml2_error.c: module displaying/handling XML parser errors
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#include <string.h>
#include <stdarg.h>

#include <libxml2_globals.h>
#include "libxml2_xmlerror2.h"

void xmlGenericErrorDefaultFunc (void *ctx ATTRIBUTE_UNUSED,
                 const char *msg,
                 ...);

#define XML_GET_VAR_STR(msg, str)                   \
{                                                   \
    int       size;                                 \
    int       chars;                                \
    char      *larger;                              \
    va_list   ap;                                   \
                                                    \
    str = (char *) xmlMalloc(150);                  \
    if (str != NULL)                                \
    {                                               \
        size = 150;                                 \
                                                    \
        while (1)                                   \
        {                                           \
            va_start(ap, msg);                      \
            chars = vsnprintf(str, size, msg, ap);  \
            va_end(ap);                             \
            if ((chars > -1) && (chars < size))     \
                break;                              \
                                                    \
            size += (chars > -1) ? chars + 1 : 100; \
            larger = (char*) xmlRealloc(str, size); \
            if (!larger)                            \
                break;                              \
                                                    \
            str = larger;                           \
        }                                           \
    }                                               \
}


/************************************************************************
 *                                                                      *
 *          Handling of out of context errors                           *
 *                                                                      *
 ************************************************************************/

/**
 * xmlGenericErrorDefaultFunc:
 * @param ctx an error context
 * @param msg the message to display/transmit
 * @param # extra parameters for the message display
 *
 * Default handler for out of context error messages.
 */
void
xmlGenericErrorDefaultFunc(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...) {
    
    return;
}

/**
 * initGenericErrorDefaultFunc:
 * @param handler the handler
 *
 * Set or reset (if NULL) the default handler for generic errors
 * to the builtin error function.
 */
XMLPUBFUNEXPORT void
initGenericErrorDefaultFunc(xmlGenericErrorFunc * handler)
{
    xmlGenericError = handler ? (*handler) : xmlGenericErrorDefaultFunc;
}

/**
 * xmlSetGenericErrorFunc:
 * @param ctx the new error handling context
 * @param handler the new handler function
 *
 * Function to reset the handler and the error context for out of
 * context error messages.
 * This simply means that handler will be called for subsequent
 * error messages while not parsing nor validating. And ctx will
 * be passed as first argument to handler
 * One can simply force messages to be emitted to another FILE * than
 * stderr by setting ctx to this file handle and handler to NULL.
 */
XMLPUBFUNEXPORT void
xmlSetGenericErrorFunc(void *ctx, xmlGenericErrorFunc handler) {
    xmlGenericErrorContext = ctx;
    xmlGenericError = handler ? handler : xmlGenericErrorDefaultFunc;
}

/**
 * xmlSetStructuredErrorFunc:
 * @param ctx the new error handling context
 * @param handler the new handler function
 *
 * Function to reset the handler and the error context for out of
 * context structured error messages.
 * This simply means that handler will be called for subsequent
 * error messages while not parsing nor validating. And ctx will
 * be passed as first argument to handler
 */
XMLPUBFUNEXPORT void
xmlSetStructuredErrorFunc(void *ctx, xmlStructuredErrorFunc handler) {
	LOAD_GS_DIRECT
    xmlGenericErrorContext = ctx;
    xmlStructuredError = handler;
}

/************************************************************************
 *                                                                      *
 *          Handling of parsing errors                                  *
 *                                                                      *
 ************************************************************************/

/**
 * xmlParserPrintFileInfo:
 * @param input an xmlParserInputPtr input
 *
 * Displays the associated file and line informations for the current input
 */

XMLPUBFUNEXPORT void
xmlParserPrintFileInfo(xmlParserInputPtr input) {
    if (input) {
        xmlGenericError(
            xmlGenericErrorContext,
            "%s: line %d: ",
            (input->filename ? input->filename : "Entity [no file name]"),
            input->line);
    }
}

/**
 * xmlParserPrintFileContext:
 * @param input an xmlParserInputPtr input
 *
 * Displays current context within the input content for error tracking
 */

static void
xmlParserPrintFileContextInternal(xmlParserInputPtr input ,
        xmlGenericErrorFunc channel, void *data ) {
    const xmlChar *cur, *base;
    unsigned int n, col;    /* GCC warns if signed, because compared with sizeof() */
    xmlChar  content[81]; /* space for 80 chars + line terminator */
    xmlChar *ctnt;

    if (input == NULL) return;
    cur = input->cur;
    base = input->base;
    /* skip backwards over any end-of-lines */
    while ((cur > base) && ((*(cur) == '\n') || (*(cur) == '\r'))) {
    cur--;
    }
    n = 0;
    /* search backwards for beginning-of-line (to max buff size) */
    while ((n++ < (sizeof(content)-1)) && (cur > base) &&
           (*(cur) != '\n') && (*(cur) != '\r'))
        cur--;
    if ((*(cur) == '\n') || (*(cur) == '\r')) cur++;
    /* calculate the error position in terms of the current position */
    col = input->cur - cur;
    /* search forward for end-of-line (to max buff size) */
    n = 0;
    ctnt = content;
    /* copy selected text to our buffer */
    while ((*cur != 0) && (*(cur) != '\n') &&
           (*(cur) != '\r') && (n < sizeof(content)-1)) {
        *ctnt++ = *cur++;
    n++;
    }
    *ctnt = 0;
    /* print out the selected text */
    channel(data ,"%s\n", content);
    /* create blank line with problem pointer */
    n = 0;
    ctnt = content;
    /* (leave buffer space for pointer + line terminator) */
    while ((n<col) && (n++ < sizeof(content)-2) && (*ctnt != 0)) {
    if (*(ctnt) != '\t')
        *(ctnt) = ' ';
    ctnt++;
    }
    *ctnt++ = '^';
    *ctnt = 0;
    channel(data ,"%s\n", content);
}

/**
 * xmlParserPrintFileContext:
 * @param input an xmlParserInputPtr input
 *
 * Displays current context within the input content for error tracking
 */
XMLPUBFUNEXPORT void
xmlParserPrintFileContext(xmlParserInputPtr input) {
   xmlParserPrintFileContextInternal(input, xmlGenericError,
                                     xmlGenericErrorContext);
}


/**
 * xmlReportError:
 * @param err the error
 * @param ctx the parser context or NULL
 * @param str the formatted error message
 *
 * Report an erro with its context, replace the 4 old error/warning
 * routines.
 */
static void
xmlReportError(xmlErrorPtr err, xmlParserCtxtPtr ctxt, const char *str,
               xmlGenericErrorFunc channel, void *data)
{
    char *file = NULL;
    int line = 0;
    int code = -1;
    int domain;
    const xmlChar *name = NULL;
    xmlNodePtr node;
    xmlErrorLevel level;
    xmlParserInputPtr input = NULL;
    xmlParserInputPtr cur = NULL;

    if (err == NULL)
        return;

    if (channel == NULL) { 
        channel = xmlGenericError;
        data = xmlGenericErrorContext;
    }
    file = err->file;
#ifdef LIBXML_ENABLE_NODE_LINEINFO
    line = err->line;
#endif
    code = err->code;
    domain = err->domain;
    level = err->level;
    node = (xmlNodePtr)err->node;

    if (code == XML_ERR_OK)
        return;

    if ((node != NULL) && (node->type == XML_ELEMENT_NODE))
        name = node->name;

    /*
     * Maintain the compatibility with the legacy error handling
     */
    if (ctxt != NULL) {
        input = ctxt->input;
        if ((input != NULL) && (input->filename == NULL) &&
            (ctxt->inputNr > 1)) {
            cur = input;
            input = ctxt->inputTab[ctxt->inputNr - 2];
        }
        if (input != NULL) {
            if (input->filename)
                channel(data, "%s:%d: ", input->filename, input->line);
            else if ((line != 0) && (domain == XML_FROM_PARSER))
                channel(data, "Entity: line %d: ", input->line);
        }
    } else {
        if (file != NULL)
            channel(data, "%s:%d: ", file, line);
        else if ((line != 0) && (domain == XML_FROM_PARSER))
            channel(data, "Entity: line %d: ", line);
    }
    if (name != NULL) {
        channel(data, "element %s: ", name);
    }
    if (code == XML_ERR_OK)
        return;
    switch (domain) {
        case XML_FROM_PARSER:
            channel(data, "parser ");
            break;
        case XML_FROM_NAMESPACE:
            channel(data, "namespace ");
            break;
        case XML_FROM_DTD:
        case XML_FROM_VALID:
            channel(data, "validity ");
            break;
        case XML_FROM_HTML:
            channel(data, "HTML parser ");
            break;
        case XML_FROM_MEMORY:
            channel(data, "memory ");
            break;
        case XML_FROM_OUTPUT:
            channel(data, "output ");
            break;
        case XML_FROM_IO:
            channel(data, "I/O ");
            break;
        case XML_FROM_XINCLUDE:
            channel(data, "XInclude ");
            break;
        case XML_FROM_XPATH:
            channel(data, "XPath ");
            break;
        case XML_FROM_XPOINTER:
            channel(data, "parser ");
            break;
        case XML_FROM_REGEXP:
            channel(data, "regexp ");
            break;
        case XML_FROM_SCHEMASV:
            channel(data, "Schemas validity ");
            break;
        case XML_FROM_SCHEMASP:
            channel(data, "Schemas parser ");
            break;
        case XML_FROM_RELAXNGP:
            channel(data, "Relax-NG parser ");
            break;
        case XML_FROM_RELAXNGV:
            channel(data, "Relax-NG validity ");
            break;
        case XML_FROM_CATALOG:
            channel(data, "Catalog ");
            break;
        case XML_FROM_C14N:
            channel(data, "C14N ");
            break;
        case XML_FROM_XSLT:
            channel(data, "XSLT ");
            break;
        default:
            break;
    }
    if (code == XML_ERR_OK)
        return;
    switch (level) {
        case XML_ERR_NONE:
            channel(data, ": ");
            break;
        case XML_ERR_WARNING:
            channel(data, "warning : ");
            break;
        case XML_ERR_ERROR:
            channel(data, "error : ");
            break;
        case XML_ERR_FATAL:
            channel(data, "error : ");
            break;
    }
    if (code == XML_ERR_OK)
        return;
    if (str != NULL) {
        int len;
        len = xmlStrlen((const xmlChar *)str);
        if ((len > 0) && (str[len - 1] != '\n'))
            channel(data, "%s\n", str);
        else
            channel(data, "%s", str);
    } else {
        channel(data, "%s\n", "out of memory error");
    }
    if (code == XML_ERR_OK)
        return;

    if (ctxt != NULL) {
        xmlParserPrintFileContextInternal(input, channel, data);
        if (cur != NULL) {
            if (cur->filename)
                channel(data, "%s:%d: \n", cur->filename, cur->line);
            else if ((line != 0) && (domain == XML_FROM_PARSER))
                channel(data, "Entity: line %d: \n", cur->line);
            xmlParserPrintFileContextInternal(cur, channel, data);
        }
    }
    if ((domain == XML_FROM_XPATH) &&
        (err->str1 != NULL) &&
        (err->int1 < 100) &&
        (err->int1 < xmlStrlen((const xmlChar *)err->str1)))
    {
        
        xmlChar buf[150];
        int i;

        channel(data, "%s\n", err->str1);
        for (i=0;i < err->int1;i++)
             buf[i] = ' ';
        buf[i++] = '^';
        buf[i] = 0;
        channel(data, "%s\n", buf);
    }
}

/**
 * __xmlRaiseError:
 * @param schannel the structured callback channel
 * @param channel the old callback channel
 * @param data the callback data
 * @param ctx the parser context or NULL
 * @param ctx the parser context or NULL
 * @param domain the domain for the error
 * @param code the code for the error
 * @param level the xmlErrorLevel for the error
 * @param file the file source of the error (or NULL)
 * @param line the line of the error or 0 if N/A
 * @param str1 extra string info
 * @param str2 extra string info
 * @param str3 extra string info
 * @param int1 extra int info
 * @param int2 extra int info
 * @param msg the message to display/transmit
 * @param # extra parameters for the message display
 *
 * Update the appropriate global or contextual error structure,
 * then forward the error message down the parser or generic
 * error callback handler
 */
void
__xmlRaiseError(xmlStructuredErrorFunc schannel,
              xmlGenericErrorFunc channel, void *data, void *ctx,
              void *nod, int domain, int code, xmlErrorLevel level,
              const char *file, int line, const char *str1,
              const char *str2, const char *str3, int int1, int int2,
          const char *msg, ...)
{
	LOAD_GS_DIRECT
	xmlParserCtxtPtr ctxt = NULL;
    xmlNodePtr node = (xmlNodePtr) nod;
    char *str = NULL;
    xmlParserInputPtr input = NULL;
    xmlErrorPtr to = &xmlLastError;
    xmlChar *base = NULL;
    int wasOOM = (code == XML_ERR_NO_MEMORY);
    
    // Check, whether we must ignore warnings
    if (!xmlGetWarningsDefaultValue && (level == XML_ERR_WARNING))
        return;

    if ((domain == XML_FROM_PARSER)    ||
        (domain == XML_FROM_HTML)      ||
        (domain == XML_FROM_DTD)       ||
        (domain == XML_FROM_NAMESPACE) ||
        (domain == XML_FROM_IO))
    {
        ctxt = (xmlParserCtxtPtr) ctx;
        if ((schannel == NULL) && (ctxt != NULL) &&
            (ctxt->sax != NULL)&& (ctxt->sax->initialized == XML_SAX2_MAGIC))
               schannel = ctxt->sax->serror;
    }
    if (schannel == NULL)
        schannel = xmlStructuredError;
    if ((domain == XML_FROM_VALID) &&
        ((channel == xmlParserValidityError) || (channel == xmlParserValidityWarning)))
    {
        ctxt = (xmlParserCtxtPtr) ctx;
        if ((schannel == NULL) &&
            (ctxt != NULL) &&
            (ctxt->sax != NULL) &&
            (ctxt->sax->initialized == XML_SAX2_MAGIC))
        {
            schannel = ctxt->sax->serror;
        }
    }
    if (code == XML_ERR_OK)
        return;

    /*
     * Formatting the message
     */
#ifndef XMLENGINE_EXCLUDE_EMBED_MSG
    if (msg == NULL || wasOOM) {
#else
    if (msg == NULL || msg == __embedded_errtxt_replacement || wasOOM) {
#endif
        // DONE: do not copy if it was OOM
        str =  (code == XML_ERR_NO_MEMORY) ? NULL :  (char *) xmlStrdup(BAD_CAST "No error message provided");
        str1 = str2 = str3 = NULL;
    } else {
        //XML_GET_VAR_STR(msg, str); //this causes panic when OOM
        str = (char *) xmlStrdup(BAD_CAST msg); // NOTE: message is not filled with data from variable list
    }
    /*
     * specific processing if a parser context is provided
     */
    if (ctxt != NULL) {
        if (file == NULL) {
            input = ctxt->input;
            if ((input != NULL) &&
                (input->filename == NULL) &&
                (ctxt->inputNr > 1))
            {
                input = ctxt->inputTab[ctxt->inputNr - 2];
            }
            if (input != NULL) {
                file = input->filename;
                line = input->line;
            }
        }
        to = &ctxt->lastError;
    } else if ((node != NULL) && (file == NULL)) {

        if(!wasOOM)
        {
            int i;

            if ((node->doc != NULL) && (node->doc->URL != NULL))
                base = xmlStrdup(node->doc->URL);
            for (i = 0;
                ((i < 10) && (node != NULL) && (node->type != XML_ELEMENT_NODE));
                i++)
             {
                node = node->parent;
             }
            if (!base  &&  node  &&  node->doc  &&  node->doc->URL)
            {
                base = xmlStrdup(node->doc->URL);
            }
        }
#ifdef LIBXML_ENABLE_NODE_LINEINFO
    if ((node != NULL) && (node->type == XML_ELEMENT_NODE))
        line = node->line;
#endif
    }
    /*
     * Save the information about the error
     */
    xmlResetError(to);
    to->domain = domain;
    to->code = code;
    to->message = str;
    to->level = level;
    if (file && !wasOOM)
        to->file = (char *) xmlStrdup((const xmlChar *) file);
    else if (base != NULL) {
        to->file = (char *) base;
        file = (char *) base;
    }

#ifdef LIBXML_ENABLE_NODE_LINEINFO
    to->line = line;
#endif

    if(!wasOOM)
    {
    if (str1)
        to->str1 = (char *) xmlStrdup((const xmlChar *) str1);
    if (str2)
        to->str2 = (char *) xmlStrdup((const xmlChar *) str2);
    if (str3)
        to->str3 = (char *) xmlStrdup((const xmlChar *) str3);
    }
    to->int1 = int1;
    to->int2 = int2;
    to->node = node;
    to->ctxt = ctx;

    if (to != &xmlLastError) {
        xmlResetError(&xmlLastError);
        xmlCopyError(to,&xmlLastError); // NOTE: xmlCopyError checks  error code whether it is OOM
    }

    /*
     * Find the callback channel.
     */
    if ((ctxt != NULL) &&
        (channel == NULL) &&
        (xmlStructuredError == NULL))
    {
        if (level == XML_ERR_WARNING)
            channel = ctxt->sax->warning;
        else
            channel = ctxt->sax->error;
        data = ctxt->userData;
    } else {
        if (channel == NULL) {
            if (xmlStructuredError != NULL)
                schannel = xmlStructuredError;
            else
                channel = xmlGenericError;
            data = xmlGenericErrorContext;
        }
    }
// <---   At this point we have error structure "to" ready
//        Implement Symbian logging if there is need to log errors (apply filtering by error codes and severity)
//
    if (schannel != NULL) {
        schannel(data, to);
        return;
    }
    if (channel == NULL)
        return;

    if ((channel == xmlParserError) ||
        (channel == xmlParserWarning) ||
        (channel == xmlParserValidityError) ||
        (channel == xmlParserValidityWarning))
    {
        xmlReportError(to, ctxt, str, NULL, NULL);
    }
    else
    {
        if ((channel == (xmlGenericErrorFunc) fprintf) ||
            (channel == xmlGenericErrorDefaultFunc))
            xmlReportError(to, ctxt, str, channel, data);
        else
            channel(data, "%s", str);
    }
}

/**
 * __xmlSimpleError:
 * @param domain where the error comes from
 * @param code the error code
 * @param node the context node
 * @param extra extra informations
 *
 * Handle an out of memory condition
 */
void
__xmlSimpleError(int domain, int code, xmlNodePtr node,
                 const char *msg, const char *extra)
{
    // DONE: OPTIMIZE: Combine all 3 calls into one
/*
    if (code == XML_ERR_NO_MEMORY) {
        if (extra)
            __xmlRaiseError(NULL, NULL, NULL, NULL, node, domain,
                    XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
                    NULL, NULL, 0, 0,
                    EMBED_ERRTXT("Memory allocation failed : %s\n"), extra);
        else
            __xmlRaiseError(NULL, NULL, NULL, NULL, node, domain,
                    XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, NULL,
                    NULL, NULL, 0, 0, EMBED_ERRTXT("Memory allocation failed\n"));
    } else {
        __xmlRaiseError(NULL, NULL, NULL, NULL, node, domain,
                code, XML_ERR_ERROR, NULL, 0, extra,
                NULL, NULL, 0, 0, msg, extra);
    }
*/
    if (code == XML_ERR_NO_MEMORY)
    {
        msg = EMBED_ERRTXT("Memory allocation failed : %s\n");
        if (!extra)
            extra = EMBED_ERRTXT("[location unspecified]");
    }
    __xmlRaiseError(NULL, NULL, NULL, NULL, node, domain,
            code, XML_ERR_ERROR, NULL, 0, extra,
            NULL, NULL, 0, 0, msg, extra);
}

/**
 * xmlParserError:
 * @param ctx an XML parser context
 * @param msg the message to display/transmit
 * @param # extra parameters for the message display
 *
 * Display and format an error messages, gives file, line, position and
 * extra parameters.
 */
XMLPUBFUNEXPORT void
xmlParserError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    xmlParserInputPtr cur = NULL;
	LOAD_GS_SAFE_CTXT(ctxt)

    if (ctxt) {
        input = ctxt->input;
        if (input && !input->filename && (ctxt->inputNr > 1))
        {
            cur = input;
            input = ctxt->inputTab[ctxt->inputNr - 2];
        }
        xmlParserPrintFileInfo(input);
    }

	if(OOM_FLAG){
        xmlGenericError(xmlGenericErrorContext, "error: [in OOM!] %s", msg);
    } else {
        char* str;
        xmlGenericError(xmlGenericErrorContext, "error: ");
        XML_GET_VAR_STR(msg, str);
        xmlGenericError(xmlGenericErrorContext, "%s", str);
        if (str)
            xmlFree(str);
    }

    if (ctxt) {
        xmlParserPrintFileContext(input);
        if (cur) {
            xmlParserPrintFileInfo(cur);
            xmlGenericError(xmlGenericErrorContext, "\n");
            xmlParserPrintFileContext(cur);
        }
    }
}

/**
 * xmlParserWarning:
 * @param ctx an XML parser context
 * @param msg the message to display/transmit
 * @param # extra parameters for the message display
 *
 * Display and format a warning messages, gives file, line, position and
 * extra parameters.
 */
XMLPUBFUNEXPORT void
xmlParserWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    xmlParserInputPtr cur = NULL;
	LOAD_GS_SAFE_CTXT(ctxt)

    if (ctxt != NULL) {
        input = ctxt->input;
        if (input && !input->filename && (ctxt->inputNr > 1)) {
            cur = input;
            input = ctxt->inputTab[ctxt->inputNr - 2];
        }
        xmlParserPrintFileInfo(input);
    }

    if(OOM_FLAG){
        xmlGenericError(xmlGenericErrorContext, "warning: [in OOM!] %s", msg);
    } else {
        char* str;
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("warning: "));
        XML_GET_VAR_STR(msg, str);
        xmlGenericError(xmlGenericErrorContext, "%s", str);
        if (str != NULL)
            xmlFree(str);
    }

    if (ctxt) {
        xmlParserPrintFileContext(input);
        if (cur) {
            xmlParserPrintFileInfo(cur);
            xmlGenericError(xmlGenericErrorContext, "\n");
            xmlParserPrintFileContext(cur);
        }
    }
}

/************************************************************************
 *                                                                      *
 *          Handling of validation errors                               *
 *                                                                      *
 ************************************************************************/

/**
 * xmlParserValidityError:
 * @param ctx an XML parser context
 * @param msg the message to display/transmit
 * @param # extra parameters for the message display
 *
 * Display and format an validity error messages, gives file,
 * line, position and extra parameters.
 */
XMLPUBFUNEXPORT void
xmlParserValidityError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;

    int len = xmlStrlen((const xmlChar *) msg);
	LOAD_GS_SAFE_CTXT(ctxt)

    if ((len > 1) && (msg[len - 2] != ':')) {
        if (ctxt != NULL) {
            input = ctxt->input;
            if ((input->filename == NULL) && (ctxt->inputNr > 1))
                input = ctxt->inputTab[ctxt->inputNr - 2];

            if (had_info == 0) {
                xmlParserPrintFileInfo(input);
            }
        }
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("validity error: "));
        had_info = 0;
    } else {
        had_info = 1;
    }

    if(OOM_FLAG){
        xmlGenericError(xmlGenericErrorContext, "validity error: [in OOM!] %s", msg);
    } else {
        char* str;
        XML_GET_VAR_STR(msg, str);
        xmlGenericError(xmlGenericErrorContext, "%s", str);
        if (str)
            xmlFree(str);
    }

    if (ctxt && input) {
        xmlParserPrintFileContext(input);
    }
}

/**
 * xmlParserValidityWarning:
 * @param ctx an XML parser context
 * @param msg the message to display/transmit
 * @param # extra parameters for the message display
 *
 * Display and format a validity warning messages, gives file, line,
 * position and extra parameters.
 */
XMLPUBFUNEXPORT void
xmlParserValidityWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    int len = xmlStrlen((const xmlChar *) msg);
	LOAD_GS_SAFE_CTXT(ctxt)

    if (ctxt  && (len != 0) && (msg[len - 1] != ':'))
    {
        input = ctxt->input;
        if ((input->filename == NULL) && (ctxt->inputNr > 1))
            input = ctxt->inputTab[ctxt->inputNr - 2];

        xmlParserPrintFileInfo(input);
    }

    if(OOM_FLAG){
        xmlGenericError(xmlGenericErrorContext, "validity warning: [in OOM!] %s", msg);
    } else {
        char* str;
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("warning: "));
        XML_GET_VAR_STR(msg, str);
        xmlGenericError(xmlGenericErrorContext, "%s", str);
        if (str)
            xmlFree(str);
    }

    if (ctxt) {
        xmlParserPrintFileContext(input);
    }
}


/************************************************************************
 *                                                                      *
 *          Extended Error Handling                                     *
 *                                                                      *
 ************************************************************************/

/**
 * xmlGetLastError:
 *
 * Get the last global error registered. This is per thread if compiled
 * with thread support.
 *
 * Returns NULL if no error occured or a pointer to the error
 */
XMLPUBFUNEXPORT xmlErrorPtr
xmlGetLastError(void)
{
	LOAD_GS_DIRECT
    if (xmlLastError.code == XML_ERR_OK)
        return (NULL);
    return (&xmlLastError);
}

/**
 * xmlResetError:
 * @param err pointer to the error.
 *
 * Cleanup the error.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlResetError(xmlErrorPtr err)
{
    if (err == NULL)
        return;
    if (err->code == XML_ERR_OK)
        return;
    if (err->message != NULL)
        xmlFree(err->message);
    if (err->file != NULL)
        xmlFree(err->file);
    if (err->str1 != NULL)
        xmlFree(err->str1);
    if (err->str2 != NULL)
        xmlFree(err->str2);
    if (err->str3 != NULL)
        xmlFree(err->str3);
    memset(err, 0, sizeof(xmlError));
    err->code = XML_ERR_OK;
}

/**
 * xmlResetLastError:
 *
 * Cleanup the last global error registered. For parsing error
 * this does not change the well-formedness result.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlResetLastError(void)
{
	LOAD_GS_DIRECT
    if (xmlLastError.code == XML_ERR_OK)
        return;
    xmlResetError(&xmlLastError);
}

/**
 * xmlCtxtGetLastError:
 * @param ctx an XML parser context
 *
 * Get the last parsing error registered.
 *
 * Returns NULL if no error occured or a pointer to the error
 */
XMLPUBFUNEXPORT xmlErrorPtr
xmlCtxtGetLastError(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;

    if (ctxt == NULL)
        return (NULL);
    if (ctxt->lastError.code == XML_ERR_OK)
        return (NULL);
    return (&ctxt->lastError);
}

/**
 * xmlCtxtResetLastError:
 * @param ctx an XML parser context
 *
 * Cleanup the last global error registered. For parsing error
 * this does not change the well-formedness result.
 */
XMLPUBFUNEXPORT void
xmlCtxtResetLastError(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;

    if (ctxt == NULL)
        return;
    if (ctxt->lastError.code == XML_ERR_OK)
        return;
    xmlResetError(&ctxt->lastError);
}

/**
 * xmlCopyError:
 * @param from a source error
 * @param to a target error
 *
 * Save the original error to the new place.
 *
 * Returns 0 in case of success and -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlCopyError(xmlErrorPtr from, xmlErrorPtr to) {
    if ((from == NULL) || (to == NULL))
        return(-1);
    if (to->message != NULL)
        xmlFree(to->message);
    if (to->file != NULL)
        xmlFree(to->file);
    if (to->str1 != NULL)
        xmlFree(to->str1);
    if (to->str2 != NULL)
        xmlFree(to->str2);
    if (to->str3 != NULL)
        xmlFree(to->str3);
    to->domain = from->domain;
    to->code = from->code;
    to->level = from->level;
#ifdef LIBXML_ENABLE_NODE_LINEINFO
    to->line = from->line;
#endif
    to->node = from->node;
    to->int1 = from->int1;
    to->int2 = from->int2;
    to->node = from->node;
    to->ctxt = from->ctxt;

    // Do not try to allocate memory during OOM handling!
    if(from->code != XML_ERR_NO_MEMORY)
    {
    if (from->message != NULL)
        to->message = (char *) xmlStrdup((xmlChar *) from->message);
    else
        to->message = NULL;
    if (from->file != NULL)
        to->file = (char *) xmlStrdup((xmlChar *) from->file);
    else
        to->file = NULL;
    if (from->str1 != NULL)
        to->str1 = (char *) xmlStrdup((xmlChar *) from->str1);
    else
        to->str1 = NULL;
    if (from->str2 != NULL)
        to->str2 = (char *) xmlStrdup((xmlChar *) from->str2);
    else
        to->str2 = NULL;
    if (from->str3 != NULL)
        to->str3 = (char *) xmlStrdup((xmlChar *) from->str3);
    else
        to->str3 = NULL;
    }
    return(0);
}


