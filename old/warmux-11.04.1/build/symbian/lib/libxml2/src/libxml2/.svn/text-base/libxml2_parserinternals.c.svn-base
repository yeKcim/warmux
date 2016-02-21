/*
 * libxml2_parserinternals.c : Internal routines (and obsolete ones) needed for the
 *                             XML and HTML parsers.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#define UNDEF_IMPORT_C_IN_DATA_ParserInternal
#include "xmlenglibxml.h"

#if defined(__SYMBIAN32__) || defined(WIN32) && !defined (__CYGWIN__)
#define XML_DIR_SEP '\\'
#else
#define XML_DIR_SEP '/'
#endif

#include <string.h>
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include <libxml2_globals.h>
#include <libxml2_parserinternals.h>
#include "libxml2_errencoding.h"
#include "libxml2_xmlerror2.h"
#include <libxml2_uri.h>

#ifdef LIBXML_CATALOG_ENABLED
#include "libxml2_catalog.h"
#endif


/*
 * Various global defaults for parsing
 */

/**
 * xmlCheckVersion:
 * @param version the include version number
 *
 * check the compiled lib version against the include one.
 * This can warn or immediately kill the application
 */
XMLPUBFUNEXPORT void
xmlCheckVersion(int version) {
    int myversion = (int) LIBXML_VERSION;

    xmlInitParser();

    if ((myversion / 10000) != (version / 10000)) {
    xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("Fatal: program compiled against libxml %d using libxml %d\n"),
        (version / 10000), (myversion / 10000));
    fprintf(stderr,
        EMBED_ERRTXT("Fatal: program compiled against libxml %d using libxml %d\n"),
        (version / 10000), (myversion / 10000));
    }
    if ((myversion / 100) < (version / 100)) {
    xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("Warning: program compiled against libxml %d using older %d\n"),
        (version / 100), (myversion / 100));
    }
}


XMLPUBFUNEXPORT const xmlChar xmlStringText[]      = { 't', 'e', 'x', 't', 0 };
XMLPUBFUNEXPORT const xmlChar xmlStringTextNoenc[] = { 't', 'e', 'x', 't', 'n', 'o', 'e', 'n', 'c', 0 };
XMLPUBFUNEXPORT const xmlChar xmlStringComment[]   = { 'c', 'o', 'm', 'm', 'e', 'n', 't', 0 };
/************************************************************************
 *                                                                      *
 *      Some factorized error routines                                  *
 *                                                                      *
 ************************************************************************/


/**
 * xmlErrMemory:
 * @param ctxt an XML parser context
 * @param extra extra informations
 *
 * Handle a redefinition of attribute error
 *
 * OOM: this function is called when OOM happens --> sets OOM flag
 */
void
xmlErrMemory(xmlParserCtxtPtr ctxt, const char *extra)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
        return;
    if (ctxt != NULL) {
        ctxt->errNo = XML_ERR_NO_MEMORY;
        ctxt->instate = XML_PARSER_EOF;
        ctxt->disableSAX = 1;
    }
    if (extra) 
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
                        NULL, NULL, 0, 0,
                        EMBED_ERRTXT("Memory allocation failed : %s\n"), extra);
    else
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, NULL,
                        NULL, NULL, 0, 0, EMBED_ERRTXT("Memory allocation failed\n"));
}

/**
 * __xmlErrEncoding:
 * @param ctxt an XML parser context
 * @param error the error number
 * @param msg the error message
 * @param str1 an string info
 * @param str2 an string info
 *
 * Handle an encoding error
 */
void
__xmlErrEncoding(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                 const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
    return;
    if (ctxt != NULL)
        ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, error, XML_ERR_FATAL,
                    NULL, 0, (const char *) str1, (const char *) str2,
                    NULL, 0, 0, msg, str1, str2);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}

/**
 * xmlErrInternal:
 * @param ctxt an XML parser context
 * @param msg the error message
 * @param str error informations
 *
 * Handle an internal error
 */
static void
xmlErrInternal(xmlParserCtxtPtr ctxt, const char *msg, const xmlChar * str)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
    return;
    if (ctxt != NULL)
        ctxt->errNo = XML_ERR_INTERNAL_ERROR;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, XML_ERR_INTERNAL_ERROR,
                    XML_ERR_FATAL, NULL, 0, (const char *) str, NULL, NULL,
                    0, 0, msg, str);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}

/**
 * xmlErrEncodingInt:
 * @param ctxt an XML parser context
 * @param error the error number
 * @param msg the error message
 * @param val an integer value
 *
 * n encoding error
 */
static void
xmlErrEncodingInt(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                  const char *msg, int val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
    return;
    if (ctxt != NULL)
        ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, error, XML_ERR_FATAL,
                    NULL, 0, NULL, NULL, NULL, val, 0, msg, val);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}

/**
 * xmlIsLetter:
 * @param c an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [84] Letter ::= BaseChar | Ideographic
 *
 * Returns 0 if not, non-zero otherwise
 */

XMLPUBFUNEXPORT int
xmlIsLetter(int c) {
    return(IS_BASECHAR(c) || IS_IDEOGRAPHIC(c));
}

/************************************************************************
 *                                                                      *
 *      Input handling functions for progressive parsing                *
 *                                                                      *
 ************************************************************************/

/* #define DEBUG_INPUT */
/* #define DEBUG_STACK */
/* #define DEBUG_PUSH */


/* we need to keep enough input to show errors in context */
#define LINE_LEN        80

#ifdef DEBUG_INPUT
#define CHECK_BUFFER(in) check_buffer(in)

/**
OOM: never
*/
static
void check_buffer(xmlParserInputPtr in) {
    if (in->base != in->buf->buffer->content) {
        xmlGenericError(xmlGenericErrorContext,EMBED_ERRTXT("xmlParserInput: base mismatch problem\n"));
    }
    if (in->cur < in->base) {
        xmlGenericError(xmlGenericErrorContext,EMBED_ERRTXT("xmlParserInput: cur < base problem\n"));
    }
    if (in->cur > in->base + in->buf->buffer->use) {
        xmlGenericError(xmlGenericErrorContext,EMBED_ERRTXT("xmlParserInput: cur > base + use problem\n"));
    }
    xmlGenericError(xmlGenericErrorContext,EMBED_ERRTXT("buffer %x : content %x, cur %d, use %d, size %d\n"),
            (int) in, (int) in->buf->buffer->content, in->cur - in->base,
        in->buf->buffer->use, in->buf->buffer->size);
}

#else
#define CHECK_BUFFER(in)
#endif


/**
 * xmlParserInputRead:
 * @param in an XML parser input
 * @param len an indicative size for the lookahead
 *
 * This function refresh the input for the parser. It doesn't try to
 * preserve pointers to the input buffer, and discard already read data
 *
 * Returns the number of xmlChars read, or -1 in case of error, 0 indicate the
 * end of this entity
 */
XMLPUBFUNEXPORT int
xmlParserInputRead(xmlParserInputPtr in, int len) {
    int ret;
    int used;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Read\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);
    if (in->buf->readcallback == NULL) return(-1);

    CHECK_BUFFER(in);

    used = in->cur - in->buf->buffer->content;
    ret = xmlBufferShrink(in->buf->buffer, used);
    if (ret > 0) {
        in->cur -= ret;
        in->consumed += ret;
    }
    ret = xmlParserInputBufferRead(in->buf, len);
    if (in->base != in->buf->buffer->content) {
        /*
         * the buffer has been reallocated
         */
        indx = in->cur - in->base;
        in->base = in->buf->buffer->content;
        in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);

    return(ret);
}

/**
 * xmlParserInputGrow:
 * @param in an XML parser input
 * @param len an indicative size for the lookahead
 *
 * This function increase the input for the parser. It tries to
 * preserve pointers to the input buffer, and keep already read data
 *
 * Returns the number of xmlChars read, or -1 in case of error, 0 indicate the
 * end of this entity
 *
 * OOM: possible --> OOM flag is set when returns -1
 */
XMLPUBFUNEXPORT int
xmlParserInputGrow(xmlParserInputPtr in, int len) {
	LOAD_GS_DIRECT
    int ret;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Grow\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);

    CHECK_BUFFER(in);

    indx = in->cur - in->base;
    if (in->buf->buffer->use > (unsigned int) indx + INPUT_CHUNK)
    {
        CHECK_BUFFER(in);
        return(0);
    }
    if (!in->buf->readcallback)
        return(0);

    ret = xmlParserInputBufferGrow(in->buf, len);
    if(OOM_FLAG)
        return -1;


    /*
     * NOTE : in->base may be a "dangling" i.e. freed pointer in this
     *        block, but we use it really as an integer to do some
     *        pointer arithmetic. 
     *             
     */
    if (in->base != in->buf->buffer->content) {
        /*
         * the buffer has been reallocated
         */
        indx = in->cur - in->base;
        in->base = in->buf->buffer->content;
        in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);

    return(ret);
}

/**
 * xmlParserInputShrink:
 * @param in an XML parser input
 *
 * This function removes used input for the parser.
 */
XMLPUBFUNEXPORT void
xmlParserInputShrink(xmlParserInputPtr in) {
    int used;
    int ret;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Shrink\n");
#endif
    if (in->buf == NULL) return;
    if (in->base == NULL) return;
    if (in->cur == NULL) return;
    if (in->buf->buffer == NULL) return;

    CHECK_BUFFER(in);

    used = in->cur - in->buf->buffer->content;
    /*
     * Do not shrink on large buffers whose only a tiny fraction
     * was consumed
     */
    if (used > INPUT_CHUNK) {
    ret = xmlBufferShrink(in->buf->buffer, used - LINE_LEN);
    if (ret > 0) {
        in->cur -= ret;
        in->consumed += ret;
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];
    }

    CHECK_BUFFER(in);

    if (in->buf->buffer->use > INPUT_CHUNK) {
        return;
    }
    xmlParserInputBufferRead(in->buf, 2 * INPUT_CHUNK);
    if (in->base != in->buf->buffer->content) {
        /*
     * the buffer has been reallocated
     */
    indx = in->cur - in->base;
    in->base = in->buf->buffer->content;
    in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);
}

/************************************************************************
 *                                                                      *
 *      UTF8 character input and related functions                      *
 *                                                                      *
 ************************************************************************/

/**
 * xmlNextChar:
 * @param ctxt the XML parser context
 *
 * Skip to the next char input char.
 *
 * OOM (REVIEW NOT FINISHED): possible --> check OOM flag
 */

XMLPUBFUNEXPORT void
xmlNextChar(xmlParserCtxtPtr ctxt)
{
	LOAD_GS(ctxt)
    if (ctxt->instate == XML_PARSER_EOF)
        return;

    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
        if ((*ctxt->input->cur == 0) &&
            (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0) &&  // May get into OOM
            (ctxt->instate != XML_PARSER_COMMENT))
        {
            /*
             * If we are at the end of the current entity and
             * the context allows it, we pop consumed entities
             * automatically.
             * the auto closing should be blocked in other cases
             */
            xmlPopInput(ctxt);
            if(OOM_FLAG)
                return;
        } else {
            const unsigned char *cur;
            unsigned char c;

            /*
             *   2.11 End-of-Line Handling
             *   the literal two-character sequence "#xD#xA" or a standalone
             *   literal #xD, an XML processor must pass to the application
             *   the single character #xA.
             */
            if (*(ctxt->input->cur) == '\n') {
                ctxt->input->line++;
                ctxt->input->col = 1;
            } else
                ctxt->input->col++;

            /*
             * We are supposed to handle UTF8, check it's valid
             * From rfc2044: encoding of the Unicode values on UTF-8:
             *
             * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
             * 0000 0000-0000 007F   0xxxxxxx
             * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
             * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
             *
             * Check for the 0x110000 limit too
             */
            cur = ctxt->input->cur;

            c = *cur;
            if (c & 0x80) {
            if (c == 0xC0)
            goto encoding_error;
                if (cur[1] == 0){
                    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                    if(OOM_FLAG)
                        return;
                }
                if ((cur[1] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xe0) == 0xe0)
                {
                    unsigned int val;

                    if (cur[2] == 0){
                        xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                        if(OOM_FLAG)
                            return;
                    }
                    if ((cur[2] & 0xc0) != 0x80)
                        goto encoding_error;
                    if ((c & 0xf0) == 0xf0)
                    {
                        if (cur[3] == 0){
                            xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                            if(OOM_FLAG)
                                return;
                        }
                        if (((c & 0xf8) != 0xf0) ||
                            ((cur[3] & 0xc0) != 0x80))
                            goto encoding_error;
                        /* 4-byte code */
                        ctxt->input->cur += 4;
                        val = (cur[0] & 0x7) << 18;
                        val |= (cur[1] & 0x3f) << 12;
                        val |= (cur[2] & 0x3f) << 6;
                        val |= cur[3] & 0x3f;
                    } else {
                        /* 3-byte code */
                        ctxt->input->cur += 3;
                        val = (cur[0] & 0xf) << 12;
                        val |= (cur[1] & 0x3f) << 6;
                        val |= cur[2] & 0x3f;
                    }
                    if (((val > 0xd7ff) && (val < 0xe000)) ||
                        ((val > 0xfffd) && (val < 0x10000)) ||
                        (val >= 0x110000))
                    {
                        xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
                                EMBED_ERRTXT("Char 0x%X out of allowed range\n"),
                                val);
                    }
                }else{
                    /* 2-byte code */
                    ctxt->input->cur += 2;
                }
            }else{
                /* 1-byte code */
                ctxt->input->cur++;
            }
            ctxt->nbChars++;
            if (*ctxt->input->cur == 0){
                xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                if(OOM_FLAG)
                    return;
            }
        }
    } else {
        /*
         * Assume it's a fixed length encoding (1) with
         * a compatible encoding for the ASCII set, since
         * XML constructs only use < 128 chars
         */

        if (*(ctxt->input->cur) == '\n') {
            ctxt->input->line++;
            ctxt->input->col = 1;
        }else{
            ctxt->input->col++;
        }
        ctxt->input->cur++;
        ctxt->nbChars++;
        if (*ctxt->input->cur == 0){
            xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
            if(OOM_FLAG)
                return;
        }
    } 
    if ((*ctxt->input->cur == '%') && (!ctxt->html)){
        xmlParserHandlePEReference(ctxt);//ISSUE: OOM review pending
    }
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0)) 
    {
        xmlPopInput(ctxt); // may set OOM flag -- not checked here
    }
    return;
encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    __xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
           EMBED_ERRTXT("Input is not proper UTF-8, indicate encoding !\n"),
           NULL, NULL);
    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
        ctxt->sax->error(ctxt->userData,
                         EMBED_ERRTXT("Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n"),
                         ctxt->input->cur[0], ctxt->input->cur[1],
                         ctxt->input->cur[2], ctxt->input->cur[3]);
    }
    ctxt->charset = XML_CHAR_ENCODING_8859_1;
    ctxt->input->cur++;
    return;
}

/**
 * xmlCurrentChar:
 * @param ctxt the XML parser context
 * @param len pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer. Implement the end of line normalization:
 * 2.11 End-of-Line Handling
 * Wherever an external parsed entity or the literal entity value
 * of an internal parsed entity contains either the literal two-character
 * sequence "#xD#xA" or a standalone literal #xD, an XML processor
 * must pass to the application the single character #xA.
 * This behavior can conveniently be produced by normalizing all
 * line breaks to #xA on input, before parsing.)
 *
 * Returns the current char value and its length
 *
 * OOM: possible --> OOM flag is set when returns 0
 */

XMLPUBFUNEXPORT int
xmlCurrentChar(xmlParserCtxtPtr ctxt, int *len) {
	LOAD_GS(ctxt)
    if (ctxt->instate == XML_PARSER_EOF)
        return(0);

    if ((*ctxt->input->cur >= 0x20) && (*ctxt->input->cur <= 0x7F))
    {
        *len = 1;
        return((int) *ctxt->input->cur);
    }
    if (ctxt->charset == XML_CHAR_ENCODING_UTF8)
    {
        /*
        * We are supposed to handle UTF8, check it's valid
        * From rfc2044: encoding of the Unicode values on UTF-8:
        *
        * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
        * 0000 0000-0000 007F   0xxxxxxx
        * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
        * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
        *
        * Check for the 0x110000 limit too
        */
        const unsigned char *cur = ctxt->input->cur;
        unsigned char c;
        unsigned int val;

        c = *cur;
        
        if (c & 0x80)
        {
            if (c == 0xC0)
                goto encoding_error;
            if (cur[1] == 0){
                xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                if(OOM_FLAG)
                    return 0;
            }
            if ((cur[1] & 0xc0) != 0x80)
                goto encoding_error;
            if ((c & 0xe0) == 0xe0)
            {
                if (cur[2] == 0){
                    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                    if(OOM_FLAG)
                        return 0;
                }
                if ((cur[2] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xf0) == 0xf0)
                {
                    if (cur[3] == 0){
                        xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                        if(OOM_FLAG)
                            return 0;
                    }
                    if (((c & 0xf8) != 0xf0) ||
                        ((cur[3] & 0xc0) != 0x80))
                    {
                        goto encoding_error;
                    }
                    /* 4-byte code */
                    *len = 4;
                    val = (cur[0] & 0x7) << 18;
                    val |= (cur[1] & 0x3f) << 12;
                    val |= (cur[2] & 0x3f) << 6;
                    val |= cur[3] & 0x3f;
                } else {
                /* 3-byte code */
                    *len = 3;
                    val = (cur[0] & 0xf) << 12;
                    val |= (cur[1] & 0x3f) << 6;
                    val |= cur[2] & 0x3f;
                }
            } else {
                /* 2-byte code */
                *len = 2;
                val = (cur[0] & 0x1f) << 6;
                val |= cur[1] & 0x3f;
            }
            if (!IS_CHAR(val)) {
                xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
                    EMBED_ERRTXT("Char 0x%X out of allowed range\n"), val);
            }
            return(val);
        } // if (c & 0x80)
        else
        {
            /* 1-byte code */
            *len = 1;
            if (*ctxt->input->cur == 0xD) {
                if (ctxt->input->cur[1] == 0xA) {
                    ctxt->nbChars++;
                    ctxt->input->cur++;
                }
                return(0xA);
            }
            return((int) *ctxt->input->cur);
        }
    } // if (ctxt->charset == XML_CHAR_ENCODING_UTF8)
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    if (*ctxt->input->cur == 0xD) {
        if (ctxt->input->cur[1] == 0xA) {
            ctxt->nbChars++;
            ctxt->input->cur++;
        }
        return(0xA);
    }
    return((int) *ctxt->input->cur);
encoding_error:
    /*
     * An encoding problem may arise from a truncated input buffer
     * splitting a character in the middle. In that case do not raise
     * an error but return 0 to endicate an end of stream problem
     */
    if (ctxt->input->end - ctxt->input->cur < 4) {
        *len = 0;
        return(0);
    }

    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    __xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
           EMBED_ERRTXT("Input is not proper UTF-8, indicate encoding !\n"),
           NULL, NULL);
    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
    {
        ctxt->sax->error(
            ctxt->userData,
            EMBED_ERRTXT("Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n"),
            ctxt->input->cur[0],
            ctxt->input->cur[1],
            ctxt->input->cur[2],
            ctxt->input->cur[3]);
    }
    ctxt->charset = XML_CHAR_ENCODING_8859_1;
    *len = 1;
    return((int) *ctxt->input->cur);
}

/**
 * xmlStringCurrentChar:
 * @param ctxt the XML parser context
 * @param cur pointer to the beginning of the char
 * @param len pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer.
 *
 * Returns the current char value and its length
 */

XMLPUBFUNEXPORT int
xmlStringCurrentChar(xmlParserCtxtPtr ctxt, const xmlChar * cur, int *len)
{
    if ((ctxt == NULL) || (ctxt->charset == XML_CHAR_ENCODING_UTF8)) {
        /*
         * We are supposed to handle UTF8, check it's valid
         * From rfc2044: encoding of the Unicode values on UTF-8:
         *
         * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
         * 0000 0000-0000 007F   0xxxxxxx
         * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
         * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
         *
         * Check for the 0x110000 limit too
         */
        unsigned char c;
        unsigned int val;

        c = *cur;
        if (c & 0x80) {
            if ((cur[1] & 0xc0) != 0x80)
                goto encoding_error;
            if ((c & 0xe0) == 0xe0) {

                if ((cur[2] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xf0) == 0xf0) {
                    if (((c & 0xf8) != 0xf0) || ((cur[3] & 0xc0) != 0x80))
                        goto encoding_error;
                    /* 4-byte code */
                    *len = 4;
                    val = (cur[0] & 0x7) << 18;
                    val |= (cur[1] & 0x3f) << 12;
                    val |= (cur[2] & 0x3f) << 6;
                    val |= cur[3] & 0x3f;
                } else {
                    /* 3-byte code */
                    *len = 3;
                    val = (cur[0] & 0xf) << 12;
                    val |= (cur[1] & 0x3f) << 6;
                    val |= cur[2] & 0x3f;
                }
            } else {
                /* 2-byte code */
                *len = 2;
                val = (cur[0] & 0x1f) << 6;
                val |= cur[1] & 0x3f;
            }
            if (!IS_CHAR(val)) {
            xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
                  EMBED_ERRTXT("Char 0x%X out of allowed range\n"), val);
            }
            return (val);
        } else {
            /* 1-byte code */
            *len = 1;
            return ((int) *cur);
        }
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    return ((int) *cur);
encoding_error:

    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    __xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
           EMBED_ERRTXT("Input is not proper UTF-8, indicate encoding !\n"),
           NULL, NULL);
    if ((ctxt != NULL) && (ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
    ctxt->sax->error(ctxt->userData,
             EMBED_ERRTXT("Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n"),
             ctxt->input->cur[0], ctxt->input->cur[1],
             ctxt->input->cur[2], ctxt->input->cur[3]);
    }
    *len = 1;
    return ((int) *cur);
}

/**
 * xmlCopyCharMultiByte:
 * @param out pointer to an array of xmlChar
 * @param val the char value
 *
 * append the char value in the array
 *
 * Returns the number of xmlChar written
 *
 * OOM: never
 */
XMLPUBFUNEXPORT int
xmlCopyCharMultiByte(xmlChar *out, int val) {
    /*
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
     */
    if  (val >= 0x80) {
        xmlChar *savedout = out;
        int bits;
        if (val <   0x800)
            { *out++= (val >>  6) | 0xC0;  bits=  0; }
        else if (val < 0x10000)
            { *out++= (val >> 12) | 0xE0;  bits=  6;}
        else if (val < 0x110000)
            { *out++= (val >> 18) | 0xF0;  bits=  12; }
        else
        {
            xmlErrEncodingInt(NULL, XML_ERR_INVALID_CHAR,
                EMBED_ERRTXT("Internal error, xmlCopyCharMultiByte 0x%X out of bound\n"),
                    val);
            return(0);
        }
        for ( ; bits >= 0; bits-= 6)
            *out++= ((val >> bits) & 0x3F) | 0x80 ;
        return (out - savedout);
    }
    *out = (xmlChar) val;
    return 1;
}

/**
 * xmlCopyChar:
 * @param len Ignored, compatibility
 * @param out pointer to an array of xmlChar
 * @param val the char value
 *
 * append the char value in the array
 *
 * Returns the number of xmlChar written
 */

XMLPUBFUNEXPORT int
xmlCopyChar(int len ATTRIBUTE_UNUSED, xmlChar *out, int val)
{
    /* the len parameter is ignored */
    if  (val >= 0x80) {
        return(xmlCopyCharMultiByte (out, val));
    }
    *out = (xmlChar) val;
    return 1;
}

/************************************************************************
 *                                                                      *
 *      Commodity functions to switch encodings                         *
 *                                                                      *
 ************************************************************************/

/**
 * xmlSwitchEncoding:
 * @param ctxt the parser context
 * @param enc the encoding value (number)
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 *
 * OOM: possible --> OOM flag is set -- check it always!
 */
XMLPUBFUNEXPORT int
xmlSwitchEncoding(xmlParserCtxtPtr ctxt, xmlCharEncoding enc)
{
	LOAD_GS(ctxt)
    xmlCharEncodingHandlerPtr handler;
    
    switch (enc) {
    case XML_CHAR_ENCODING_ERROR:
        __xmlErrEncoding(ctxt, XML_ERR_UNKNOWN_ENCODING,
                       EMBED_ERRTXT("encoding unknown\n"), NULL, NULL);
        break;
    case XML_CHAR_ENCODING_NONE:
        /* let's assume it's UTF-8 without the XML decl */
        ctxt->charset = XML_CHAR_ENCODING_UTF8;
        return(0);
    case XML_CHAR_ENCODING_UTF8:
        /* default encoding, no conversion should be needed */
        ctxt->charset = XML_CHAR_ENCODING_UTF8;

        /*
         * Errata on XML-1.0 June 20 2001
         * Specific handling of the Byte Order Mark for
         * UTF-8
         */
        if ((ctxt->input != NULL) &&
            (ctxt->input->cur[0] == 0xEF) &&
            (ctxt->input->cur[1] == 0xBB) &&
            (ctxt->input->cur[2] == 0xBF))
        {
            ctxt->input->cur += 3;
        }
        return(0);
    case XML_CHAR_ENCODING_UTF16LE:
    case XML_CHAR_ENCODING_UTF16BE:
        /*The raw input characters are encoded
         *in UTF-16. As we expect this function
         *to be called after xmlCharEncInFunc, we expect
         *ctxt->input->cur to contain UTF-8 encoded characters.
         *So the raw UTF16 Byte Order Mark
         *has also been converted into
         *an UTF-8 BOM. Let's skip that BOM.
         */
        if ((ctxt->input != NULL) &&
            (ctxt->input->cur[0] == 0xEF) &&
            (ctxt->input->cur[1] == 0xBB) &&
            (ctxt->input->cur[2] == 0xBF))
        {
            ctxt->input->cur += 3;
        }
    break ;
    default:
        break;
    }
    handler = xmlGetCharEncodingHandler(enc);
    if (handler == NULL) {
        if(OOM_FLAG)
            return -1;
        /*
        * Default handlers.
        */
        switch (enc) {
            case XML_CHAR_ENCODING_ERROR:
                __xmlErrEncoding(ctxt, XML_ERR_UNKNOWN_ENCODING,
                    EMBED_ERRTXT("encoding unknown\n"), NULL, NULL);
                break;
            case XML_CHAR_ENCODING_NONE:
                /* let's assume it's UTF-8 without the XML decl */
                ctxt->charset = XML_CHAR_ENCODING_UTF8;
                return(0);
            case XML_CHAR_ENCODING_UTF8:
            case XML_CHAR_ENCODING_ASCII:
                /* default encoding, no conversion should be needed */
                ctxt->charset = XML_CHAR_ENCODING_UTF8;
                return(0);
            case XML_CHAR_ENCODING_UTF16LE:
                break;
            case XML_CHAR_ENCODING_UTF16BE:
                break;
            case XML_CHAR_ENCODING_UCS4LE:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "USC4 little endian", NULL);
                break;
            case XML_CHAR_ENCODING_UCS4BE:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "USC4 big endian", NULL);
                break;
            case XML_CHAR_ENCODING_EBCDIC:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "EBCDIC", NULL);
                break;
            case XML_CHAR_ENCODING_UCS4_2143:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "UCS4 2143", NULL);
                break;
            case XML_CHAR_ENCODING_UCS4_3412:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "UCS4 3412", NULL);
                break;
            case XML_CHAR_ENCODING_UCS2:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "UCS2", NULL);
                break;
            case XML_CHAR_ENCODING_8859_1:
            case XML_CHAR_ENCODING_8859_2:
            case XML_CHAR_ENCODING_8859_3:
            case XML_CHAR_ENCODING_8859_4:
            case XML_CHAR_ENCODING_8859_5:
            case XML_CHAR_ENCODING_8859_6:
            case XML_CHAR_ENCODING_8859_7:
            case XML_CHAR_ENCODING_8859_8:
            case XML_CHAR_ENCODING_8859_9:
                /*
                * We used to keep the internal content in the
                * document encoding however this turns being unmaintainable
                * So xmlGetCharEncodingHandler() will return non-null
                * values for this now.
                */
                if ((ctxt->inputNr == 1) &&
                    (ctxt->encoding == NULL) &&
                    (ctxt->input->encoding != NULL))
                {
                    ctxt->encoding = xmlStrdup(ctxt->input->encoding);
                }
                ctxt->charset = enc;
                return(0);
            case XML_CHAR_ENCODING_2022_JP:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "ISO-2022-JP", NULL);
                break;
            case XML_CHAR_ENCODING_SHIFT_JIS:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "Shift_JIS", NULL);
                break;
            case XML_CHAR_ENCODING_EUC_JP:
                __xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
                        EMBED_ERRTXT("encoding not supported %s\n"),
                        BAD_CAST "EUC-JP", NULL);
                break;
        }
    }
    if (handler == NULL)
        return(-1);
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    return(xmlSwitchToEncoding(ctxt, handler));
}

/**
 * xmlSwitchInputEncoding:
 * @param ctxt the parser context
 * @param input the input stream
 * @param handler the encoding handler
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 *
 * OOM: possible --> check OOM flag when -1 is returned
 */
XMLPUBFUNEXPORT int
xmlSwitchInputEncoding(xmlParserCtxtPtr ctxt, xmlParserInputPtr input,
                       xmlCharEncodingHandlerPtr handler)
{
    int nbchars;
    LOAD_GS_SAFE_CTXT(ctxt)

    if (!handler || !input)
        return (-1);

    if (input->buf)
    {
        if (input->buf->encoder)
        {
            /*
             * Check in case the auto encoding detetection triggered
             * in already.
             */
            if (input->buf->encoder == handler)
                return (0);

            /*
             * "UTF-16" can be used for both LE and BE
             if ((!xmlStrncmp(BAD_CAST input->buf->encoder->name,
             BAD_CAST "UTF-16", 6)) &&
             (!xmlStrncmp(BAD_CAST handler->name,
             BAD_CAST "UTF-16", 6))) {
             return(0);
             }
             */

            /*
             * Note: this is a bit dangerous, but that's what it
             * takes to use nearly compatible signature for different
             * encodings.
             */
            xmlCharEncCloseFunc(input->buf->encoder);
            input->buf->encoder = handler;
            return (0);
        }
        input->buf->encoder = handler;

        /*
         * Is there already some content down the pipe to convert ?
         */
        if (input->buf->buffer && (input->buf->buffer->use > 0))
        {
            int processed;
            unsigned int use;
            
            /*
             * Specific handling of the Byte Order Mark for
             * UTF-16
             */
            if ((handler->name != NULL) &&
                (!strcmp(handler->name, "UTF-16LE")) &&
                (input->cur[0] == 0xFF) && (input->cur[1] == 0xFE))
            {
                input->cur += 2;
            }
            if ((handler->name != NULL) &&
                (!strcmp(handler->name, "UTF-16BE")) &&
                (input->cur[0] == 0xFE) && (input->cur[1] == 0xFF))
            {
                input->cur += 2;
            }
            /*
             * Errata on XML-1.0 June 20 2001
             * Specific handling of the Byte Order Mark for
             * UTF-8
             */
            if ((handler->name != NULL) &&
                (!strcmp(handler->name, "UTF-8")) &&
                (input->cur[0] == 0xEF) &&
                (input->cur[1] == 0xBB) && (input->cur[2] == 0xBF))
            {
                input->cur += 3;
            }

            /*
             * Shrink the current input buffer.
             * Move it as the raw buffer and create a new input buffer
             */
            processed = input->cur - input->base;
            xmlBufferShrink(input->buf->buffer, processed);
            input->buf->raw = input->buf->buffer;
            input->buf->buffer = xmlBufferCreate(); 
            input->buf->rawconsumed = processed;
            use = input->buf->raw->use;

            if (ctxt->html) {
                /*
                 * convert as much as possible of the buffer
                 */
                nbchars = xmlCharEncInFunc(input->buf->encoder,
                                           input->buf->buffer,
                                           input->buf->raw);
            } else {
                /*
                 * convert just enough to get
                 * '<?xml version="1.0" encoding="xxx"?>'
                 * parsed with the autodetected encoding
                 * into the parser reading buffer.
                 */
                nbchars = xmlCharEncFirstLine(input->buf->encoder,
                                              input->buf->buffer,
                                              input->buf->raw);
            }
            if(OOM_FLAG)
                return -1;

            if (nbchars < 0) {
                xmlErrInternal(ctxt,
                               EMBED_ERRTXT("switching encoding: encoder error\n"),
                               NULL);
                return (-1);
            }
            input->buf->rawconsumed += use - input->buf->raw->use;
            input->base = input->cur = input->buf->buffer->content;
            input->end = &input->base[input->buf->buffer->use];
        }
        return (0);
    } // end if (input->buf)
    else if (input->length == 0)
    {
    // NOTE: part of the ELSE branch was removed -- it is so in newer version from open-source
            /*
             * When parsing a static memory array one must know the
             * size to be able to convert the buffer.
             */
            xmlErrInternal(ctxt, EMBED_ERRTXT("switching encoding : no input\n"), NULL);
            return (-1);
    }
    return (0);
}

/**
 * xmlSwitchToEncoding:
 * @param ctxt the parser context
 * @param handler the encoding handler
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 *
 * OOM: possible --> OOM flag is set when -1 is returned
 */
XMLPUBFUNEXPORT int
xmlSwitchToEncoding(xmlParserCtxtPtr ctxt, xmlCharEncodingHandlerPtr handler)
{
	LOAD_GS(ctxt)
    if(!handler)
        return -1;

    if (ctxt->input != NULL) {
        xmlSwitchInputEncoding(ctxt, ctxt->input, handler);
        if(OOM_FLAG)
            return -1;
    } else {
        xmlErrInternal(ctxt, EMBED_ERRTXT("xmlSwitchToEncoding : no input\n"), NULL);
        return(-1);
    }
    /*
    * The parsing is now done in UTF8 natively
    */
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    return(0);
}

/************************************************************************
 *                                                                      *
 *  Commodity functions to handle entities processing                   *
 *                                                                      *
 ************************************************************************/

/**
 * xmlFreeInputStream:
 * @param input an xmlParserInputPtr
 *
 * Free up an input stream.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlFreeInputStream(xmlParserInputPtr input) {
    if (!input) return;

    if ( input->filename )  xmlFree((char*) input->filename);
    if ( input->directory ) xmlFree((char*) input->directory);
    if ( input->encoding )  xmlFree((char*) input->encoding);
    if ( input->version )   xmlFree((char*) input->version);

    if ( input->free && input->base )
        input->free((xmlChar*) input->base);

    if ( input->buf )
        xmlFreeParserInputBuffer(input->buf);
    xmlFree(input);
}

/**
 * xmlNewInputStream:
 * @param ctxt an XML parser context
 *
 * Create a new input stream structure
 * Returns the new input stream or NULL
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlNewInputStream(xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr input;
	LOAD_GS_SAFE_CTXT(ctxt)
	
    
    input = (xmlParserInputPtr) xmlMalloc(sizeof(xmlParserInput));
    if (input == NULL) {
        xmlErrMemory(ctxt,  EMBED_ERRTXT("couldn't allocate a new input stream\n"));
        return(NULL);
    }
    memset(input, 0, sizeof(xmlParserInput));
    input->line = 1;
    input->col = 1;
    input->standalone = -1;
    /*
     * we don't care about thread reentrancy unicity for a single
     * parser context (and hence thread) is sufficient.
     */
    input->id = xmlInputStreamId++;
    return(input);
}

/**
 * xmlNewIOInputStream:
 * @param ctxt an XML parser context
 * @param input an I/O Input
 * @param enc the charset encoding if known
 *
 * Create a new input stream structure encapsulating the input into
 * a stream suitable for the parser.
 *
 * Returns the new input stream or NULL
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlNewIOInputStream(xmlParserCtxtPtr ctxt, xmlParserInputBufferPtr input,
                xmlCharEncoding enc) {
    xmlParserInputPtr inputStream;
    LOAD_GS_SAFE_CTXT(ctxt)                	

    if (xmlParserDebugEntities)
    xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("new input from I/O\n"));
    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
    return(NULL);
    }
    inputStream->filename = NULL;
    inputStream->buf = input;
    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;
    inputStream->end = &inputStream->base[inputStream->buf->buffer->use];
    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    return(inputStream);
}

/**
 * xmlNewEntityInputStream:
 * @param ctxt an XML parser context
 * @param entity an Entity pointer
 *
 * Create a new input stream based on an xmlEntityPtr
 *
 * Returns the new input stream or NULL
 *
 * OOM:
 */
//ISSUE: unreviewed branch of code for OOM handling
XMLPUBFUNEXPORT xmlParserInputPtr
xmlNewEntityInputStream(xmlParserCtxtPtr ctxt, xmlEntityPtr entity) {
    xmlParserInputPtr input;
    LOAD_GS_SAFE_CTXT(ctxt)

    if (entity == NULL) {
        xmlErrInternal(ctxt, EMBED_ERRTXT("xmlNewEntityInputStream entity = NULL\n"), NULL);
        return(NULL);
    }
    
    if (xmlParserDebugEntities)
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("new input from entity: %s\n"), entity->name);

    if (entity->content == NULL) {
        switch (entity->etype) {
                case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
                    xmlErrInternal(ctxt, EMBED_ERRTXT("Cannot parse entity %s\n"), entity->name);
                    break;
                case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
                case XML_EXTERNAL_PARAMETER_ENTITY:
                    return(xmlLoadExternalEntity(
                                    (char*) entity->URI,
                                    (char*) entity->ExternalID, ctxt));
                case XML_INTERNAL_GENERAL_ENTITY:
                    xmlErrInternal(ctxt,
                    EMBED_ERRTXT("Internal entity %s without content !\n"),
                            entity->name);
                    break;
                case XML_INTERNAL_PARAMETER_ENTITY:
                    xmlErrInternal(ctxt,
                    EMBED_ERRTXT("Internal parameter entity %s without content !\n"),
                            entity->name);
                    break;
                case XML_INTERNAL_PREDEFINED_ENTITY:
                    xmlErrInternal(ctxt,
                    EMBED_ERRTXT("Predefined entity %s without content !\n"),
                            entity->name);
                    break;
        }
        return(NULL);
    }
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
        return(NULL);
    }
    input->filename = (char *) entity->URI;
    input->base = entity->content;
    input->cur = entity->content;
    input->length = entity->length;
    input->end = &entity->content[input->length];
    return(input);
}

/**
 * xmlNewStringInputStream:
 * @param ctxt an XML parser context
 * @param buffer an memory buffer
 *
 * Create a new input stream based on a memory buffer.
 * Returns the new input stream
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlNewStringInputStream(xmlParserCtxtPtr ctxt, const xmlChar *buffer) {
    xmlParserInputPtr input;
	LOAD_GS_SAFE_CTXT(ctxt)
	
    if (buffer == NULL) {
        xmlErrInternal(ctxt, EMBED_ERRTXT("xmlNewStringInputStream string = NULL\n"),
                   NULL);
    return(NULL);
    }
    if (xmlParserDebugEntities)
    xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("new fixed input: %.30s\n"), buffer);
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
        xmlErrMemory(ctxt,  EMBED_ERRTXT("couldn't allocate a new input stream\n"));
    return(NULL);
    }
    input->base = buffer;
    input->cur = buffer;
    input->length = xmlStrlen(buffer);
    input->end = &buffer[input->length];
    return(input);
}

/**
 * xmlNewInputFromFile:
 * @param ctxt an XML parser context
 * @param filename the filename to use as entity
 *
 * Create a new input stream based on a file or an URL.
 *
 * Returns the new input stream or NULL in case of error
 *
 * OOM: possible --> always check OOM flag
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlNewInputFromFile(xmlParserCtxtPtr ctxt, const char* filename) {
    xmlParserInputBufferPtr buf;
    xmlParserInputPtr       inputStream;
    // 'directory' variable was removed during optimization
    const xmlChar*          URI;
    LOAD_GS_SAFE_CTXT(ctxt)

    if (xmlParserDebugEntities)
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("new input from file: %s\n"), filename);

    if (!ctxt)
        return(NULL);

    buf = xmlParserInputBufferCreateFilename(filename, XML_CHAR_ENCODING_NONE);
    if (!buf) {
        __xmlLoaderErr(ctxt, EMBED_ERRTXT("failed to load external entity \"%s\"\n"),
                     (const char*) filename);
        return(NULL);
    }

    inputStream = xmlNewInputStream(ctxt);
    if (!inputStream) {
        xmlFreeParserInputBuffer(buf);
        return(NULL);
    }

    inputStream->buf = buf;
    inputStream = xmlCheckHTTPInput(ctxt, inputStream);
    if (!inputStream)
        return(NULL);

    URI = (const xmlChar*)(inputStream->filename
                            ? inputStream->filename
                            : filename);

    inputStream->directory = xmlParserGetDirectory((const char*) URI);
    inputStream->filename = (char*) xmlCanonicPath((const xmlChar*) URI);
    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur  = inputStream->buf->buffer->content;
    inputStream->end = &inputStream->base[inputStream->buf->buffer->use];

    if (!ctxt->directory && inputStream->directory)
        ctxt->directory = (char*) xmlStrdup((const xmlChar*) inputStream->directory);

    return(inputStream);
}

/************************************************************************
 *                                                                      *
 *      Commodity functions to handle parser contexts                   *
 *                                                                      *
 ************************************************************************/

/**
 * xmlInitParserCtxt:
 * @param ctxt an XML parser context
 *
 * Initialize a parser context
 *
 * Returns 0 in case of success and -1 in case of error
 *
 * OOM: possible --> returns -1;  OOM flag is set
 */

XMLPUBFUNEXPORT int
xmlInitParserCtxt(xmlParserCtxtPtr ctxt)
{   
	LOAD_GS_SAFE_CTXT(ctxt)
	
    
    if(ctxt==NULL) {
        xmlErrInternal(NULL, EMBED_ERRTXT("Got NULL parser context\n"), NULL);
        return(-1);
    }

    xmlDefaultSAXHandlerInit();

    ctxt->dict = xmlDictCreate();
    if (!ctxt->dict) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot initialize parser context\n"));
        return(-1);
    }
    ctxt->sax = (xmlSAXHandler*) xmlMalloc(sizeof(xmlSAXHandler));
    if (ctxt->sax == NULL) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot initialize parser context\n"));
        return(-1);
    }
    else
        xmlSAXVersion(ctxt->sax, 2);

    ctxt->maxatts = 0;
    ctxt->atts = NULL;
    /* Allocate the Input stack */
    ctxt->inputTab = (xmlParserInputPtr *) xmlMalloc(5 * sizeof(xmlParserInputPtr));
    if (ctxt->inputTab == NULL) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot initialize parser context\n"));
        goto OOM;
    }
    ctxt->inputNr = 0;
    ctxt->inputMax = 5;
    ctxt->input = NULL;

    ctxt->version = NULL;
    ctxt->encoding = NULL;
    ctxt->standalone = -1;
    ctxt->hasExternalSubset = 0;
    ctxt->hasPErefs = 0;
    ctxt->html = 0;
    ctxt->external = 0;
    ctxt->instate = XML_PARSER_START;
    ctxt->token = 0;
    ctxt->directory = NULL;

    
    /* Allocate the Node stack */
    ctxt->nodeTab = (xmlNodePtr*) xmlMalloc(10 * sizeof(xmlNodePtr));
    if (ctxt->nodeTab == NULL) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot initialize parser context\n"));
        goto OOM_input_tab;
    }
    ctxt->nodeNr = 0;
    ctxt->nodeMax = 10;
    ctxt->node = NULL;

    /* Allocate the Name stack */
    ctxt->nameTab = (const xmlChar**) xmlMalloc(10 * sizeof(xmlChar*));
    if (!ctxt->nameTab) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot initialize parser context\n"));
        goto OOM_node_tab;
    }
    ctxt->nameNr = 0;
    ctxt->nameMax = 10;
    ctxt->name = NULL;

    /* Allocate the space stack */
    ctxt->spaceTab = (int*) xmlMalloc(10 * sizeof(int));
    if (!ctxt->spaceTab) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot initialize parser context\n"));
        goto OOM_name_tab;
    }
    ctxt->spaceNr = 1;
    ctxt->spaceMax = 10;
    ctxt->spaceTab[0] = -1;
    ctxt->space = &ctxt->spaceTab[0];
    ctxt->userData = ctxt;
    ctxt->myDoc = NULL;
    ctxt->wellFormed = 1;
    ctxt->nsWellFormed = 1;
    ctxt->valid = 1;
    ctxt->loadsubset = xmlLoadExtDtdDefaultValue;
    ctxt->validate = xmlDoValidityCheckingDefaultValue;
    ctxt->pedantic = xmlPedanticParserDefaultValue;
#ifdef LIBXML_ENABLE_NODE_LINEINFO
    ctxt->linenumbers = xmlLineNumbersDefaultValue;
#endif
    ctxt->keepBlanks = xmlKeepBlanksDefaultValue;

    if (ctxt->keepBlanks == 0)
        ctxt->sax->ignorableWhitespace = xmlSAX2IgnorableWhitespace;

    ctxt->vctxt.userData = ctxt;
    ctxt->vctxt.error = xmlParserValidityError;
    ctxt->vctxt.warning = xmlParserValidityWarning;

    if (ctxt->validate) {
        if (xmlGetWarningsDefaultValue == 0)
           ctxt->vctxt.warning = NULL;
        else
            ctxt->vctxt.warning = xmlParserValidityWarning;
        ctxt->vctxt.nodeMax = 0;
    }

    ctxt->replaceEntities = xmlSubstituteEntitiesDefaultValue;

#ifdef XMLENGINE_ENABLE_PARSER_RECORD_INFO
    ctxt->record_info = 0;
    xmlInitNodeInfoSeq(&ctxt->node_seq);
#endif

    ctxt->nbChars = 0;
    ctxt->checkIndex = 0;
    ctxt->inSubset = 0;
    ctxt->errNo = XML_ERR_OK;
    ctxt->depth = 0;
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    ctxt->catalogs = NULL;
    return(0);
//----------------------------------------------- OOM handling
    // >>: Note: Maybe just clear whole contents of *ctxt ? -- NO, we shouldn't free ctxt
    //       After all, do nothing.. let caller free partially constructed ctxt

OOM_name_tab:
    xmlFree(ctxt->nameTab);
    ctxt->nameTab = NULL;
    ctxt->spaceNr = 0;
    ctxt->spaceMax = 0;
    ctxt->space = NULL;
OOM_node_tab:
    xmlFree(ctxt->nodeTab);
    ctxt->nodeTab = NULL;
    ctxt->nameNr = 0;
    ctxt->nameMax = 0;
    ctxt->name = NULL;
OOM_input_tab:
    xmlFree(ctxt->inputTab);
    ctxt->inputTab = NULL;
    ctxt->nodeNr = 0;
    ctxt->nodeMax = 0;
    ctxt->node = NULL;
OOM:
    ctxt->inputNr = 0;
    ctxt->inputMax = 0;
    ctxt->input = NULL;
    return -1;
}

/**
 * xmlFreeParserCtxt:
 * @param ctxt an XML parser context
 *
 * Free all the memory used by a parser context. However the parsed
 * document in ctxt->myDoc is not freed.
 *
 * OOM: never
 */

XMLPUBFUNEXPORT void
xmlFreeParserCtxt(xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr input;
    LOAD_GS_SAFE_CTXT(ctxt)

    if (ctxt == NULL) return;

    while ((input = inputPop(ctxt)) != NULL) { /* Non consuming */
        xmlFreeInputStream(input);
    }
    if (ctxt->spaceTab) xmlFree(ctxt->spaceTab);
    if (ctxt->nameTab)  xmlFree((xmlChar**)ctxt->nameTab);
    if (ctxt->nodeTab)  xmlFree(ctxt->nodeTab);
    if (ctxt->inputTab) xmlFree(ctxt->inputTab); 
    if (ctxt->version)  xmlFree((char*) ctxt->version);
    if (ctxt->encoding) xmlFree((char*) ctxt->encoding);
    if (ctxt->extSubURI)xmlFree((char*) ctxt->extSubURI);
    if (ctxt->extSubSystem) xmlFree((char*) ctxt->extSubSystem);
#ifdef LIBXML_SAX1_ENABLED
    if ((ctxt->sax) &&
        (ctxt->sax != (xmlSAXHandlerPtr) &xmlDefaultSAXHandler))
#else
    if (ctxt->sax)
#endif /* LIBXML_SAX1_ENABLED */
        xmlFree(ctxt->sax);
    if (ctxt->directory)        xmlFree((char*) ctxt->directory);
    if (ctxt->vctxt.nodeTab)    xmlFree(ctxt->vctxt.nodeTab);
    if (ctxt->atts)             xmlFree((xmlChar**)ctxt->atts);
    if (ctxt->dict)             xmlDictFree(ctxt->dict);
    if (ctxt->nsTab)            xmlFree((char*) ctxt->nsTab);
    if (ctxt->pushTab)          xmlFree(ctxt->pushTab);
    if (ctxt->attallocs)        xmlFree(ctxt->attallocs);
    if (ctxt->attsDefault)
        xmlHashFree(ctxt->attsDefault, (xmlHashDeallocator) xmlFree);
    if (ctxt->attsSpecial)
        xmlHashFree(ctxt->attsSpecial, NULL);
    if (ctxt->freeElems) {
        xmlNodePtr cur, next;

        cur = ctxt->freeElems;
        while (cur) {
            next = cur->next;
            xmlFree(cur);
            cur = next;
        }
    }
    if (ctxt->freeAttrs) {
        xmlAttrPtr cur, next;

        cur = ctxt->freeAttrs;
        while (cur) {
            next = cur->next;
            xmlFree(cur);
            cur = next;
        }
    }
    /*
     * cleanup the error strings
     */
    xmlResetError(&ctxt->lastError);

#ifdef LIBXML_CATALOG_ENABLED
    if (ctxt->catalogs != NULL)
        xmlCatalogFreeLocal(ctxt->catalogs);
#endif
    xmlFree(ctxt);
}

/**
 * xmlNewParserCtxt:
 *
 * Allocate and initialize a new parser context.
 *
 * Returns the xmlParserCtxtPtr or NULL
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */

XMLPUBFUNEXPORT xmlParserCtxtPtr
xmlNewParserCtxt()
{
    xmlParserCtxtPtr ctxt;

    ctxt = (xmlParserCtxtPtr) xmlMalloc(sizeof(xmlParserCtxt));
    if (!ctxt) {
        xmlErrMemory(NULL, EMBED_ERRTXT("cannot allocate parser context\n"));
        return(NULL);
    }
    memset(ctxt, 0, sizeof(xmlParserCtxt));

#ifdef XE_ENABLE_GS_CACHING /*LIBXML_ENABLE_GS_CACHING_IN_CTXT*/
    ctxt->cachedGs = xmlGetGlobalState();
#endif

    if (xmlInitParserCtxt(ctxt) < 0) {
        // OOM!!!
        xmlFreeParserCtxt(ctxt);
        return(NULL);
    }
    return(ctxt);
}

/************************************************************************
 *                                                                      *
 *      Handling of node informations                                   *
 *                                                                      *
 ************************************************************************/

/**
 * xmlClearParserCtxt:
 * @param ctxt an XML parser context
 *
 * Clear (release owned resources) and reinitialize a parser context
 */

XMLPUBFUNEXPORT void
xmlClearParserCtxt(xmlParserCtxtPtr ctxt)
{
  if (!ctxt)
    return;
#ifdef XMLENGINE_ENABLE_PARSER_RECORD_INFO
  xmlClearNodeInfoSeq(&ctxt->node_seq);
#endif
  xmlInitParserCtxt(ctxt);
}

#ifdef XMLENGINE_ENABLE_PARSER_RECORD_INFO
/**
 * xmlParserFindNodeInfo:
 * @param ctx an XML parser context
 * @param node an XML node within the tree
 *
 * Find the parser node info struct for a given node
 *
 * Returns an xmlParserNodeInfo block pointer or NULL
 */
const xmlParserNodeInfo* xmlParserFindNodeInfo(const xmlParserCtxtPtr ctx,
                                               const xmlNodePtr node)
{
  unsigned long pos;

  /* Find position where node should be at */
  pos = xmlParserFindNodeInfoIndex(&ctx->node_seq, node);
  if (pos < ctx->node_seq.length && ctx->node_seq.buffer[pos].node == node)
    return &ctx->node_seq.buffer[pos];
  else
    return NULL;
}


/**
 * xmlInitNodeInfoSeq:
 * @param seq a node info sequence pointer
 *
 * -- Initialize (set to initial state) node info sequence
 *
 * OOM: never
 */
void
xmlInitNodeInfoSeq(xmlParserNodeInfoSeqPtr seq)
{
  seq->length = 0;
  seq->maximum = 0;
  seq->buffer = NULL;
}

/**
 * xmlClearNodeInfoSeq:
 * @param seq a node info sequence pointer
 *
 * -- Clear (release memory and reinitialize) node
 *   info sequence
 */
void
xmlClearNodeInfoSeq(xmlParserNodeInfoSeqPtr seq)
{
  if (seq->buffer)
    xmlFree(seq->buffer);
  xmlInitNodeInfoSeq(seq);
}


/**
 * xmlParserFindNodeInfoIndex:
 * @param seq a node info sequence pointer
 * @param node an XML node pointer
 *
 *
 * xmlParserFindNodeInfoIndex : Find the index that the info record for
 *   the given node is or should be at in a sorted sequence
 *
 * Returns a long indicating the position of the record
 */
unsigned long xmlParserFindNodeInfoIndex(const xmlParserNodeInfoSeqPtr seq,
                                         const xmlNodePtr node)
{
  unsigned long upper, lower, middle;
  int found = 0;

  /* Do a binary search for the key */
  lower = 1;
  upper = seq->length;
  middle = 0;
  while ( lower <= upper && !found) {
    middle = lower + (upper - lower) / 2;
    if ( node == seq->buffer[middle - 1].node )
      found = 1;
    else if ( node < seq->buffer[middle - 1].node )
      upper = middle - 1;
    else
      lower = middle + 1;
  }

  /* Return position */
  if ( middle == 0 || seq->buffer[middle - 1].node < node )
    return middle;
  else
    return middle - 1;
}


/**
 * xmlParserAddNodeInfo:
 * @param ctxt an XML parser context
 * @param info a node info sequence pointer
 *
 * Insert node info record into the sorted sequence
 */
 
void
xmlParserAddNodeInfo(xmlParserCtxtPtr ctxt,
                     const xmlParserNodeInfoPtr info)
{
    unsigned long pos;

    /* Find pos and check to see if node is already in the sequence */
    pos = xmlParserFindNodeInfoIndex(&ctxt->node_seq, (xmlNodePtr)
                                     info->node);
    if (pos < ctxt->node_seq.length
        && ctxt->node_seq.buffer[pos].node == info->node) {
        ctxt->node_seq.buffer[pos] = *info;
    }

    /* Otherwise, we need to add new node to buffer */
    else {
        if (ctxt->node_seq.length + 1 > ctxt->node_seq.maximum) {
            xmlParserNodeInfo *tmp_buffer;
            unsigned int byte_size;

            if (ctxt->node_seq.maximum == 0)
                ctxt->node_seq.maximum = 2;
            byte_size = (sizeof(*ctxt->node_seq.buffer) *
            (2 * ctxt->node_seq.maximum));

            if (ctxt->node_seq.buffer == NULL)
                tmp_buffer = (xmlParserNodeInfo *) xmlMalloc(byte_size);
            else
                tmp_buffer =
                    (xmlParserNodeInfo *) xmlRealloc(ctxt->node_seq.buffer,
                                                     byte_size);

            if (tmp_buffer == NULL) {
        xmlErrMemory(ctxt, EMBED_ERRTXT("failed to allocate buffer\n"));
                return;
            }
            ctxt->node_seq.buffer = tmp_buffer;
            ctxt->node_seq.maximum *= 2;
        }

        /* If position is not at end, move elements out of the way */
        if (pos != ctxt->node_seq.length) {
            unsigned long i;

            for (i = ctxt->node_seq.length; i > pos; i--)
                ctxt->node_seq.buffer[i] = ctxt->node_seq.buffer[i - 1];
        }

        /* Copy element and increase length */
        ctxt->node_seq.buffer[pos] = *info;
        ctxt->node_seq.length++;
    }
}

#endif /* XMLENGINE_ENABLE_PARSER_RECORD_INFO */

/************************************************************************
 *                                                                      *
 *      Defaults settings                                               *
 *                                                                      *
 ************************************************************************/
/**
 * xmlPedanticParserDefault:
 * @param val int 0 or 1
 *
 * Set and return the previous value for enabling pedantic warnings.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

XMLPUBFUNEXPORT int
xmlPedanticParserDefault(int val) {
	LOAD_GS_DIRECT
    int old = xmlPedanticParserDefaultValue;

    xmlPedanticParserDefaultValue = val;
    return(old);
}

/**
 * xmlSubstituteEntitiesDefault:
 * @param val int 0 or 1
 *
 * Set and return the previous value for default entity support.
 * Initially the parser always keep entity references instead of substituting
 * entity values in the output. This function has to be used to change the
 * default parser behavior
 * SAX::substituteEntities() has to be used for changing that on a file by
 * file basis.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

XMLPUBFUNEXPORT int
xmlSubstituteEntitiesDefault(int val) {
	LOAD_GS_DIRECT
    int old = xmlSubstituteEntitiesDefaultValue;

    xmlSubstituteEntitiesDefaultValue = val;
    return(old);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlKeepBlanksDefault:
 * @param val int 0 or 1
 *
 * Set and return the previous value for default blanks text nodes support.
 * The 1.x version of the parser used an heuristic to try to detect
 * ignorable white spaces. As a result the SAX callback was generating
 * xmlSAX2IgnorableWhitespace() callbacks instead of characters() one, and when
 * using the DOM output text nodes containing those blanks were not generated.
 * The 2.x and later version will switch to the XML standard way and
 * ignorableWhitespace() are only generated when running the parser in
 * validating mode and when the current element doesn't allow CDATA or
 * mixed content.
 * This function is provided as a way to force the standard behavior
 * on 1.X libs and to switch back to the old mode for compatibility when
 * running 1.X client code on 2.X . Upgrade of 1.X code should be done
 * by using xmlIsBlankNode() commodity function to detect the "empty"
 * nodes generated.
 * This value also affect autogeneration of indentation when saving code
 * if blanks sections are kept, indentation is not generated.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

int
xmlKeepBlanksDefault(int val) {
    int old = xmlKeepBlanksDefaultValue;

    xmlKeepBlanksDefaultValue = val;
    xmlIndentTreeOutput = !val;
    return(old);
}

#ifdef LIBXML_ENABLE_NODE_LINEINFO
/**
 * xmlLineNumbersDefault:
 * @param val int 0 or 1
 *
 * Set and return the previous value for enabling line numbers in elements
 * contents. This may break on old application and is turned off by default.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

int
xmlLineNumbersDefault(int val) {
    int old = xmlLineNumbersDefaultValue;

    xmlLineNumbersDefaultValue = val;
    return(old);
}
#endif

#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

