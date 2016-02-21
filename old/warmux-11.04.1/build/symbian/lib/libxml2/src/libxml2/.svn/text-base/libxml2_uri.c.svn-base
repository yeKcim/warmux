/**
 * libxml2_uri.c: set of generic URI related routines
 *
 * Reference: RFC 2396
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#include <string.h>

#include <libxml2_uri.h>
#include <libxml2_globals.h>

/************************************************************************
 *                                                                      *
 *      Macros to differentiate various character type                  *
 *          directly extracted from RFC 2396                            *
 *                                                                      *
 ************************************************************************/

/*
 * alpha    = lowalpha | upalpha
 */
#define IS_ALPHA(x) (IS_LOWALPHA(x) || IS_UPALPHA(x))


/*
 * lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" |
 *            "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" |
 *            "u" | "v" | "w" | "x" | "y" | "z"
 */

#define IS_LOWALPHA(x) (((x) >= 'a') && ((x) <= 'z'))

/*
 * upalpha = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" |
 *           "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" |
 *           "U" | "V" | "W" | "X" | "Y" | "Z"
 */
#define IS_UPALPHA(x) (((x) >= 'A') && ((x) <= 'Z'))

/*
 * digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 */

#define IS_DIGIT(x) (((x) >= '0') && ((x) <= '9'))

/*
 * alphanum = alpha | digit
 */

#define IS_ALPHANUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

/*
 * hex = digit | "A" | "B" | "C" | "D" | "E" | "F" |
 *               "a" | "b" | "c" | "d" | "e" | "f"
 */

#define IS_HEX(x) ((IS_DIGIT(x)) || (((x) >= 'a') && ((x) <= 'f')) || \
        (((x) >= 'A') && ((x) <= 'F')))

/*
 * mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
 */

#define IS_MARK(x) (((x) == '-') || ((x) == '_') || ((x) == '.') || \
    ((x) == '!') || ((x) == '~') || ((x) == '*') || ((x) == '\'') ||    \
    ((x) == '(') || ((x) == ')'))


/*
 * reserved = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ","
 */

#define IS_RESERVED(x) (((x) == ';') || ((x) == '/') || ((x) == '?') || \
        ((x) == ':') || ((x) == '@') || ((x) == '&') || ((x) == '=') || \
    ((x) == '+') || ((x) == '$') || ((x) == ','))

static int isReserved(char ch)
    {
    return IS_RESERVED(ch);
    }

/*
 * unreserved = alphanum | mark
 */
#define IS_UNRESERVED(x) (IS_ALPHANUM(x) || IS_MARK(x))

static int isUnreserved(char ch)
    {
    return IS_UNRESERVED(ch);
    }
/*
 * escaped = "%" hex hex
 */

#define IS_ESCAPED(p) \
            ((*(p) == '%') && (IS_HEX((p)[1])) && (IS_HEX((p)[2])))

static int isEscaped(const char* pch)
    {
    return IS_ESCAPED(pch);
    }
/*
 * uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" |
 *                        "&" | "=" | "+" | "$" | ","
 */
/*
#define IS_URIC_NO_SLASH(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||\
            ((*(p) == ';')) || ((*(p) == '?')) || ((*(p) == ':')) ||\
            ((*(p) == '@')) || ((*(p) == '&')) || ((*(p) == '=')) ||\
            ((*(p) == '+')) || ((*(p) == '$')) || ((*(p) == ',')))

Optimized into:

#define IS_URIC_NO_SLASH(p) \
            ((IS_UNRESERVED(*(p))) || \
             (IS_ESCAPED(p))       || \
             ((*(p)) != '/' && IS_RESERVED(p)))
*/
static int isUriCNoSlash(const char* pch)
    {
    char ch = *pch;
    return isUnreserved(ch) ||
           isEscaped(pch)     ||
           (ch != '/' && isReserved(ch));
    }


/*
 * pchar = unreserved | escaped | ":" | "@" | "&" | "=" | "+" | "$" | ","
 */

/*
#define IS_PCHAR(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||    \
            ((*(p) == ':')) || ((*(p) == '@')) || ((*(p) == '&')) ||\
            ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||\
            ((*(p) == ',')))
*/
static int isPChar(const char* pch)
    {
    char ch = *pch;
    return isUnreserved(ch) ||
           isEscaped(pch)   ||
           ch == ':' || ch == '@' || ch == '&' ||
           ch == '=' || ch == '+' || ch == '$' ||
           ch == ',';
    }


/*
 * rel_segment   = 1*( unreserved | escaped |
 *                 ";" | "@" | "&" | "=" | "+" | "$" | "," )
 */

/*
#define IS_SEGMENT(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||  \
            ((*(p) == ';')) || ((*(p) == '@')) || ((*(p) == '&')) ||  \
            ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||  \
            ((*(p) == ',')))
*/
static int isSegment(const char* pch)
    {
    char ch = *pch;
    return isUnreserved(ch)  || isEscaped(pch) ||
           ch == ';' || ch == '@' || ch == '&' ||
           ch == '=' || ch == '+' || ch == '$' ||
           ch == ',';
    }
/*
 * scheme = alpha *( alpha | digit | "+" | "-" | "." )
 */

#define IS_SCHEME(x) ((IS_ALPHA(x)) || (IS_DIGIT(x)) ||         \
                  ((x) == '+') || ((x) == '-') || ((x) == '.'))

/* the macros is used only once
static int isScheme(char x)
    {
    return ((IS_ALPHA(x)) || (IS_DIGIT(x)) ||
            (x) == '+') || ((x) == '-') || ((x) == '.'))
    }
*/

/*
 * reg_name = 1*( unreserved | escaped | "$" | "," |
 *                ";" | ":" | "@" | "&" | "=" | "+" )
 */

/*
#define IS_REG_NAME(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) || \
       ((*(p) == '$')) || ((*(p) == ',')) || ((*(p) == ';')) ||     \
       ((*(p) == ':')) || ((*(p) == '@')) || ((*(p) == '&')) ||     \
       ((*(p) == '=')) || ((*(p) == '+')))
*/

static int isRegName(const char* pch)
    {
    char ch = *pch;
    return isUnreserved(ch) || isEscaped(pch) ||
           ch == '$' || ch == ',' || ch == ';' ||
           ch == ':' || ch == '@' || ch == '&' ||
           ch == '=' || ch == '+';
    }
/*
 * userinfo = *( unreserved | escaped | ";" | ":" | "&" | "=" |
 *                      "+" | "$" | "," )
 */

/*
#define IS_USERINFO(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) || \
       ((*(p) == ';')) || ((*(p) == ':')) || ((*(p) == '&')) ||     \
       ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||     \
       ((*(p) == ',')))
*/


static int isUserInfo(const char* pch)
    {
    char ch = *pch;
    return
            isUnreserved(ch) ||
            isEscaped(pch)   ||
            ch == ';' || ch == ':' || ch == '&' ||
            ch == '=' || ch == '+' || ch == '$' ||
            ch == ',';
    }

/*
 * uric = reserved | unreserved | escaped
 */

//#define IS_URIC(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) || (IS_RESERVED(*(p))))

static int isUriC(const char* pch)
    {
    char ch = *pch;
    return isUnreserved(ch) || isEscaped(pch) || isReserved(ch);
    }

/*
* unwise = "{" | "}" | "|" | "\" | "^" | "[" | "]" | "`"
*/

/*
#define IS_UNWISE(p)                                                    \
      (((*(p) == '{')) || ((*(p) == '}')) || ((*(p) == '|')) ||         \
       ((*(p) == '\\'))|| ((*(p) == '^')) || ((*(p) == '[')) ||        \
       ((*(p) == ']')) || ((*(p) == '`')))
*/

static int isUnwise(char ch)
    {
    return
       ch == '{'  || ch == '}' || ch == '|' ||
       ch == '\\' || ch == '^' || ch == '[' ||
       ch == ']'  || ch == '`';
    }
/*
 * Skip to next pointer char, handle escaped sequences
 */

#define NEXT(p) ((*p == '%')? p += 3 : p++)

/*
 * Productions from the spec.
 *
 *    authority     = server | reg_name
 *    reg_name      = 1*( unreserved | escaped | "$" | "," |
 *                        ";" | ":" | "@" | "&" | "=" | "+" )
 *
 * path          = [ abs_path | opaque_part ]
 */

/************************************************************************
 *                                                                      *
 *          Generic URI structure functions                             *
 *                                                                      *
 ************************************************************************/

/**
 * xmlCreateURI:
 *
 * Simply creates an empty xmlURI
 *
 * Returns the new structure or NULL in case of error
 *
 * OOM: possible --> returns NULL , sets OOM flag
 */
XMLPUBFUNEXPORT xmlURIPtr
xmlCreateURI(void) {
    xmlURIPtr ret;

    ret = (xmlURIPtr) xmlMalloc(sizeof(xmlURI));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlCreateURI: out of memory\n"));
        return(NULL);
    }
    memset(ret, 0, sizeof(xmlURI));
    return(ret);
}

/**
 * xmlSaveUri:
 * @param uri pointer to an xmlURI
 *
 * Save the URI as an escaped string
 *
 * Returns a new string (to be deallocated by caller)
 *
 * OOM:
 */
XMLPUBFUNEXPORT xmlChar*
xmlSaveUri(xmlURIPtr uri)
{
    xmlChar* ret;// = NULL;
    const char* p;
    int len;
    int max;
    char ch;

    if (!uri)
        return(NULL);

    max = 80;
    ret = (xmlChar*) xmlMallocAtomic((max + 1) * sizeof(xmlChar));
    if (!ret)
        goto OOM;

    len = 0;

    if (uri->scheme != NULL) {
        p = uri->scheme;
        
        while (*p != 0) {
            if (len >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            ret[len++] = *p++;
        }
        if (len >= max) {
            xmlChar* tmp;
            max *= 2;
            tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
            if (!tmp)
                goto OOM;
            ret = tmp;
        }
        ret[len++] = ':';
    }
    if (uri->opaque != NULL) {
        p = uri->opaque;
        while (*p != 0) {
            if (len + 3 >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            if (isReserved(*p) || isUnreserved(*p))
            {
                ret[len++] = *p++;
            }
            else
            {
                int val = *(unsigned char*)p++;
                int hi = val / 0x10;
                int lo = val % 0x10;
                ret[len++] = '%';
                ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                ret[len++] = lo + (lo > 9? 'A'-10 : '0');
            }
        }
    }
    else
    {
        if (uri->server != NULL) {
            if (len + 3 >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            ret[len++] = '/';
            ret[len++] = '/';
            if (uri->user != NULL) {
                p = uri->user;
                
                while (*p != 0) {
                    if (len + 3 >= max) {
                        xmlChar* tmp;
                        max *= 2;
                        tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                        if (!tmp)
                            goto OOM;
                        ret = tmp;
                    }
                    ch = *p;
                    if (isUnreserved(ch) ||
                        ch == ';' || ch == ':' ||
                        ch == '&' || ch == '=' ||
                        ch == '+' || ch == '$' ||
                        ch == ',')
                    {
                        ret[len++] = *p++;
                    }
                    else
                    {
                        int val = *(unsigned char *)p++;
                        int hi = val / 0x10, lo = val % 0x10;
                        ret[len++] = '%';
                        ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                        ret[len++] = lo + (lo > 9? 'A'-10 : '0');
                    }
                }
                if (len + 3 >= max) {
                    xmlChar* tmp;
                    max *= 2;
                    tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                    if (!tmp)
                        goto OOM;
                    ret = tmp;
                }
                ret[len++] = '@';
            }
            p = uri->server;
            while (*p != 0) {
                if (len >= max) {
                    xmlChar* tmp;
                    max *= 2;
                    tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                    if (!tmp)
                        goto OOM;
                    ret = tmp;
                }
                ret[len++] = *p++;
            }
            if (uri->port > 0) {
                if (len + 10 >= max) {
                    xmlChar* tmp;
                    max *= 2;
                    tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                    if (!tmp)
                        goto OOM;
                    ret = tmp;
                }
                len += snprintf((char*) &ret[len], max - len, ":%d", uri->port);
            }
        }
        else if (uri->authority != NULL) {
            if (len + 3 >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            ret[len++] = '/';
            ret[len++] = '/';
            p = uri->authority;
            while (*p != 0) {
                if (len + 3 >= max) {
                    xmlChar* tmp;
                    max *= 2;
                    tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                    if (!tmp)
                        goto OOM;
                    ret = tmp;
                }
                ch = *p;
                if (isUnreserved(ch) ||
                            ch == '$' || ch == ',' || ch == ';' ||
                            ch == ':' || ch == '@' || ch == '&' ||
                            ch == '=' || ch == '+')
                {
                    ret[len++] = *p++;
                }
                else
                {
                    int val = *(unsigned char *)p++;
                    int hi = val / 0x10;
                    int lo = val % 0x10;
                    ret[len++] = '%';
                    ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                    ret[len++] = lo + (lo > 9? 'A'-10 : '0');
                }
            }
    }
    else if (uri->scheme != NULL) {
        if (len + 3 >= max) {
            xmlChar* tmp;
            max *= 2;
            tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
            if (!tmp)
                goto OOM;
            ret = tmp;
        }
        ret[len++] = '/';
        ret[len++] = '/';
    }
    if (uri->path != NULL) {
        p = uri->path;
        while (*p != 0) {
            if (len + 3 >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            ch = *p;
            if (isUnreserved(ch) || ch == '/' ||
                ch == ';' || ch == '@' || ch == '&' ||
                ch == '=' || ch == '+' || ch == '$' ||
                ch == ',')
            {
                ret[len++] = *p++;
            }
            else
            {
                int val = *(unsigned char *)p++;
                int hi = val / 0x10;
                int lo = val % 0x10;
                ret[len++] = '%';
                ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                ret[len++] = lo + (lo > 9? 'A'-10 : '0');
            }
        }
    }
    if (uri->query != NULL) {
        
        if (len + 3 >= max) {
            xmlChar* tmp;
            max *= 2;
            tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
            if (!tmp)
                goto OOM;
            ret = tmp;
        }
        ret[len++] = '?';
        p = uri->query;
        while (*p != 0) {
            if (len + 3 >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            if (isUnreserved(*p) || isReserved(*p))
            {
                ret[len++] = *p++;
            }
            else
            {
                int val = *(unsigned char *)p++;
                int hi = val / 0x10;
                int lo = val % 0x10;
                ret[len++] = '%';
                ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                ret[len++] = lo + (lo > 9? 'A'-10 : '0');
            }
        }
    }
    }
    if (uri->fragment != NULL) {
        if (len + 3 >= max) {
            xmlChar* tmp;
            max *= 2;
            tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
            if (!tmp)
                goto OOM;
            ret = tmp;
        }
        ret[len++] = '#';
        p = uri->fragment;
        
        while (*p != 0) {
            if (len + 3 >= max) {
                xmlChar* tmp;
                max *= 2;
                tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
                if (!tmp)
                    goto OOM;
                ret = tmp;
            }
            if (isUnreserved(*p) || isReserved(*p))
            {
                ret[len++] = *p++;
            }
            else
            {
                int val = *(unsigned char *)p++;
                int hi = val / 0x10;
                int lo = val % 0x10;
                ret[len++] = '%';
                ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                ret[len++] = lo + (lo > 9? 'A'-10 : '0');
            }
        }
    }
    if (len >= max) {
        xmlChar* tmp;
        max *= 2;
        tmp = (xmlChar*) xmlRealloc(ret, (max + 1) * sizeof(xmlChar)); // DONE: Fix xmlRealloc
        if (!tmp)
            goto OOM;
        ret = tmp;
    }
    ret[len++] = 0;
    return(ret);
//------------------------------
OOM:
    if(ret)
        xmlFree(ret);
    xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlSaveUri: out of memory\n"));
    return(NULL);
}

#ifndef XMLENGINE_EXCLUDE_FILE_FUNC
/**
 * xmlPrintURI:
 * @param stream a FILE* for the output
 * @param uri pointer to an xmlURI
 *
 * Prints the URI in the stream stream.
 */
void
xmlPrintURI(FILE *stream, xmlURIPtr uri) {
    xmlChar *out;

    out = xmlSaveUri(uri);
    if (out != NULL) {
        fprintf(stream, "%s", (char *) out);
        xmlFree(out);
    }
}
#endif

/**
 * xmlCleanURI:
 * @param uri pointer to an xmlURI
 *
 * Make sure the xmlURI struct is free of content
 *
 * OOM: never
 */
static void
xmlCleanURI(xmlURIPtr uri) {
    if (uri == NULL) return;
    
    if (uri->scheme){
        xmlFree(uri->scheme);
        uri->scheme = NULL;
    }
    if (uri->server){
        xmlFree(uri->server);
        uri->server = NULL;
    }
    if (uri->user){
        xmlFree(uri->user);
        uri->user = NULL;
    }
    if (uri->path) {
        xmlFree(uri->path);
        uri->path = NULL;
    }
    if (uri->fragment) {
        xmlFree(uri->fragment);
        uri->fragment = NULL;
    }
    if (uri->opaque) {
        xmlFree(uri->opaque);
        uri->opaque = NULL;
    }
    if (uri->authority) {
        xmlFree(uri->authority);
        uri->authority = NULL;
    }
    if (uri->query) {
        xmlFree(uri->query);
        uri->query = NULL;
    }
}

/**
 * xmlFreeURI:
 * @param uri pointer to an xmlURI
 *
 * Free up the xmlURI struct
 */
XMLPUBFUNEXPORT void
xmlFreeURI(xmlURIPtr uri) {
    if (uri == NULL) return;

    if (uri->scheme) xmlFree(uri->scheme);
    if (uri->server) xmlFree(uri->server);
    if (uri->user  ) xmlFree(uri->user);
    if (uri->path  ) xmlFree(uri->path);
    if (uri->fragment) xmlFree(uri->fragment);
    if (uri->opaque) xmlFree(uri->opaque);
    if (uri->authority) xmlFree(uri->authority);
    if (uri->query) xmlFree(uri->query);
    xmlFree(uri);
}

/************************************************************************
 *                                                                      *
 *          Helper functions                                            *
 *                                                                      *
 ************************************************************************/

/**
 * xmlNormalizeURIPath:
 * @param path pointer to the path string
 *
 * Applies the 5 normalization steps to a path string--that is, RFC 2396
 * Section 5.2, steps 6.c through 6.g.
 *
 * Normalization occurs directly on the string, no new allocation is done
 *
 * Returns 0 or an error code
 */
XMLPUBFUNEXPORT int
xmlNormalizeURIPath(char *path) {
    char *cur, *out;

    if (path == NULL)
        return(-1);

    /* Skip all initial "/" chars.  We want to get to the beginning of the
     * first non-empty segment.
     */
    cur = path;
    while (cur[0] == '/')
      ++cur;
    if (cur[0] == '\0')
      return(0);

    /* Keep everything we've seen so far.  */
    out = cur;

    /*
     * Analyze each segment in sequence for cases (c) and (d).
     */
    while (cur[0] != '\0') {
    /*
     * c) All occurrences of "./", where "." is a complete path segment,
     *    are removed from the buffer string.
     */
    if ((cur[0] == '.') && (cur[1] == '/')) {
        cur += 2;
        /* '//' normalization should be done at this point too */
        while (cur[0] == '/')
        cur++;
        continue;
    }

    /*
     * d) If the buffer string ends with "." as a complete path segment,
     *    that "." is removed.
     */
    if ((cur[0] == '.') && (cur[1] == '\0'))
        break;

    /* Otherwise keep the segment.  */
    while (cur[0] != '/') {
            if (cur[0] == '\0')
              goto done_cd;
        (out++)[0] = (cur++)[0];
    }
    /* nomalize // */
    while ((cur[0] == '/') && (cur[1] == '/'))
        cur++;

        (out++)[0] = (cur++)[0];
    }
 done_cd:
    out[0] = '\0';

    /* Reset to the beginning of the first segment for the next sequence.  */
    cur = path;
    while (cur[0] == '/')
      ++cur;
    if (cur[0] == '\0')
        return(0);

    /*
     * Analyze each segment in sequence for cases (e) and (f).
     *
     * e) All occurrences of "<segment>/../", where <segment> is a
     *    complete path segment not equal to "..", are removed from the
     *    buffer string.  Removal of these path segments is performed
     *    iteratively, removing the leftmost matching pattern on each
     *    iteration, until no matching pattern remains.
     *
     * f) If the buffer string ends with "<segment>/..", where <segment>
     *    is a complete path segment not equal to "..", that
     *    "<segment>/.." is removed.
     *
     * To satisfy the "iterative" clause in (e), we need to collapse the
     * string every time we find something that needs to be removed.  Thus,
     * we don't need to keep two pointers into the string: we only need a
     * "current position" pointer.
     */
    while (1) {
        char *segp, *tmp;

        /* At the beginning of each iteration of this loop, "cur" points to
         * the first character of the segment we want to examine.
         */

        /* Find the end of the current segment.  */
        segp = cur;
        while ((segp[0] != '/') && (segp[0] != '\0'))
          ++segp;

        /* If this is the last segment, we're done (we need at least two
         * segments to meet the criteria for the (e) and (f) cases).
         */
        if (segp[0] == '\0')
          break;

        /* If the first segment is "..", or if the next segment _isn't_ "..",
         * keep this segment and try the next one.
         */
        ++segp;
        if (((cur[0] == '.') && (cur[1] == '.') && (segp == cur+3))
            || ((segp[0] != '.') || (segp[1] != '.')
            || ((segp[2] != '/') && (segp[2] != '\0')))) {
          cur = segp;
          continue;
        }

        /* If we get here, remove this segment and the next one and back up
         * to the previous segment (if there is one), to implement the
         * "iteratively" clause.  It's pretty much impossible to back up
         * while maintaining two pointers into the buffer, so just compact
         * the whole buffer now.
         */

        /* If this is the end of the buffer, we're done.  */
        if (segp[2] == '\0') {
          cur[0] = '\0';
          break;
        }
        /* Valgrind complained, strcpy(cur, segp + 3); */
    /* string will overlap, do not use strcpy */
    tmp = cur;
    segp += 3;
    while ((*tmp++ = *segp++) != 0) {} // NOTE: no loop body here

        /* If there are no previous segments, then keep going from here.  */
        segp = cur;
        while ((segp > path) && ((--segp)[0] == '/'))
          ;
        if (segp == path)
          continue;

        /* "segp" is pointing to the end of a previous segment; find it's
         * start.  We need to back up to the previous segment and start
         * over with that to handle things like "foo/bar/../..".  If we
         * don't do this, then on the first pass we'll remove the "bar/..",
         * but be pointing at the second ".." so we won't realize we can also
         * remove the "foo/..".
         */
        cur = segp;
        while ((cur > path) && (cur[-1] != '/'))
          --cur;
    }
    out[0] = '\0';

    /*
     * g) If the resulting buffer string still begins with one or more
     *    complete path segments of "..", then the reference is
     *    considered to be in error. Implementations may handle this
     *    error by retaining these components in the resolved path (i.e.,
     *    treating them as part of the final URI), by removing them from
     *    the resolved path (i.e., discarding relative levels above the
     *    root), or by avoiding traversal of the reference.
     *
     * We discard them from the final path.
     */
    if (path[0] == '/') {
      cur = path;
      while ((cur[0] == '/') && (cur[1] == '.') && (cur[2] == '.')
             && ((cur[3] == '/') || (cur[3] == '\0')))
    cur += 3;

      if (cur != path) {
    out = path;
    while (cur[0] != '\0')
          (out++)[0] = (cur++)[0];
    out[0] = 0;
      }
    }

    return(0);
}

static int is_hex(char c) {
    if (((c >= '0') && (c <= '9')) ||
        ((c >= 'a') && (c <= 'f')) ||
        ((c >= 'A') && (c <= 'F')))
    return(1);
    return(0);
}

/**
 * xmlURIUnescapeString:
 * @param str the string to unescape
 * @param len the length in bytes to unescape (or <= 0 to indicate full string)
 * @param target optional destination buffer
 *
 * Unescaping routine, does not do validity checks !
 * Output is direct unsigned char translation of %XX values (no encoding)
 *
 * Returns an copy of the string, but unescaped
 *
 * OOM: possible --> sets OOM when returns NULL for target==NULL
 */
XMLPUBFUNEXPORT char*
xmlURIUnescapeString(const char* str, int len, char* target) {
    char* ret;
    char* out;
    const char *in;

    if (!str)
        return(NULL);
    if (len <= 0)
        len = strlen(str);
    if (len < 0)
        return(NULL);

    if (!target) {
        ret = (char*) xmlMallocAtomic(len + 1); // may set OOM
        if (!ret) {
            xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlURIUnescapeString: out of memory\n"));
            return(NULL);
        }
    } else
        ret = target;

    in = str;
    out = ret;
    while(len > 0) {
        if ((*in == '%') && (is_hex(in[1])) && (is_hex(in[2])))
        {
            in++;
            if ((*in >= '0') && (*in <= '9'))
                *out = (*in - '0');
            else if ((*in >= 'a') && (*in <= 'f'))
                *out = (*in - 'a') + 10;
            else if ((*in >= 'A') && (*in <= 'F'))
                *out = (*in - 'A') + 10;
            in++;
            if ((*in >= '0') && (*in <= '9'))
                *out = *out * 16 + (*in - '0');
            else if ((*in >= 'a') && (*in <= 'f'))
                *out = *out * 16 + (*in - 'a') + 10;
            else if ((*in >= 'A') && (*in <= 'F'))
                *out = *out * 16 + (*in - 'A') + 10;
            in++;
            len -= 3;
            out++;
        } else {
            *out++ = *in++;
            len--;
        }
    }
    *out = 0;
    return(ret);
}

/**
 * xmlURIEscapeStr:
 * @param str string to escape
 * @param list exception list string of chars not to escape
 *
 * This routine escapes a string to hex, ignoring reserved characters (a-z)
 * and the characters in the exception list.
 *
 * Returns a new escaped string or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlChar*
xmlURIEscapeStr(const xmlChar* str, const xmlChar* list)
{
    xmlChar* ret;
    xmlChar  ch;
    const xmlChar *in;

    unsigned int len, out;

    if (!str)
        return(NULL);
    len = xmlStrlen(str);
    if (!(len > 0)) return(NULL);

    len += 20;
    ret = (xmlChar*) xmlMallocAtomic(len);
    if (!ret)
        goto OOM;

    in = (const xmlChar *) str;
    out = 0;
    while(*in != 0) {
        if (len - out <= 3) {
            xmlChar* tmp;
            len += 20;
            tmp = (xmlChar*) xmlRealloc(ret, len); // DONE: Fix xmlRealloc
            if (!tmp)
                goto OOM;
            ret = tmp;
        }

        ch = *in;

        if ((ch != '@') && (!isUnreserved(ch)) && (!xmlStrchr(list, ch))) {
            unsigned char val;
            ret[out++] = '%';
            val = ch >> 4;
            if (val <= 9)
                ret[out++] = '0' + val;
            else
                ret[out++] = 'A' + val - 0xA;
            val = ch & 0xF;
            if (val <= 9)
                ret[out++] = '0' + val;
            else
                ret[out++] = 'A' + val - 0xA;
            in++;
        } else {
            ret[out++] = *in++;
        }
    } // while(*in != 0)
    ret[out] = 0;
    return(ret);
//---------------------
OOM:
    if(ret)
        xmlFree(ret);
    xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlURIEscapeStr: out of memory\n"));
    return(NULL);
}

/**
 * xmlURIEscape:
 * @param str the string of the URI to escape
 *
 * Escaping routine, does not do validity checks !
 * It will try to escape the chars needing this, but this is heuristic
 * based it's impossible to be sure.
 *
 * Returns an copy of the string, but escaped
 *
 * 25 May 2001
 * Uses xmlParseURI and xmlURIEscapeStr to try to escape correctly
 * according to RFC2396.
 *   - Carl Douglas
 */
XMLPUBFUNEXPORT xmlChar*
xmlURIEscape(const xmlChar * str)
{
    xmlChar* ret;
    xmlChar* segment;
    xmlURIPtr uri;
    int ret2;

#define NULLCHK(p) if(!p) goto OOM


    if (str == NULL)
        return (NULL);

    uri = xmlCreateURI();
    if (uri != NULL) {
    /*
     * Allow escaping errors in the unescaped form
     */
        uri->cleanup = 1;
        ret2 = xmlParseURIReference(uri, (const char *)str);
        if (ret2) {
            xmlFreeURI(uri);
            return (NULL);
        }
    }

    if (!uri)
        return NULL;

    ret = NULL;

    if (uri->scheme) {
        segment = xmlURIEscapeStr(BAD_CAST uri->scheme, BAD_CAST "+-.");
        NULLCHK(segment);
        ret = xmlStrcat(ret, segment);
        ret = xmlStrcat(ret, BAD_CAST ":");
        xmlFree(segment);
    }

    if (uri->authority) {
        segment = xmlURIEscapeStr(BAD_CAST uri->authority, BAD_CAST "/?;:@");
        NULLCHK(segment);
        ret = xmlStrcat(ret, BAD_CAST "//");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->user) {
        segment = xmlURIEscapeStr(BAD_CAST uri->user, BAD_CAST ";:&=+$,");
        NULLCHK(segment);
        ret = xmlStrcat(ret,BAD_CAST "//");
        ret = xmlStrcat(ret, segment);
        ret = xmlStrcat(ret, BAD_CAST "@");
        xmlFree(segment);
    }

    if (uri->server) {
        segment = xmlURIEscapeStr(BAD_CAST uri->server, BAD_CAST "/?;:@");
        NULLCHK(segment);
        if (uri->user == NULL)
            ret = xmlStrcat(ret, BAD_CAST "//");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->port) {
        xmlChar port[10];

        snprintf((char *) port, 10, "%d", uri->port);
        ret = xmlStrcat(ret, BAD_CAST ":");
        ret = xmlStrcat(ret, port);
    }

    if (uri->path) {
        segment = xmlURIEscapeStr(BAD_CAST uri->path, BAD_CAST ":@&=+$,/?;");
        NULLCHK(segment);
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->query) {
        segment = xmlURIEscapeStr(BAD_CAST uri->query, BAD_CAST ";/?:@&=+,$");
        NULLCHK(segment);
        ret = xmlStrcat(ret, BAD_CAST "?");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->opaque) {
        segment = xmlURIEscapeStr(BAD_CAST uri->opaque, BAD_CAST "");
        NULLCHK(segment);
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->fragment) {
        segment = xmlURIEscapeStr(BAD_CAST uri->fragment, BAD_CAST "#");
        NULLCHK(segment);
        ret = xmlStrcat(ret, BAD_CAST "#");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    xmlFreeURI(uri);
#undef NULLCHK

    return (ret);
//--------------------
OOM:
    xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlURIEscape: out of memory\n"));
    xmlFreeURI(uri);
    if(ret) xmlFree(ret);
    return NULL;
}

/************************************************************************
 *                                                                      *
 *          Escaped URI parsing                                         *
 *                                                                      *
 ************************************************************************/

/**
 * xmlParseURIFragment:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an URI fragment string and fills in the appropriate fields
 * of the uri structure.
 *
 * fragment = *uric
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> returns XML_ERR_NO_MEMORY and sets OOM flag
 */
static int
xmlParseURIFragment(xmlURIPtr uri, const char **str)
{
    const char *cur = *str;

    if (str == NULL)
        return (-1);

    while (isUriC(cur) || isUnwise(*cur))
        NEXT(cur);

    if (uri != NULL) {
        if (uri->fragment != NULL)
            xmlFree(uri->fragment);
        uri->fragment = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM flag
        if(!uri->fragment)
            return XML_ERR_NO_MEMORY;
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIQuery:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse the query part of an URI
 *
 * query = *uric
 *
 * Returns 0 or the error code
 *
 * OOM: possible for uri!=NULL --> returns XML_ERR_NO_MEMORY and sets OOM flag
 */
static int
xmlParseURIQuery(xmlURIPtr uri, const char **str)
{
    const char *cur = *str;

    if (str == NULL)
        return (-1);

    while (isUriC(cur) || (uri && uri->cleanup && isUnwise(*cur)))
        NEXT(cur);
    if (uri != NULL) {
        if (uri->query != NULL)
            xmlFree(uri->query);
        uri->query = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM flag
        if(!uri->query)
            return XML_ERR_NO_MEMORY;
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIScheme:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an URI scheme
 *
 * scheme = alpha *( alpha | digit | "+" | "-" | "." )
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIScheme(xmlURIPtr uri, const char **str) {
    const char *cur;

    if (str == NULL)
        return(-1);

    cur = *str;
    if (!IS_ALPHA(*cur))
        return(1); // was 2 which is XML_ERR_NO_MEMORY
    cur++;
    while (IS_SCHEME(*cur))
        cur++;
    if (uri != NULL) {
        if (uri->scheme)
            xmlFree(uri->scheme);
        /* !!! strndup */
        uri->scheme = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM, when returns NULL
        if(!uri->scheme)
            return XML_ERR_NO_MEMORY;
    }
    *str = cur;
    return(0);
}

/**
 * xmlParseURIOpaquePart:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an URI opaque part
 *
 * opaque_part = uric_no_slash *uric
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag and returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIOpaquePart(xmlURIPtr uri, const char **str)
{
    const char* cur;

    if (str == NULL)
        return (-1);

    cur = *str;
    if (!(isUriCNoSlash(cur) ||
        ( (uri != NULL) && (uri->cleanup) && (isUnwise(*cur)) )
        ))
    {
        return (3);
    }
    NEXT(cur);
    while (isUriC(cur) || ((uri != NULL) && (uri->cleanup) && (isUnwise(*cur))))
    {
        NEXT(cur);
    }
    if (uri != NULL) {
        if (uri->opaque != NULL)
            xmlFree(uri->opaque);
        uri->opaque = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM flag
        if(!uri->opaque)
            return XML_ERR_NO_MEMORY;
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIServer:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse a server subpart of an URI, it's a finer grain analysis
 * of the authority part.
 *
 * server        = [ [ userinfo "@" ] hostport ]
 * userinfo      = *( unreserved | escaped |
 *                       ";" | ":" | "&" | "=" | "+" | "$" | "," )
 * hostport      = host [ ":" port ]
 * host          = hostname | IPv4address
 * hostname      = *( domainlabel "." ) toplabel [ "." ]
 * domainlabel   = alphanum | alphanum *( alphanum | "-" ) alphanum
 * toplabel      = alpha | alpha *( alphanum | "-" ) alphanum
 * IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit
 * port          = *digit
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag and returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIServer(xmlURIPtr uri, const char **str) {
    const char* cur;
    const char* host;
    const char* tmp;
    const int IPmax = 4;
    int oct;

    if (str == NULL)
        return(-1);

    cur = *str;

    /*
     * is there an userinfo ?
     */
    while (isUserInfo(cur))
    {
        NEXT(cur);
    }

    if (*cur == '@')
    {
        if (uri != NULL) {
            if (uri->user != NULL)
                xmlFree(uri->user);
            uri->user = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM flag
            if(!uri->user)
                return XML_ERR_NO_MEMORY;
        }
        cur++;
    } else {
        if (uri != NULL) {
            if (uri->user != NULL)
                xmlFree(uri->user);
            uri->user = NULL;
        }
        cur = *str;
    }
    /*
     * This can be empty in the case where there is no server
     */
    host = cur;
    if (*cur == '/') {
        if (uri != NULL) {
            if (uri->authority != NULL)
                xmlFree(uri->authority);
            uri->authority = NULL;
            if (uri->server != NULL)
                xmlFree(uri->server);
            uri->server = NULL;
            uri->port = 0;
        }
        return(0);
    }
    /*
     * host part of hostport can derive either an IPV4 address
     * or an unresolved name. Check the IP first, it easier to detect
     * errors if wrong one
     */
    for (oct = 0; oct < IPmax; ++oct) {
        if (*cur == '.')
            return(3); /* e.g. http://.xml/ or http://18.29..30/ */
        while(IS_DIGIT(*cur))
            cur++;
        if (oct == (IPmax-1))
            continue;
        if (*cur != '.')
            break;
        cur++;
    }
    if (oct < IPmax || (*cur == '.' && cur++) || IS_ALPHA(*cur)) {
        /* maybe host_name */
        if (!IS_ALPHANUM(*cur))
            return(4); /* e.g. http://xml.$oft */
        do {
            do ++cur; while (IS_ALPHANUM(*cur));
            if (*cur == '-') {
                --cur;
                if (*cur == '.')
                    return(5); /* e.g. http://xml.-soft */
                ++cur;
                continue;
            }
            if (*cur == '.') {
                --cur;
                if (*cur == '-')
                    return(6); /* e.g. http://xml-.soft */
                if (*cur == '.')
                    return(7); /* e.g. http://xml..soft */
                ++cur;
                continue;
            }
            break;
        } while (1);

        tmp = cur;
        if (tmp[-1] == '.')
            --tmp; /* e.g. http://xml.$Oft/ */
        do --tmp; while (tmp >= host && IS_ALPHANUM(*tmp));
        if ((++tmp == host || tmp[-1] == '.') && !IS_ALPHA(*tmp))
            return(8); /* e.g. http://xmlsOft.0rg/ */
    }
    if (uri != NULL) {
        if (uri->authority != NULL)
            xmlFree(uri->authority);
        uri->authority = NULL;
        if (uri->server != NULL)
            xmlFree(uri->server);
        uri->server = xmlURIUnescapeString(host, cur - host, NULL); // may set OOM flag
        if(!uri->server)
            return XML_ERR_NO_MEMORY;
    }
    /*
     * finish by checking for a port presence.
     */
    if (*cur == ':') {
        cur++;
        if (IS_DIGIT(*cur)) {
            if (uri != NULL)
                uri->port = 0;
            while (IS_DIGIT(*cur)) {
                if (uri != NULL)
                    uri->port = uri->port * 10 + (*cur - '0');
                cur++;
            }
        }
    }
    *str = cur;
    return(0);
}

/**
 * xmlParseURIRelSegment:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an URI relative segment
 *
 * rel_segment = 1*( unreserved | escaped | ";" | "@" | "&" | "=" |
 *                          "+" | "$" | "," )
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM and returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIRelSegment(xmlURIPtr uri, const char **str)
{
    const char *cur;

    if (str == NULL)
        return (-1);

    cur = *str;
    if (!(isSegment(cur) || ((uri) && (uri->cleanup) && (isUnwise(*cur))))) {
        return (3);
    }
    NEXT(cur);
    while (isSegment(cur) || ((uri) && (uri->cleanup) && (isUnwise(*cur))))
        NEXT(cur);
    if (uri != NULL) {
        if (uri->path != NULL)
            xmlFree(uri->path);
        uri->path = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM flag
        if(!uri->path)
            return XML_ERR_NO_MEMORY;
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIPathSegments:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 * @param slash should we add a leading slash
 *
 * Parse an URI set of path segments
 *
 * path_segments = segment *( "/" segment )
 * segment       = *pchar *( ";" param )
 * param         = *pchar
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag and returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIPathSegments(xmlURIPtr uri, const char **str, int slash)
{
    const char *cur;

    if (str == NULL)
        return (-1);

    cur = *str;
// XMLENGINE: BEGIN REPLACE
    for(;;) {
    // Replace while(1) for infinite loop
    // do {
// XMLENGINE: END REPLACE
        
        while (isPChar(cur) || (uri && uri->cleanup && (isUnwise(*cur))))
        {
            NEXT(cur);
        }
        while (*cur == ';') {
            cur++;
            while (isPChar(cur) || ((uri != NULL) && (uri->cleanup) && (isUnwise(*cur))))
                NEXT(cur);
        }
        if (*cur != '/')
            break;
        cur++;
// XMLENGINE: BEGIN REPLACE
    }
    //} while (1);
// XMLENGINE: END REPLACE
    if (uri != NULL) {
        int len, len2 = 0;
        char *path;

        /*
         * Concat the set of path segments to the current path
         */
        len = cur - *str;
        if (slash)
            len++;

        if (uri->path != NULL) {
            len2 = strlen(uri->path);
            len += len2;
        }
        path = (char *) xmlMallocAtomic(len + 1); // may set OOM flag
        if (path == NULL) {
            xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlParseURIPathSegments: out of memory\n"));
            *str = cur;
            //return (-1);
            return XML_ERR_NO_MEMORY;
        }
        if (uri->path != NULL)
            memcpy(path, uri->path, len2);
        if (slash) {
            path[len2] = '/';
            len2++;
        }
        path[len2] = 0;
        if (cur - *str > 0)
            xmlURIUnescapeString(*str, cur - *str, &path[len2]); // MAY NOT set OOM flag !!!
        if (uri->path != NULL)
            xmlFree(uri->path);
        uri->path = path;
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIAuthority:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse the authority part of an URI.
 *
 * authority = server | reg_name
 * server    = [ [ userinfo "@" ] hostport ]
 * reg_name  = 1*( unreserved | escaped | "$" | "," | ";" | ":" |
 *                        "@" | "&" | "=" | "+" )
 *
 * Note : this is completely ambiguous since reg_name is allowed to
 *        use the full set of chars in use by server:
 *
 *        3.2.1. Registry-based Naming Authority
 *
 *        The structure of a registry-based naming authority is specific
 *        to the URI scheme, but constrained to the allowed characters
 *        for an authority component.
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> OOM flag is set, returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIAuthority(xmlURIPtr uri, const char** str) {
    const char *cur;
    int ret;

    if (str == NULL)
        return(-1);

    cur = *str;

    /*
     * try first to parse it as a server string.
     */
    ret = xmlParseURIServer(uri, str); // may set OOM flag -- returns XML_ERR_NO_MEMORY
    if ((ret == 0) && (*str != NULL) &&
        ((**str == 0) || (**str == '/') || (**str == '?')))
        return(0);
    if(ret!=0)
        return ret; // error happened, maybe OOM
    *str = cur;

    /*
     * failed, fallback to reg_name
     */
    if (!isRegName(cur)) {
        return(5);
    }
    NEXT(cur);
    while (isRegName(cur))
        NEXT(cur);

    if (uri != NULL) {
        if (uri->server != NULL)
            xmlFree(uri->server);
        uri->server = NULL;
        if (uri->user != NULL)
            xmlFree(uri->user);
        uri->user = NULL;
        if (uri->authority != NULL)
            xmlFree(uri->authority);
        uri->authority = xmlURIUnescapeString(*str, cur - *str, NULL); // may set OOM
        if(!uri->authority)
            return XML_ERR_NO_MEMORY;
    }
    *str = cur;
    return(0);
}

/**
 * xmlParseURIHierPart:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an URI hierarchical part
 *
 * hier_part = ( net_path | abs_path ) [ "?" query ]
 * abs_path = "/"  path_segments
 * net_path = "//" authority [ abs_path ]
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag and returns XML_ERR_NO_MEMORY
 */
static int
xmlParseURIHierPart(xmlURIPtr uri, const char** str) {
    int ret;
    const char* cur;

    if (!str)
        return(-1);

    cur = *str;

    if ((cur[0] == '/') && (cur[1] == '/'))
    {
        cur += 2;
        ret = xmlParseURIAuthority(uri, &cur); // may set OOM flag and return XML_ERR_NO_MEMORY
        if (ret != 0)
            return(ret);

        if (cur[0] == '/') {
            cur++;
            ret = xmlParseURIPathSegments(uri, &cur, 1);
        }
    } else if (cur[0] == '/') {
        cur++;
        ret = xmlParseURIPathSegments(uri, &cur, 1);
    } else {
        return(4);
    }
    if (ret != 0)
        return(ret); // there was error, maybe OOM

    if (*cur == '?') {
        cur++;
        ret = xmlParseURIQuery(uri, &cur); // may set OOM flag
        if (ret != 0)
            return(ret);
    }
    *str = cur;
    return(0);
}

/**
 * xmlParseAbsoluteURI:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an URI reference string and fills in the appropriate fields
 * of the uri structure
 *
 * absoluteURI   = scheme ":" ( hier_part | opaque_part )
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM and returns XML_ERR_NO_MEMORY
 */
static int
xmlParseAbsoluteURI(xmlURIPtr uri, const char** str) {
    int ret;
    const char* cur;

    if (str == NULL)
        return(-1);

    cur = *str;

    ret = xmlParseURIScheme(uri, str); // may set OOM and return XML_ERR_NO_MEMORY
    if (ret != 0) return(ret);

    if (**str != ':') {
        *str = cur;
        return(1);
    }
    (*str)++;
    if (**str == '/')
        return(xmlParseURIHierPart(uri, str)); // may set OOM flag
    return(xmlParseURIOpaquePart(uri, str)); //may set OOM flag
}

/**
 * xmlParseRelativeURI:
 * @param uri pointer to an URI structure
 * @param str pointer to the string to analyze
 *
 * Parse an relative URI string and fills in the appropriate fields
 * of the uri structure
 *
 * relativeURI = ( net_path | abs_path | rel_path ) [ "?" query ]
 * abs_path = "/"  path_segments
 * net_path = "//" authority [ abs_path ]
 * rel_path = rel_segment [ abs_path ]
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag and return XML_ERR_NO_MEMORY
 */
static int
xmlParseRelativeURI(xmlURIPtr uri, const char **str) {
    int ret = 0;
    const char *cur;

    if (str == NULL)
        return(-1);

    cur = *str;
    if ((cur[0] == '/') && (cur[1] == '/')) {
        cur += 2;

        ret = xmlParseURIAuthority(uri, &cur); // may set OOM flag
        if (ret != 0)
            return(ret);
        if (cur[0] == '/') {
            cur++;
            ret = xmlParseURIPathSegments(uri, &cur, 1); // may set OOM flag
        }
    } else if (cur[0] == '/') {
        cur++;
        ret = xmlParseURIPathSegments(uri, &cur, 1); // may set OOM flag
    } else if (cur[0] != '#' && cur[0] != '?') {
        ret = xmlParseURIRelSegment(uri, &cur); // may set OOM flag
        if (ret != 0)
            return(ret);
        if (cur[0] == '/') {
            cur++;
            ret = xmlParseURIPathSegments(uri, &cur, 1); // may set OOM flag
        }
    }
    if (ret != 0)
        return(ret);
    if (*cur == '?') {
        cur++;
        ret = xmlParseURIQuery(uri, &cur); // may set OOM flag
        if (ret != 0)
            return(ret);
    }
    *str = cur;
    return(ret);
}

/**
 * xmlParseURIReference:
 * @param uri pointer to an URI structure
 * @param str the string to analyze
 *
 * Parse an URI reference string and fills in the appropriate fields
 * of the uri structure
 *
 * URI-reference = [ absoluteURI | relativeURI ] [ "#" fragment ]
 *
 * Returns 0 or the error code
 *
 * OOM: possible --> sets OOM flag and returns XML_ERR_NO_MEMORY
 */
XMLPUBFUNEXPORT int
xmlParseURIReference(xmlURIPtr uri, const char *str) {
    int ret;
    const char *tmp = str;

    if (!str)
        return(-1);
    xmlCleanURI(uri);

    /*
     * Try first to parse absolute refs, then fallback to relative if
     * it fails.
     */
    ret = xmlParseAbsoluteURI(uri, &str); // may set OOM flag
    if (ret == XML_ERR_NO_MEMORY)
        return ret;

    if (ret != 0) { // No, it is not an absolute URI, try it as a relative one...
        xmlCleanURI(uri);
        str = tmp;
        ret = xmlParseRelativeURI(uri, &str); // may set OOM flag
        if (ret == XML_ERR_NO_MEMORY)
            return ret;
    }

    if (ret != 0) {
        xmlCleanURI(uri);
        return(ret);
    }

    if (*str == '#') {
        str++;
        ret = xmlParseURIFragment(uri, &str); // may set OOM flag
        if (ret != 0) return(ret);
    }
    if (*str != 0) {
        xmlCleanURI(uri);
        return(1);
    }
    return(0);
}

/**
 * xmlParseURI:
 * @param str the URI string to analyze
 *
 * Parse an URI
 *
 * URI-reference = [ absoluteURI | relativeURI ] [ "#" fragment ]
 *
 * Returns a newly build xmlURIPtr or NULL in case of error
 *
 * OOM: possible --> returns NULL for uri!=NULL
 */
XMLPUBFUNEXPORT xmlURIPtr
xmlParseURI(const char *str)
{
    xmlURIPtr uri;
    int ret;

    if (!str)
        return(NULL);
    uri = xmlCreateURI();
    if (uri) {
        ret = xmlParseURIReference(uri, str);
        if (ret) {
            xmlCleanURI(uri);
            xmlFreeURI(uri);
            return(NULL);
        }
    }
    return(uri);
}

/************************************************************************
 *                                                                      *
 *          Public functions                                            *
 *                                                                      *
 ************************************************************************/

/**
 * xmlBuildURI:
 * @param URI the URI instance found in the document
 * @param base the base value
 *
 * Computes he final URI of the reference done by checking that
 * the given URI is valid, and building the final URI using the
 * base URI. This is processed according to section 5.2 of the
 * RFC 2396
 *
 * 5.2. Resolving Relative References to Absolute Form
 *
 * Returns a new URI string (to be freed by the caller) or NULL in case
 *         of error.
 *
 * OOM:
 */
XMLPUBFUNEXPORT xmlChar*
xmlBuildURI(const xmlChar *URI, const xmlChar *base) {
    xmlChar *val = NULL;
    int ret, len, indx, cur, out;
    xmlURIPtr ref = NULL;
    xmlURIPtr bas = NULL;
    xmlURIPtr res = NULL;

    /*
     * 1) The URI reference is parsed into the potential four components and
     *    fragment identifier, as described in Section 4.3.
     *
     *    NOTE that a completely empty URI is treated by modern browsers
     *    as a reference to "." rather than as a synonym for the current
     *    URI.  Should we do that here?
     */
    if (!URI)
        ret = -1;
    else {
        if (*URI) {
            ref = xmlCreateURI();
            if (ref == NULL)
                goto done; 
            ret = xmlParseURIReference(ref, (const char*) URI);
        }
        else
            ret = 0;
    }
    if (ret != 0)
        goto done;
    if ((ref != NULL) && (ref->scheme != NULL)) {
        /*
         * The URI is absolute don't modify.
         */
        val = xmlStrdup(URI); 
        goto done;
    }
    if (base == NULL)
        ret = -1;
    else {
        bas = xmlCreateURI();
        if (bas == NULL)
            goto done;
        ret = xmlParseURIReference(bas, (const char *) base);
    }
    if (ret != 0) {
        if (ref)
            val = xmlSaveUri(ref); 
        goto done;
    }
    if (ref == NULL) {
        /*
         * the base fragment must be ignored
         */
        if (bas->fragment != NULL) {
            xmlFree(bas->fragment);
            bas->fragment = NULL;
        }
        val = xmlSaveUri(bas);
        goto done;
    }

    /*
     * 2) If the path component is empty and the scheme, authority, and
     *    query components are undefined, then it is a reference to the
     *    current document and we are done.  Otherwise, the reference URI's
     *    query and fragment components are defined as found (or not found)
     *    within the URI reference and not inherited from the base URI.
     *
     *    NOTE that in modern browsers, the parsing differs from the above
     *    in the following aspect:  the query component is allowed to be
     *    defined while still treating this as a reference to the current
     *    document.
     */
    res = xmlCreateURI();
    if (res == NULL)
        goto done;
    if ((ref->scheme == NULL)   &&
        (ref->path == NULL)     &&
        (ref->authority == NULL)&&
        (ref->server == NULL))
    {   
        if (bas->scheme != NULL)
            res->scheme = xmlMemStrdup(bas->scheme);
        if (bas->authority != NULL)
            res->authority = xmlMemStrdup(bas->authority);
        else if (bas->server != NULL) {
            res->server = xmlMemStrdup(bas->server);
            if (bas->user != NULL)
                res->user = xmlMemStrdup(bas->user);
            res->port = bas->port;
        }
        if (bas->path != NULL)
            res->path = xmlMemStrdup(bas->path);
        if (ref->query != NULL)
            res->query = xmlMemStrdup(ref->query);
        else if (bas->query != NULL)
            res->query = xmlMemStrdup(bas->query);
        if (ref->fragment != NULL)
            res->fragment = xmlMemStrdup(ref->fragment);
        goto step_7;
    }

    /*
     * 3) If the scheme component is defined, indicating that the reference
     *    starts with a scheme name, then the reference is interpreted as an
     *    absolute URI and we are done.  Otherwise, the reference URI's
     *    scheme is inherited from the base URI's scheme component.
     */
    if (ref->scheme != NULL) {
        val = xmlSaveUri(ref);
        goto done;
    }
    if (bas->scheme != NULL)
        res->scheme = xmlMemStrdup(bas->scheme);

    if (ref->query != NULL)
        res->query = xmlMemStrdup(ref->query);
    if (ref->fragment != NULL)
        res->fragment = xmlMemStrdup(ref->fragment);

    /*
     * 4) If the authority component is defined, then the reference is a
     *    network-path and we skip to step 7.  Otherwise, the reference
     *    URI's authority is inherited from the base URI's authority
     *    component, which will also be undefined if the URI scheme does not
     *    use an authority component.
     */
    if ((ref->authority != NULL) || (ref->server != NULL)) {
        if (ref->authority != NULL)
            res->authority = xmlMemStrdup(ref->authority);
        else {
            res->server = xmlMemStrdup(ref->server);
            if (ref->user != NULL)
            res->user = xmlMemStrdup(ref->user);
                res->port = ref->port;
        }
        if (ref->path != NULL)
            res->path = xmlMemStrdup(ref->path);
        goto step_7;
     }
     if (bas->authority != NULL)
        res->authority = xmlMemStrdup(bas->authority);
     else if (bas->server != NULL) {
            res->server = xmlMemStrdup(bas->server);
            if (bas->user != NULL)
                res->user = xmlMemStrdup(bas->user);
            res->port = bas->port;
        }

    /*
     * 5) If the path component begins with a slash character ("/"), then
     *    the reference is an absolute-path and we skip to step 7.
     */
    if ((ref->path != NULL) && (ref->path[0] == '/')) {
        res->path = xmlMemStrdup(ref->path);
        goto step_7;
    }


    /*
     * 6) If this step is reached, then we are resolving a relative-path
     *    reference.  The relative path needs to be merged with the base
     *    URI's path.  Although there are many ways to do this, we will
     *    describe a simple method using a separate string buffer.
     *
     * Allocate a buffer large enough for the result string.
     */
    len = 2; /* extra / and 0 */
    if (ref->path != NULL)
        len += strlen(ref->path);
    if (bas->path != NULL)
        len += strlen(bas->path);
    res->path = (char *) xmlMallocAtomic(len);
    if (res->path == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlBuildURI: out of memory\n"));
        goto done;
    }
    res->path[0] = 0;

    /*
     * a) All but the last segment of the base URI's path component is
     *    copied to the buffer.  In other words, any characters after the
     *    last (right-most) slash character, if any, are excluded.
     */
    cur = 0;
    out = 0;
    if (bas->path != NULL) {
        while (bas->path[cur] != 0) {
            while ((bas->path[cur] != 0) && (bas->path[cur] != '/'))
                cur++;
            if (bas->path[cur] == 0)
                break;

            cur++;
            while (out < cur) {
                res->path[out] = bas->path[out];
                out++;
            }
        }
    }
    res->path[out] = 0;

    /*
     * b) The reference's path component is appended to the buffer
     *    string.
     */
    if (ref->path != NULL && ref->path[0] != 0) {
        indx = 0;
        /*
         * Ensure the path includes a '/'
         */
        if ((out == 0) && (bas->server != NULL))
            res->path[out++] = '/';
        while (ref->path[indx] != 0) {
            res->path[out++] = ref->path[indx++];
        }
    }
    res->path[out] = 0;

    /*
     * Steps c) to h) are really path normalization steps
     */
    xmlNormalizeURIPath(res->path);

step_7:

    /*
     * 7) The resulting URI components, including any inherited from the
     *    base URI, are recombined to give the absolute form of the URI
     *    reference.
     */
    val = xmlSaveUri(res);

done:
    if (ref)
        xmlFreeURI(ref);
    if (bas)
        xmlFreeURI(bas);
    if (res)
        xmlFreeURI(res);
    return(val);
}

/**
 * xmlCanonicPath:
 * @param path the resource locator in a filesystem notation
 *
 * Constructs a canonic path from the specified path.
 *
 * Returns a new canonic path, or a duplicate of the path parameter if the
 * construction fails. The caller is responsible for freeing the memory occupied
 * by the returned string. If there is insufficient memory available, or the
 * argument is NULL, the function returns NULL.
 *
 * OOM:
 */
#define IS_WINDOWS_PATH(p)                          \
    ((p != NULL) &&                                 \
     (((p[0] >= 'a') && (p[0] <= 'z')) ||           \
      ((p[0] >= 'A') && (p[0] <= 'Z'))) &&          \
     (p[1] == ':') && ((p[2] == '/') || (p[2] == '\\')))
//ISSUE: Not finished reviewing for OOM handling / O.K.: 16.05.05
XMLPUBFUNEXPORT xmlChar*
xmlCanonicPath(const xmlChar *path)
{
#if (defined(_WIN32)||defined(__SYMBIAN32__)) && !defined(__CYGWIN__)
    int len = 0;
    //int i = 0;
    xmlChar *p = NULL;
#endif
    xmlChar *ret;
    xmlURIPtr uri;

    // DO NOT REMOVE this check
    if (path == NULL)
        return(NULL);

    if ((uri = xmlParseURI((const char *) path)) != NULL)
    {
        xmlFreeURI(uri);
        return xmlStrdup(path); 
    }
    // It's should be OOM already!!! // ISSUE: Not finished work / OK: 16.05.05
    uri = xmlCreateURI();
    if (uri == NULL) {
        return(NULL);
    }

#if (defined(_WIN32)||defined(__SYMBIAN32__)) && !defined(__CYGWIN__)
    len = xmlStrlen(path);
    if ((len > 2) && IS_WINDOWS_PATH(path)) {
        uri->scheme = (char*) xmlStrdup(BAD_CAST "file");
        uri->path = (char*) xmlMallocAtomic(len + 2);
        uri->path[0] = '/';
        p = (xmlChar*) uri->path + 1;
        strncpy((char*)p, (char*)path, len + 1);
    } else {
        uri->path = (char*) xmlStrdup(path);
        p = (xmlChar*) uri->path;
    }
    while (*p != '\0') {
        if (*p == '\\')
            *p = '/';
        p++;
    }
#else
    uri->path = (char *) xmlStrdup((const xmlChar *) path); 
#endif

    ret = xmlSaveUri(uri);
    xmlFreeURI(uri);
    return(ret);
}
