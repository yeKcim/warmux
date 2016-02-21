/*
 * libxml2_tree.c : implementation of access function for an XML tree.
 *
 * References:
 *   XHTML 1.0 W3C REC: http://www.w3.org/TR/2002/REC-xhtml1-20020801/
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 *
 */
#pragma warning(disable: 4127 4132)

#define IN_LIBXML

#include "xmlenglibxml.h"

#include <stdlib.h>
#include <string.h> /* for memset() only ! */


#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include <libxml2_globals.h>
#include <libxml2_uri.h>
#include <libxml2_parserinternals.h>
#include "libxml2_xmlerror2.h"

#ifdef LIBXML_HTML_ENABLED
#include "libxml2_htmltree.h"
#endif


xmlNsPtr xmlNewReconciliedNs(xmlDocPtr doc, xmlNodePtr tree, xmlNsPtr ns);

/************************************************************************
 *                                                                      *
 *      Tree memory error handler                                       *
 *                                                                      *
 ************************************************************************/
/**
 * xmlTreeErrMemory:
 * @param extra extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlTreeErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_TREE, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * xmlTreeErr:
 * @param code the error number
 * @param extra extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlTreeErr(int code, xmlNodePtr node, const char *extra)
{
    const char *msg = NULL;

    switch(code) {
        case XML_TREE_INVALID_HEX:
        msg = EMBED_ERRTXT("invalid hexadecimal character value");
        break;
    case XML_TREE_INVALID_DEC:
        msg = "invalid decimal character value";
        break;
    case XML_TREE_UNTERMINATED_ENTITY:
        msg = EMBED_ERRTXT("unterminated entity reference %15s");
        break;
    default:
        msg = EMBED_ERRTXT("unexpected error number");
    }
    __xmlSimpleError(XML_FROM_TREE, code, node, msg, extra);
}


/************************************************************************
 *                                                                      *
 *      A few static variables and macros                               *
 *                                                                      *
 ************************************************************************/


#define UPDATE_LAST_CHILD_AND_PARENT(n)                             \
    if ((n) != NULL) {                                              \
        xmlNodePtr ulccur = (n)->children;                          \
        if (ulccur == NULL) {                                       \
            (n)->last = NULL;                                       \
        } else {                                                    \
            while (ulccur->next != NULL) {                          \
                ulccur->parent = (n);                               \
            ulccur = ulccur->next;                                  \
        }                                                           \
        ulccur->parent = (n);                                       \
        (n)->last = ulccur;                                         \
    }}

/* #define DEBUG_BUFFER */
/* #define DEBUG_TREE */

/************************************************************************
 *                                                                      *
 *      Functions to move to entities.c once the                        *
 *      API freeze is smoothen and they can be made public.             *
 *                                                                      *
 ************************************************************************/
//#include "libxml/Libxml2_hash.h" // included via parser.h

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlGetEntityFromDtd:
 * @param dtd A pointer to the DTD to search
 * @param name The entity name
 *
 * Do an entity lookup in the DTD entity hash table and
 * return the corresponding entity, if found.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetEntityFromDtd(xmlDtdPtr dtd, const xmlChar *name) {
    xmlEntitiesTablePtr table;

    if((dtd != NULL) && (dtd->entities != NULL)) {
        table = (xmlEntitiesTablePtr) dtd->entities;
        return((xmlEntityPtr) xmlHashLookup(table, name));
        /* return(xmlGetEntityFromTable(table, name)); */
    }
    return(NULL);
}
/**
 * xmlGetParameterEntityFromDtd:
 * @param dtd A pointer to the DTD to search
 * @param name The entity name
 *
 * Do an entity lookup in the DTD pararmeter entity hash table and
 * return the corresponding entity, if found.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetParameterEntityFromDtd(xmlDtdPtr dtd, const xmlChar *name) {
    xmlEntitiesTablePtr table;

    if ((dtd != NULL) && (dtd->pentities != NULL)) {
    table = (xmlEntitiesTablePtr) dtd->pentities;
    return((xmlEntityPtr) xmlHashLookup(table, name));
    /* return(xmlGetEntityFromTable(table, name)); */
    }
    return(NULL);
}
#endif /* LIBXML_TREE_ENABLED */

/************************************************************************
 *                                                                      *
 *          QName handling helper                                       *
 *                                                                      *
 ************************************************************************/

/**
 * xmlBuildQName:
 * @param ncname the Name
 * @param prefix the prefix
 * @param memory preallocated memory
 * @param len preallocated memory length
 *
 * Builds the QName prefix:ncname in memory if there is enough space
 * and prefix is not NULL nor empty, otherwise allocate a new string.
 * If prefix is NULL or empty it returns ncname.
 *
 * Returns the new string which must be freed by the caller if different from
 *         memory and ncname or NULL in case of error
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlChar*
xmlBuildQName(const xmlChar* ncname, const xmlChar* prefix,
              xmlChar* memory, int len)
{
    int lenn, lenp;
    xmlChar* ret;

    if (!ncname || !prefix)
        return((xmlChar*)ncname);

    lenn = strlen((char*) ncname);
    lenp = strlen((char*) prefix);

    if (!memory || (len < lenn + lenp + 2))
    {
        ret = (xmlChar*) xmlMallocAtomic(lenn + lenp + 2);
        if (!ret) {
            xmlTreeErrMemory(EMBED_ERRTXT("building QName"));
            return(ret);
        }
    } else {
        ret = memory;
    }
    memcpy(&ret[0], prefix, lenp);
    ret[lenp] = ':';
    memcpy(&ret[lenp + 1], ncname, lenn);
    ret[lenn + lenp + 1] = 0;
    return(ret);
}

/**
 * xmlSplitQName2:
 * @param name the full QName
 * @param prefix a xmlChar **
 *
 * parse an XML qualified name string
 *
 * [NS 5] QName ::= (Prefix ':')? LocalPart
 *
 * [NS 6] Prefix ::= NCName
 *
 * [NS 7] LocalPart ::= NCName
 *
 * Returns NULL if not a QName, otherwise the local part, and prefix
 *   is updated to get the Prefix if any.
 *
 * OOM: possible --> when returns NULL and OOM flag is set
 */

XMLPUBFUNEXPORT xmlChar*
xmlSplitQName2(const xmlChar *name, xmlChar **prefix)
{
    int len;
    xmlChar* ret; // = NULL;

    *prefix = NULL;
    if (!name)
        return(NULL);

#ifndef XML_XML_NAMESPACE
    /* xml: prefix is not really a namespace */
    if ((name[0] == 'x') && (name[1] == 'm') &&
        (name[2] == 'l') && (name[3] == ':'))
        return(NULL);
#endif

    /* nasty but valid */
    if (name[0] == ':')
        return(NULL);

    /*
     * we are not trying to validate but just to cut, and yes it will
     * work even if this is as set of UTF-8 encoded chars
     */
    len = 0;
    while ((name[len] != 0) && (name[len] != ':'))
        len++;

    if (name[len] == 0)
        return(NULL);

    *prefix = xmlStrndup(name, len);
    if (!*prefix)
        goto OOM_exit;

    ret = xmlStrdup(&name[len + 1]);
    if (!ret) {
        if (*prefix) {
            xmlFree(*prefix);
            *prefix = NULL;
        }
OOM_exit:
        xmlTreeErrMemory(EMBED_ERRTXT("QName split"));
        return(NULL);
    }

    return(ret);
}

/**
 * xmlSplitQName3:
 * @param name the full QName
 * @param len an int *
 *
 * parse an XML qualified name string,i
 *
 * returns NULL if it is not a Qualified Name, otherwise, update len
 *         with the lenght in byte of the prefix and return a pointer
 */

XMLPUBFUNEXPORT const xmlChar *
xmlSplitQName3(const xmlChar *name, int *len) {
    int i = 0;

    if (name == NULL) return(NULL);
    if (len == NULL) return(NULL);

    /* nasty but valid */
    if (name[0] == ':')
        return(NULL);

    /*
     * we are not trying to validate but just to cut, and yes it will
     * work even if this is as set of UTF-8 encoded chars
     */
    while ((name[i] != 0) && (name[i] != ':'))
        i++;

    if (name[i] == 0)
        return(NULL);

    *len = i;

    return(&name[i+1]);
}

/************************************************************************
 *                                                                      *
 *      Check Name, NCName and QName strings                            *
 *                                                                      *
 ************************************************************************/

#define CUR_SCHAR(s, l) xmlStringCurrentChar(NULL, s, &l)

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_XPATH_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlValidateNCName:
 * @param value the value to check
 * @param space allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of NCName
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
XMLPUBFUNEXPORT int
xmlValidateNCName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    /*
     * First quick algorithm for ASCII range
     */
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) ||
        ((*cur >= 'A') && (*cur <= 'Z')) ||
        (*cur == '_'))
        cur++;
    else
        goto try_complex;

    while (((*cur >= 'a') && (*cur <= 'z')) ||
        ((*cur >= 'A') && (*cur <= 'Z')) ||
        ((*cur >= '0') && (*cur <= '9')) ||
         (*cur == '_') || (*cur == '-') || (*cur == '.'))
    {
        cur++;
    }
    if (space)
        while (IS_BLANK_CH(*cur)) cur++;

    if (*cur == 0)
        return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
        while (IS_BLANK(c)) {
            cur += l;
            c = CUR_SCHAR(cur, l);
        }
    }
    if ((!IS_LETTER(c)) && (c != '_'))
        return(1);

    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) ||
          (c == '.') || (c == '-') || (c == '_') ||
           IS_COMBINING(c) || IS_EXTENDER(c))
    {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    if (space) {
        while (IS_BLANK(c)) {
            cur += l;
            c = CUR_SCHAR(cur, l);
        }
    }
    if (c != 0)
        return(1);

    return(0);
}
#endif

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlValidateQName:
 * @param value the value to check
 * @param space allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of QName
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
XMLPUBFUNEXPORT int
xmlValidateQName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    /*
     * First quick algorithm for ASCII range
     */
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) || ((*cur >= 'A') && (*cur <= 'Z')) ||
    (*cur == '_'))
    cur++;
    else
    goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
       ((*cur >= 'A') && (*cur <= 'Z')) ||
       ((*cur >= '0') && (*cur <= '9')) ||
       (*cur == '_') || (*cur == '-') || (*cur == '.'))
    cur++;
    if (*cur == ':') {
    cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) ||
        ((*cur >= 'A') && (*cur <= 'Z')) ||
        (*cur == '_'))
        cur++;
    else
        goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
           ((*cur >= 'A') && (*cur <= 'Z')) ||
           ((*cur >= '0') && (*cur <= '9')) ||
           (*cur == '_') || (*cur == '-') || (*cur == '.'))
        cur++;
    }
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
    return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
    while (IS_BLANK(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if ((!IS_LETTER(c)) && (c != '_'))
    return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
       (c == '-') || (c == '_') || IS_COMBINING(c) ||
       IS_EXTENDER(c)) {
    cur += l;
    c = CUR_SCHAR(cur, l);
    }
    if (c == ':') {
    cur += l;
    c = CUR_SCHAR(cur, l);
    if ((!IS_LETTER(c)) && (c != '_'))
        return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
           (c == '-') || (c == '_') || IS_COMBINING(c) ||
           IS_EXTENDER(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if (space) {
    while (IS_BLANK(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if (c != 0)
    return(1);
    return(0);
}

/**
 * xmlValidateName:
 * @param value the value to check
 * @param space allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of Name
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
XMLPUBFUNEXPORT int
xmlValidateName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    /*
     * First quick algorithm for ASCII range
     */
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) || ((*cur >= 'A') && (*cur <= 'Z')) ||
    (*cur == '_') || (*cur == ':'))
    cur++;
    else
    goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
       ((*cur >= 'A') && (*cur <= 'Z')) ||
       ((*cur >= '0') && (*cur <= '9')) ||
       (*cur == '_') || (*cur == '-') || (*cur == '.') || (*cur == ':'))
    cur++;
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
    return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
    while (IS_BLANK(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if ((!IS_LETTER(c)) && (c != '_') && (c != ':'))
    return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') || (c == ':') ||
       (c == '-') || (c == '_') || IS_COMBINING(c) || IS_EXTENDER(c)) {
    cur += l;
    c = CUR_SCHAR(cur, l);
    }
    if (space) {
    while (IS_BLANK(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if (c != 0)
    return(1);
    return(0);
}

/**
 * xmlValidateNMToken:
 * @param value the value to check
 * @param space allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of NMToken
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
XMLPUBFUNEXPORT int
xmlValidateNMToken(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    /*
     * First quick algorithm for ASCII range
     */
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) ||
        ((*cur >= 'A') && (*cur <= 'Z')) ||
        ((*cur >= '0') && (*cur <= '9')) ||
        (*cur == '_') || (*cur == '-') || (*cur == '.') || (*cur == ':'))
    cur++;
    else
    goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
       ((*cur >= 'A') && (*cur <= 'Z')) ||
       ((*cur >= '0') && (*cur <= '9')) ||
       (*cur == '_') || (*cur == '-') || (*cur == '.') || (*cur == ':'))
    cur++;
    if (space)
    while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
    return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
    while (IS_BLANK(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if (!(IS_LETTER(c) || IS_DIGIT(c) || (c == '.') || (c == ':') ||
        (c == '-') || (c == '_') || IS_COMBINING(c) || IS_EXTENDER(c)))
    return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') || (c == ':') ||
       (c == '-') || (c == '_') || IS_COMBINING(c) || IS_EXTENDER(c)) {
    cur += l;
    c = CUR_SCHAR(cur, l);
    }
    if (space) {
    while (IS_BLANK(c)) {
        cur += l;
        c = CUR_SCHAR(cur, l);
    }
    }
    if (c != 0)
    return(1);
    return(0);
}
#endif /* LIBXML_TREE_ENABLED */

/************************************************************************
 *                                  *
 *      Allocation and deallocation of basic structures     *
 *                                  *
 ************************************************************************/

/**
 * xmlSetBufferAllocationScheme:
 * @param scheme allocation method to use
 *
 * Set the buffer allocation method.  Types are
 * XML_BUFFER_ALLOC_EXACT - use exact sizes, keeps memory usage down
 * XML_BUFFER_ALLOC_DOUBLEIT - double buffer when extra needed,
 *                             improves performance
 */
XMLPUBFUNEXPORT void
xmlSetBufferAllocationScheme(xmlBufferAllocationScheme scheme) {
	LOAD_GS_DIRECT
    xmlBufferAllocScheme = scheme;
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlGetBufferAllocationScheme:
 *
 * Types are
 * XML_BUFFER_ALLOC_EXACT - use exact sizes, keeps memory usage down
 * XML_BUFFER_ALLOC_DOUBLEIT - double buffer when extra needed,
 *                             improves performance
 *
 * Returns the current allocation scheme
 */
xmlBufferAllocationScheme
xmlGetBufferAllocationScheme(void) {
    
    return(xmlBufferAllocScheme);
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlNewNs:
 * @param node:  the element carrying the namespace
 * @param href:  the URI associated
 * @param prefix:  the prefix for the namespace
 *
 * Creation of a new Namespace. This function will refuse to create
 * a namespace with a similar prefix than an existing one present on this
 * node.
 * We use href==NULL in the case of an element creation where the namespace
 * was not defined.
 * Returns a new namespace pointer or NULL
 *
 * OOM: possible --> sets OOM flag and returns NULL
 *    Check OOM flag always!  (NULL may be if node!=NULL && prefix is already bound)
 */
XMLPUBFUNEXPORT xmlNsPtr
xmlNewNs(xmlNodePtr node, const xmlChar* href, const xmlChar* prefix)
{
	xmlNsPtr cur;
	LOAD_GS_SAFE_NODE(node)
	
    if ( (  node &&
            node->type != XML_ELEMENT_NODE )
            ||
         ( prefix &&
           xmlStrEqual(prefix, BAD_CAST "xml") ) )
    {
        return(NULL);
    }
    /*
     * Allocate a new Namespace and fill the fields.
     */
     
    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
    if (!cur)
        goto OOM;

    memset(cur, 0, sizeof(xmlNs));
    cur->type = XML_LOCAL_NAMESPACE;

    cur->href   = href   ? xmlStrdup(href)  : href   /* NULL */;
    cur->prefix = prefix ? xmlStrdup(prefix): prefix /* NULL */;

    if (OOM_FLAG){
        xmlFreeNs(cur);
OOM:
        xmlTreeErrMemory(EMBED_ERRTXT("building namespace"));
        return(NULL);
    }
    /*
     * Add it at the end to preserve parsing order ...
     * and checks for existing use of the prefix
     */
    if (node) {
        if (!node->nsDef) {
            // No namespaces so far, so add this new as the only one
            node->nsDef = cur;
        } else {

            xmlNsPtr prev = node->nsDef;

            if ((!prev->prefix && !cur->prefix) ||
                 xmlStrEqual(prev->prefix, cur->prefix))
             {
                // there is already such namespace {pref and URI} active
                // so do nothing
                xmlFreeNs(cur);
                return(NULL);
            }
            while (prev->next)
            {
                prev = prev->next;
                
                if ((!prev->prefix && !cur->prefix) ||
                    xmlStrEqual(prev->prefix, cur->prefix))
                {
                    xmlFreeNs(cur);
                    return(NULL);
                }
            }
            prev->next = cur;
        }
    }
    return(cur);
}


/**
 * xmlSetNs:
 * @param node a node in the document
 * @param ns a namespace pointer
 *
 * Associate a namespace to a node, a posteriori.
 */
XMLPUBFUNEXPORT void
xmlSetNs(xmlNodePtr node, xmlNsPtr ns) {
    if (!node) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlSetNs: node == NULL\n");
#endif
        return;
    }
    node->ns = ns;
}

/**
 * xmlFreeNs:
 * @param cur the namespace pointer
 *
 * Free up the structures associated to a namespace
 *
 * OOM: never
 * Precondition: cur is not NULL
 */
XMLPUBFUNEXPORT void
xmlFreeNs(xmlNsPtr cur)
{
#ifdef DEBUG_TREE
    if (!cur) {
        xmlGenericError(xmlGenericErrorContext, "xmlFreeNs : ns == NULL\n");
        return;
    }
#endif
    if (cur->href)
        xmlFree((char*) cur->href);
    if (cur->prefix)
        xmlFree((char*) cur->prefix);
    xmlFree(cur);
}

/**
 * xmlFreeNsList:
 * @param cur the first namespace pointer
 *
 * Free up all the structures associated to the chained namespaces.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlFreeNsList(xmlNsPtr cur)
{
    xmlNsPtr next;

#ifdef DEBUG_TREE
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext, "xmlFreeNsList : ns == NULL\n");
        return;
    }
#endif
    
    while (cur != NULL) {
        next = cur->next;
        xmlFreeNs(cur);
        cur = next;
    }
}

/**
 * xmlNewDtd:
 * @param doc the document pointer
 * @param name the DTD name
 * @param ExternalID the external ID
 * @param SystemID the system ID
 *
 * Creation of a new DTD for the external subset. To create an
 * internal subset, use xmlCreateIntSubset().
 *
 * Returns a pointer to the new DTD structure
 *
 * OOM: possible --> if returns NULL,  OOM flag is set too
 */
XMLPUBFUNEXPORT xmlDtdPtr
xmlNewDtd(xmlDocPtr doc, const xmlChar *name,
                    const xmlChar *ExternalID, const xmlChar *SystemID) {
    xmlDtdPtr cur;
    LOAD_GS_SAFE_DOC(doc)                    

    if (doc && doc->extSubset) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlNewDtd(%s): document %s already have a DTD %s\n",
        /* !!! */ (char *) name, doc->name,
        /* !!! */ (char *)doc->extSubset->name);
#endif
        return(NULL);
    }

    /*
     * Allocate a new DTD and fill the fields.
     */
    cur = (xmlDtdPtr) xmlMalloc(sizeof(xmlDtd));
    if (!cur)
        goto OOM;

    memset(cur, 0 , sizeof(xmlDtd));
    cur->type = XML_DTD_NODE;

    cur->name       = name       ? xmlStrdup(name)       : name /* NULL */;
    cur->ExternalID = ExternalID ? xmlStrdup(ExternalID) : ExternalID /* NULL */;
    cur->SystemID   = SystemID   ? xmlStrdup(SystemID)   : SystemID /* NULL */;

    if(OOM_FLAG){
        
        if(cur->name)
            xmlFree((void*)cur->name);
        if(cur->ExternalID)
            xmlFree((void*)cur->ExternalID);
        if(cur->SystemID)
            xmlFree((void*)cur->SystemID);
        xmlFree(cur);
OOM:
        xmlTreeErrMemory(EMBED_ERRTXT("building DTD"));
        return(NULL);
    }

    if (doc){
        cur->doc = doc;
        doc->extSubset = cur;
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    return(cur);
}

/**
 * xmlGetIntSubset:
 * @param doc the document pointer
 *
 * Get the internal subset of a document
 * Returns a pointer to the DTD structure or NULL if not found
 */

XMLPUBFUNEXPORT xmlDtdPtr
xmlGetIntSubset(xmlDocPtr doc) {
    xmlNodePtr cur;

    if (doc == NULL)
        return(NULL);

    cur = doc->children;
    while (cur != NULL) {
        if (cur->type == XML_DTD_NODE)
            return((xmlDtdPtr) cur);
        cur = cur->next;
    }
    return((xmlDtdPtr) doc->intSubset);
}

/**
 * xmlCreateIntSubset:
 * @param doc the document pointer
 * @param name the DTD name
 * @param ExternalID the external (PUBLIC) ID
 * @param SystemID the system ID
 *
 * Create the internal subset of a document
 * Returns a pointer to the new DTD structure
 */
XMLPUBFUNEXPORT xmlDtdPtr
xmlCreateIntSubset(xmlDocPtr doc, const xmlChar *name,
                   const xmlChar *ExternalID, const xmlChar *SystemID) {
    xmlDtdPtr cur;
    LOAD_GS_SAFE_DOC(doc)                   

    if ((doc != NULL) && (xmlGetIntSubset(doc) != NULL)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
             "xmlCreateIntSubset(): document %s already have an internal subset\n",
            doc->name);
#endif
    return(NULL);
    }

    /*
     * Allocate a new DTD and fill the fields.
     */
    cur = (xmlDtdPtr) xmlMalloc(sizeof(xmlDtd));
    if (cur == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("building internal subset"));
        return(NULL);
    }
    memset(cur, 0, sizeof(xmlDtd));
    cur->type = XML_DTD_NODE;

    if (name != NULL)
    cur->name = xmlStrdup(name);
    if (ExternalID != NULL)
    cur->ExternalID = xmlStrdup(ExternalID);
    if (SystemID != NULL)
    cur->SystemID = xmlStrdup(SystemID);
    if (doc != NULL) {
        doc->intSubset = cur;
        cur->parent = doc;
        cur->doc = doc;
        if (doc->children == NULL) {
            doc->children = (xmlNodePtr) cur;
            doc->last = (xmlNodePtr) cur;
        } else {
            if (doc->type == XML_HTML_DOCUMENT_NODE) {
                xmlNodePtr prev;

                prev = doc->children;
                prev->prev = (xmlNodePtr) cur;
                cur->next = prev;
                doc->children = (xmlNodePtr) cur;
            } else {
                xmlNodePtr next;

                next = doc->children;
                while ((next != NULL) && (next->type != XML_ELEMENT_NODE))
                    next = next->next;
                if (next == NULL) {
                    cur->prev = doc->last;
                    cur->prev->next = (xmlNodePtr) cur;
                    cur->next = NULL;
                    doc->last = (xmlNodePtr) cur;
                } else {
                    cur->next = next;
                    cur->prev = next->prev;
                    if (cur->prev == NULL)
                    doc->children = (xmlNodePtr) cur;
                    else
                    cur->prev->next = (xmlNodePtr) cur;
                    next->prev = (xmlNodePtr) cur;
                }
            }
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
    xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    return(cur);
}

/**
 * DICT_FREE:
 * @param str a string
 *
 * Free a string if it is not owned by the "dict" dictionnary in the
 * current scope
 */
#define DICT_FREE(str)                      \
    if ((str) && ((!dict) ||                \
        (xmlDictOwns(dict, (const xmlChar *)(str)) == 0)))  \
        xmlFree((char *)(str));

/**
 * xmlFreeDtd:
 * @param cur the DTD structure to free up
 *
 * Free a DTD structure.
 */
XMLPUBFUNEXPORT void
xmlFreeDtd(xmlDtdPtr cur) {
    xmlDictPtr dict = NULL;
    LOAD_GS_SAFE_DTD(cur)

    if (cur == NULL) {
    return;
    }
    if (cur->doc)
        dict = cur->doc->dict;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
    xmlDeregisterNodeDefaultValue((xmlNodePtr)cur);

    if (cur->children != NULL) {
    xmlNodePtr next, c = cur->children;

    /*
     * Cleanup all the DTD comments they are not in the DTD
     * indexes.
     */
        while (c != NULL) {
        next = c->next;
        if ((c->type == XML_COMMENT_NODE) || (c->type == XML_PI_NODE)) {
        xmlUnlinkNode(c);
        xmlFreeNode(c);
        }
        c = next;
    }
    }
    DICT_FREE(cur->name)
    DICT_FREE(cur->SystemID)
    DICT_FREE(cur->ExternalID)
    
    if (cur->notations != NULL)
        xmlFreeNotationTable((xmlNotationTablePtr) cur->notations);

    if (cur->elements != NULL)
        xmlFreeElementTable((xmlElementTablePtr) cur->elements);
    if (cur->attributes != NULL)
        xmlFreeAttributeTable((xmlAttributeTablePtr) cur->attributes);
    if (cur->entities != NULL)
        xmlFreeEntitiesTable((xmlEntitiesTablePtr) cur->entities);
    if (cur->pentities != NULL)
        xmlFreeEntitiesTable((xmlEntitiesTablePtr) cur->pentities);

    xmlFree(cur);
}

/**
 * xmlNewDoc:
 * @param version xmlChar string giving the version of XML "1.0"
 *
 * Creates a new XML document
 *
 * Returns a new document
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlDocPtr
xmlNewDoc(const xmlChar *version) {
	DEFINE_GS_PROXY
    xmlDocPtr cur;
    int i = 0;    

    if (!version)
        version = (const xmlChar *) "1.0";

    /*
     * Allocate a new document and fill the fields.
     */
    cur = (xmlDocPtr) xmlMalloc(sizeof(xmlDoc));
    if (!cur)
        goto OOM;

    memset(cur, 0, sizeof(xmlDoc));
    cur->type = XML_DOCUMENT_NODE;
#ifdef XE_ENABLE_GS_CACHING /*LIBXML_ENABLE_GS_CACHING_IN_DOC*/
    cur->cachedGs = xmlGetGlobalState();
    SET_GS_PROXY(cur->cachedGs)
#endif

    cur->version = xmlStrdup(version);
    if(! cur->version)
        goto OOM;

    cur->standalone = -1;
    cur->compression = -1; /* not initialized */
    cur->doc = cur;
    /*
     * The in memory encoding is always UTF8
     * This field will never change and would
     * be obsolete if not for binary compatibility.
     */
    
    cur->charset = XML_CHAR_ENCODING_UTF8;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    
    /*
     * Initialize array of node pointers to nodes that store external data references.
     * Added in S60 3.2 release.
     */
    cur->dataNodeMax = 10;
    cur->dataNodeList = (xmlNodePtr*) xmlMalloc(cur->dataNodeMax * sizeof(xmlNodePtr));
    if (!cur->dataNodeList)
        goto OOM; 
    for( i = 0; i < cur->dataNodeMax; i++)
    	{
    	cur->dataNodeList[i] = NULL;
    	}
   
    return(cur);
//------------------
OOM:
    xmlTreeErrMemory(EMBED_ERRTXT("building doc"));
    xmlFreeDoc(cur);
    return NULL;
}

/**
 * xmlFreeDoc:
 * @param cur pointer to the document
 *
 * Free up all the structures used by a document, tree included.
 */
XMLPUBFUNEXPORT void
xmlFreeDoc(xmlDocPtr cur) {
    xmlDtdPtr extSubset, intSubset;
    xmlDictPtr dict;// unneeded = NULL;
    LOAD_GS_SAFE_DOC(cur)

    if (!cur) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlFreeDoc : document == NULL\n");
#endif
        return;
    }
    dict = cur->dict;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
        xmlDeregisterNodeDefaultValue((xmlNodePtr)cur);

    /*
     * Do this before freeing the children list to avoid ID lookups
     */
    if (cur->ids)
        xmlFreeIDTable((xmlIDTablePtr) cur->ids);
    cur->ids = NULL;
    if (cur->refs)
        xmlFreeRefTable((xmlRefTablePtr) cur->refs);
    cur->refs = NULL;
    extSubset = cur->extSubset;
    intSubset = cur->intSubset;
    if (intSubset == extSubset)
        extSubset = NULL;
    if (extSubset) {
        xmlUnlinkNode((xmlNodePtr) cur->extSubset);
        cur->extSubset = NULL;
        xmlFreeDtd(extSubset);
    }
    if (intSubset) {
        xmlUnlinkNode((xmlNodePtr) cur->intSubset);
        cur->intSubset = NULL;
        xmlFreeDtd(intSubset);
    }

    if (cur->children)
        xmlFreeNodeList(cur->children);
    if (cur->oldNs)
        xmlFreeNsList(cur->oldNs);
	if(cur->dataNodeList)    
    	xmlFree(cur->dataNodeList);

    DICT_FREE(cur->version)
    DICT_FREE(cur->name)
    DICT_FREE(cur->encoding)
    DICT_FREE(cur->URL)
    xmlFree(cur);
    if (dict) xmlDictFree(dict);
}

/**
 * xmlStringLenGetNodeList:
 * @param doc the document
 * @param value the value of the text
 * @param len the length of the string value
 *
 * Parse the value string and build the node list associated. Should
 * produce a flat tree with only TEXTs and ENTITY_REFs.
 * Returns a pointer to the first child
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlStringLenGetNodeList(xmlDocPtr doc, const xmlChar *value, int len) {
    xmlNodePtr ret = NULL, last = NULL;
    xmlNodePtr node;
    xmlChar *val;
    const xmlChar *cur = value, *end = cur + len;
    const xmlChar *q;
    xmlEntityPtr ent;

    if (value == NULL) return(NULL);

    q = cur;

    while ((cur < end) && (*cur != 0)) {
        if (cur[0] == '&') {
            int charval = 0;
            xmlChar tmp;

            /*
            * Save the current text.
            */
            if (cur != q) {
                if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
                    xmlNodeAddContentLen(last, q, cur - q);
                } else {
                    node = xmlNewDocTextLen(doc, q, cur - q);
                    if (node == NULL) return(ret);
                    if (last == NULL)
                    last = ret = node;
                    else {
                    last->next = node;
                    node->prev = last;
                    last = node;
                    }
                }
            }
            q = cur;
            if ((cur + 2 < end) && (cur[1] == '#') && (cur[2] == 'x')) {
                cur += 3;
                if (cur < end)
                    tmp = *cur;
                else
                    tmp = 0;
                while (tmp != ';') { /* Non input consuming loop */
                    if ((tmp >= '0') && (tmp <= '9'))
                        charval = charval * 16 + (tmp - '0');
                    else if ((tmp >= 'a') && (tmp <= 'f'))
                        charval = charval * 16 + (tmp - 'a') + 10;
                    else if ((tmp >= 'A') && (tmp <= 'F'))
                        charval = charval * 16 + (tmp - 'A') + 10;
                    else {
                        xmlTreeErr(XML_TREE_INVALID_HEX, (xmlNodePtr) doc, NULL);
                        charval = 0;
                        break;
                    }
                    cur++;
                    if (cur < end)
                        tmp = *cur;
                    else
                        tmp = 0;
                }
                if (tmp == ';')
                    cur++;
                q = cur;
            }
            else
                if ((cur + 1 < end) && (cur[1] == '#')) {
                    cur += 2;
                    if (cur < end)
                        tmp = *cur;
                    else
                        tmp = 0;
                    while (tmp != ';') { /* Non input consuming loops */
                        if ((tmp >= '0') && (tmp <= '9'))
                            charval = charval * 10 + (tmp - '0');
                        else {
                            xmlTreeErr(XML_TREE_INVALID_DEC, (xmlNodePtr) doc, NULL);
                            charval = 0;
                            break;
                        }
                        cur++;
                        if (cur < end)
                            tmp = *cur;
                        else
                            tmp = 0;
                    }
                    if (tmp == ';')
                        cur++;
                    q = cur;
                }
                else
                {
                    /*
                    * Read the entity string
                    */
                    cur++;
                    q = cur;
                    while ((cur < end) && (*cur != 0) && (*cur != ';')) cur++;

                    if ((cur >= end) || (*cur == 0)) {
                        xmlTreeErr(XML_TREE_UNTERMINATED_ENTITY, (xmlNodePtr) doc, (const char *) q);
                        return(ret);
                    }
                    if (cur != q) {
                        /*
                        * Predefined entities don't generate nodes
                        */
                        val = xmlStrndup(q, cur - q);
                        ent = xmlGetDocEntity(doc, val);
                        if ((ent != NULL) &&
                            (ent->etype == XML_INTERNAL_PREDEFINED_ENTITY))
                        {
                            if (last == NULL) {
                                node = xmlNewDocText(doc, ent->content);
                                last = ret = node;
                            } else if (last->type != XML_TEXT_NODE) {
                                node = xmlNewDocText(doc, ent->content);
                                last = xmlAddNextSibling(last, node);
                            } else
                                xmlNodeAddContent(last, ent->content);

                        } else {
                        /*
                        * Create a new REFERENCE_REF node
                        */
                            node = xmlNewReference(doc, val);
                            if (node == NULL) {
                                if (val != NULL) xmlFree(val);
                                return(ret);
                            }
                            else if ((ent != NULL) && (ent->children == NULL)) {
                                xmlNodePtr temp;

                                ent->children = xmlStringGetNodeList(doc, (const xmlChar*)node->content);
                                ent->owner = 1;
                                temp = ent->children;
                                while (temp) {
                                    temp->parent = (xmlNodePtr)ent;
                                    temp = temp->next;
                                }
                            }
                            if (last == NULL) {
                                last = ret = node;
                            } else {
                                last = xmlAddNextSibling(last, node);
                            }
                        }
                        xmlFree(val);
                    }
                    cur++;
                    q = cur;
                }
            if (charval != 0) {
                xmlChar buf[10];
                int l;

                l = xmlCopyCharMultiByte(buf, charval);
                buf[l] = 0;
                node = xmlNewDocText(doc, buf);
                if (node != NULL) {
                    if (last == NULL) {
                        last = ret = node;
                    } else {
                        last = xmlAddNextSibling(last, node);
                    }
                }
                charval = 0;
            }
        } else
            cur++;
    } // while ((cur < end) && (*cur != 0))

    if ((cur != q) || (ret == NULL))
    {
        /*
        * Handle the last piece of text.
        */
        if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
            xmlNodeAddContentLen(last, q, cur - q);
        } else {
            node = xmlNewDocTextLen(doc, q, cur - q);
            if (node == NULL) return(ret);
            if (last == NULL) {
                last = ret = node;
            } else {
                last = xmlAddNextSibling(last, node);
            }
        }
    } // if ((cur != q) || (ret == NULL))
    return(ret);
}

/**
 * xmlStringGetNodeList:
 * @param doc the document
 * @param value the value of the attribute
 *
 * Parse the value string and build the node list associated. Should
 * produce a flat tree with only TEXTs and ENTITY_REFs.
 * Returns a pointer to the first child
 *
 * OOM: possible --> iif returms NULL for value!=NULL and OOM flag is set
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlStringGetNodeList(xmlDocPtr doc, const xmlChar *value) {
    xmlNodePtr ret = NULL, last = NULL;
    xmlNodePtr node;
    xmlChar* val;
    const xmlChar *cur = value;
    const xmlChar *q;
    xmlEntityPtr ent;
    LOAD_GS_SAFE_DOC(doc)

    if (value == NULL) return(NULL);

    q = cur;
    while (*cur != 0) {
    if (cur[0] == '&') {
        int charval = 0;
        xmlChar tmp;

        /*
         * Save the current text.
         */
         if (cur != q)
         {
            if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
                xmlNodeAddContentLen(last, q, cur - q); // may set OOM flag
                if(OOM_FLAG)
                    goto OOM;
            } else {
                node = xmlNewDocTextLen(doc, q, cur - q); // may set OOM flag
                if (node == NULL)
                    goto OOM;

                if (last == NULL){
                    last = ret = node;
                }else{
                    last->next = node;
                    node->prev = last;
                    last = node;
                }
            }
        }
        q = cur;
        if ((cur[1] == '#') && (cur[2] == 'x'))
        {
            cur += 3;
            tmp = *cur;
            while (tmp != ';')
            { /* Non input consuming loop */
                if ((tmp >= '0') && (tmp <= '9'))
                    charval = charval * 16 + (tmp - '0');
                else if ((tmp >= 'a') && (tmp <= 'f'))
                    charval = charval * 16 + (tmp - 'a') + 10;
                else if ((tmp >= 'A') && (tmp <= 'F'))
                    charval = charval * 16 + (tmp - 'A') + 10;
                else {
                    xmlTreeErr(XML_TREE_INVALID_HEX, (xmlNodePtr) doc, NULL);
                    charval = 0;
                    break;
                }
                cur++;
                tmp = *cur;
            }
            if (tmp == ';')
                cur++;
            q = cur;
        } else if  (cur[1] == '#') {
            cur += 2;
            tmp = *cur;
            while (tmp != ';')
            { /* Non input consuming loops */
                if ((tmp >= '0') && (tmp <= '9'))
                    charval = charval * 10 + (tmp - '0');
                else {
                    xmlTreeErr(XML_TREE_INVALID_DEC, (xmlNodePtr) doc, NULL);
                    charval = 0;
                    break;
                }
                cur++;
                tmp = *cur;
            }
            if (tmp == ';')
                cur++;
            q = cur;
        } else {
            /*
            * Read the entity string
            */
            cur++;
            q = cur;
            while ((*cur != 0) && (*cur != ';'))
            {
                cur++;
            }
            if (*cur == 0) {
                xmlTreeErr(XML_TREE_UNTERMINATED_ENTITY, (xmlNodePtr) doc, (const char *) q);
                return(ret); 
            }
            if (cur != q)
            {
                /*
                * Predefined entities don't generate nodes
                */
                val = xmlStrndup(q, cur - q);
                if(!val)
                    goto OOM;

                ent = xmlGetDocEntity(doc, val);
                if ((ent != NULL) && (ent->etype == XML_INTERNAL_PREDEFINED_ENTITY))
                {
                    if (last == NULL) {
                        node = xmlNewDocText(doc, ent->content); // may set OOM flag
                        if(!node)
                            goto OOM_val;

                        last = ret = node;
                    } else if (last->type != XML_TEXT_NODE) {
                        node = xmlNewDocText(doc, ent->content);
                        if(!node)
                            goto OOM_val;

                        last = xmlAddNextSibling(last, node); // NULL in OOM
                        if(!last)
                            goto OOM_val;
                    } else {
                        xmlNodeAddContent(last, ent->content);
                        if(OOM_FLAG)
                            goto OOM_val;
                    }
                } else {
                    /*
                    * Create a new REFERENCE_REF node
                    */
                    node = xmlNewReference(doc, val);
                    if (!node)
                    {
                        goto OOM_val;
                    }
                    else if ((ent != NULL) && (ent->children == NULL))
                    {
                        xmlNodePtr temp;
                        

                        // NOTE: Recursive call!
                        ent->children = xmlStringGetNodeList(doc, (const xmlChar*)node->content);
                        if(OOM_FLAG)
                            goto OOM_node_val;

                        ent->owner = 1;
                        temp = ent->children;
                        while (temp) {
                            temp->parent = (xmlNodePtr)ent;
                            temp = temp->next;
                        }
                    }
                    if (last == NULL) {
                        last = ret = node;
                    } else {
                        last = xmlAddNextSibling(last, node); // NULL in OOM
                        if(!last)
                            goto OOM_node_val;
                    }
                }
                xmlFree(val);
            }
            cur++;
            q = cur;
        }
        if (charval != 0) {
            xmlChar buf[10];
            int len;

            len = xmlCopyCharMultiByte(buf, charval);
            buf[len] = 0;
            node = xmlNewDocText(doc, buf);
            if (!node)
                goto OOM;
            if (last == NULL) {
                last = ret = node;
            } else {
                last = xmlAddNextSibling(last, node);
                if(!last){
                    xmlFreeNode(node);
                    goto OOM;
                }
            }
            charval = 0;
        }
    } else
        cur++;
    }
    if ((cur != q) || (ret == NULL))
    {
        /*
        * Handle the last piece of text.
        */
        if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
            xmlNodeAddContentLen(last, q, cur - q);
            if(OOM_FLAG)
                goto OOM;
        } else {
            node = xmlNewDocTextLen(doc, q, cur - q);
            if (node == NULL)
                goto OOM;
            if (last == NULL) {
                last = ret = node;
            } else {
                last = xmlAddNextSibling(last, node);
                if (!last){
                    xmlFreeNode(node);
                    goto OOM;
                }
            }
        }
    }
    return(ret);
//----------------------------
OOM_node_val:
    xmlFreeNode(node);
OOM_val:
    if(val) xmlFree(val);
OOM:
    xmlFreeNodeList(ret);
    return NULL;
}

/**
 * xmlNodeListGetString:
 * @param doc:  the document
 * @param list:  a Node list
 * @param inLine:  should we replace entity contents or show their external form
 *
 * Build the string equivalent to the text contained in the Node list
 * made of TEXTs and ENTITY_REFs
 *
 * Returns a pointer to the string copy, the caller must free it with xmlFree().
 *
 * OOM: global flag is set in OOM when NULL is returned --> flag should be checked
 */
XMLPUBFUNEXPORT xmlChar*
xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list, int inLine)
{
    xmlNodePtr node = list;
    xmlChar *ret = NULL;
    xmlEntityPtr ent;
    LOAD_GS_SAFE_DOC(doc)

    if (list == NULL)
        return (NULL);

    while (node != NULL) {
        if ((node->type == XML_TEXT_NODE) ||
            (node->type == XML_CDATA_SECTION_NODE)) {
            if (inLine) {
                ret = xmlStrcat(ret, node->content);
            } else {
                xmlChar *buffer;

                buffer = xmlEncodeEntitiesReentrant(doc, node->content); // OOM handling below
                if (buffer != NULL) {
                    ret = xmlStrcat(ret, buffer);
                    xmlFree(buffer);
                }
            }
        } else if (node->type == XML_ENTITY_REF_NODE) {
            if (inLine) {
                ent = xmlGetDocEntity(doc, node->name);
                if (ent != NULL) {
                    xmlChar *buffer;

                    /* an entity content can be any "well balanced chunk",
                     * i.e. the result of the content [43] production:
                     * http://www.w3.org/TR/REC-xml#NT-content.
                     * So it can contain text, CDATA section or nested
                     * entity reference nodes (among others).
                     * -> we recursive  call xmlNodeListGetString()
                     * which handles these types */
                    buffer = xmlNodeListGetString(doc, ent->children, 1); // may set OOM flag (the check is below)
                    if (buffer != NULL) {
                        ret = xmlStrcat(ret, buffer); // may set OOM flag
                        xmlFree(buffer);
                    }
                } else {
                    ret = xmlStrcat(ret, node->content);// may set OOM flag
                }
            } else {
                
                
                xmlChar buf[2];
                // xmlStr(n)cat calls may set OOM flag
                buf[0] = '&';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
                ret = xmlStrcat(ret, node->name);
                buf[0] = ';';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
            }
        }
#if 0
        else {
            xmlGenericError(xmlGenericErrorContext,
                            "xmlGetNodeListString : invalid node type %d\n",
                            node->type);
        }
#endif
        if(OOM_FLAG){
            if(ret)
                xmlFree(ret);
            return NULL;
        }
        node = node->next;
    }
    return (ret);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlNodeListGetRawString:
 * @param doc the document
 * @param list a Node list
 * @param inLine should we replace entity contents or show their external form
 *
 * Builds the string equivalent to the text contained in the Node list
 * made of TEXTs and ENTITY_REFs, contrary to xmlNodeListGetString()
 * this function doesn't do any character encoding handling.
 *
 * Returns a pointer to the string copy, the caller must free it with xmlFree().
 */
XMLPUBFUNEXPORT xmlChar*
xmlNodeListGetRawString(xmlDocPtr doc, xmlNodePtr list, int inLine)
{
    xmlNodePtr   node = list;
    xmlChar*     ret = NULL;
    xmlEntityPtr ent;

    if (!list)
        return (NULL);

    while (node) {
        if ((node->type == XML_TEXT_NODE) ||
            (node->type == XML_CDATA_SECTION_NODE)) {
            if (inLine) {
                ret = xmlStrcat(ret, node->content);
            } else {
                xmlChar *buffer;

                buffer = xmlEncodeSpecialChars(doc, node->content);
                if (buffer != NULL) {
                    ret = xmlStrcat(ret, buffer);
                    xmlFree(buffer);
                }
            }
        } else if (node->type == XML_ENTITY_REF_NODE) {
            if (inLine) {
                ent = xmlGetDocEntity(doc, node->name);
                if (ent) {
                    xmlChar *buffer;

                    /* an entity content can be any "well balanced chunk",
                     * i.e. the result of the content [43] production:
                     * http://www.w3.org/TR/REC-xml#NT-content.
                     * So it can contain text, CDATA section or nested
                     * entity reference nodes (among others).
                     * -> we recursive  call xmlNodeListGetRawString()
                     * which handles these types */
                    buffer = xmlNodeListGetRawString(doc, ent->children, 1);
                    if (buffer) {
                        ret = xmlStrcat(ret, buffer);
                        xmlFree(buffer);
                    }
                } else {
                    ret = xmlStrcat(ret, node->content);
                }
            } else {
                xmlChar buf[2];

                buf[0] = '&';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
                ret = xmlStrcat(ret, node->name);
                buf[0] = ';';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
            }
        }
#if 0
        else {
            xmlGenericError(xmlGenericErrorContext,
                            "xmlGetNodeListString : invalid node type %d\n",
                            node->type);
        }
#endif
        node = node->next;
    }
    return (ret);
}
#endif /* LIBXML_TREE_ENABLED */

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_HTML_ENABLED)
/**
 * xmlNewProp:
 * @param node the holding node
 * @param name the name of the attribute
 * @param value the value of the attribute
 *
 * Create a new property carried by a node.
 * Returns a pointer to the attribute
 *
 * OOM: possible --> sets OOM flag and returns NULL for name!=NULL && node is element
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlNewProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value) {
    xmlAttrPtr cur;
    xmlDocPtr doc;
    xmlChar *buffer;
    LOAD_GS_SAFE_NODE(node)

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewProp : name == NULL\n");
#endif
        return(NULL);
    }
    if ((node != NULL) && (node->type != XML_ELEMENT_NODE))
        return(NULL);

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
        return(NULL);
    }

    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node;
    cur->doc = doc = (node ? node->doc : NULL);
    cur->name = xmlStrdup(name);
    if(!cur->name)
        goto OOM;
    //
    if (value)
    {
        xmlNodePtr tmp;
        // value != NULL, so returned buffer==NULL only if OOM happens
        buffer = xmlEncodeEntitiesReentrant(doc, value);
        // if buffer==NULL, then xmlStringGetNodeList will return NULL as in OOM
        cur->children = xmlStringGetNodeList(doc, buffer);
        if(!cur->children)
            goto OOM_buffer;

        cur->last = NULL;
        tmp = cur->children;
        while (tmp) {
            tmp->parent = (xmlNodePtr) cur;
            tmp->doc = doc;
            if (tmp->next == NULL)
                cur->last = tmp;
            tmp = tmp->next;
        }
        xmlFree(buffer);
    }

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
        if (node->properties == NULL) {
            node->properties = cur;
        } else {
            xmlAttrPtr prev = node->properties;

            while (prev->next != NULL)
                prev = prev->next;
            prev->next = cur;
            cur->prev = prev;
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    return(cur);
//------------------
OOM_buffer:
    xmlFree(buffer);
OOM:
    xmlFreeProp(cur);
    return NULL;
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNewNsProp:
 * @param node the holding node
 * @param ns the namespace
 * @param name the name of the attribute
 * @param value the value of the attribute
 *
 * Create a new property tagged with a namespace and carried by a node.
 * Returns a pointer to the attribute
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlNewNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar *name, const xmlChar *value)
{
    xmlAttrPtr cur;
    xmlDocPtr doc = NULL;
    LOAD_GS_SAFE_NODE(node)

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewNsProp : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */

    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
        return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node;
    if (node != NULL) {
        doc = node->doc;
        cur->doc = doc;
    }
    cur->ns = ns;
    if (name)
    	{
    	cur->name = xmlStrdup(name);
    	if (!cur->name)
    		{
    		xmlFreeProp(cur);
    		xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
        	return(NULL);
    		}
    	}
    
    
    if (value != NULL) {
        xmlChar *buffer;
        xmlNodePtr tmp;

        buffer = xmlEncodeEntitiesReentrant(doc, value);
        if (!buffer)
        	{
			xmlFreeProp(cur);
    		xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
        	return(NULL);        	
        	}
        cur->children = xmlStringGetNodeList(doc, buffer);
        if(!cur->children)
        	{
			xmlFreeProp(cur);
			xmlFree(buffer);
    		xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
        	return(NULL);
            }
        cur->last = NULL;
        tmp = cur->children;
        while (tmp != NULL) {
            tmp->parent = (xmlNodePtr) cur;
            if (tmp->next == NULL)
                cur->last = tmp;
            tmp = tmp->next;
        }
        xmlFree(buffer);
    }

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
        if (node->properties == NULL) {
            node->properties = cur;
        } else {
            xmlAttrPtr prev = node->properties;

            while (prev->next != NULL) prev = prev->next;
            prev->next = cur;
            cur->prev = prev;
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    return(cur);
}

/**
 * xmlNewNsPropEatName:
 * @param node the holding node
 * @param ns the namespace
 * @param name the name of the attribute
 * @param value the value of the attribute
 *
 * Create a new property tagged with a namespace and carried by a node.
 * Returns a pointer to the attribute
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlNewNsPropEatName(xmlNodePtr node, xmlNsPtr ns, xmlChar *name,
           const xmlChar *value)
{
    xmlAttrPtr cur;
    xmlDocPtr doc = NULL;
    LOAD_GS_SAFE_NODE(node)

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewNsPropEatName : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
        xmlFree(name);
        return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node;
    if (node != NULL) {
            doc = node->doc;
            cur->doc = doc;
    }
    cur->ns = ns;
    cur->name = name;
    if (value != NULL) {
        xmlChar *buffer;
        xmlNodePtr tmp;

        buffer = xmlEncodeEntitiesReentrant(doc, value);
        cur->children = xmlStringGetNodeList(doc, buffer);
        cur->last = NULL;
        tmp = cur->children;
        while (tmp != NULL) {
            tmp->parent = (xmlNodePtr) cur;
            if (tmp->next == NULL)
                cur->last = tmp;
            tmp = tmp->next;
        }
        xmlFree(buffer);
    }

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
        if (node->properties == NULL) {
            node->properties = cur;
        } else {
            xmlAttrPtr prev = node->properties;

            while (prev->next != NULL)
                prev = prev->next;
            prev->next = cur;
            cur->prev = prev;
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    return(cur);
}

/**
 * xmlNewDocProp:
 * @param doc the document
 * @param name the name of the attribute
 * @param value the value of the attribute
 *
 * Create a new property carried by a document.
 * Returns a pointer to the attribute
 *
 * OOM: possible ->  NULL is returned for name!=NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlNewDocProp(xmlDocPtr doc, const xmlChar *name, const xmlChar *value) {
    xmlAttrPtr cur;
    LOAD_GS_SAFE_DOC(doc)

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewDocProp : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;
    cur->name = xmlStrdup(name);
    if(!cur->name)
        goto OOM;

    cur->doc = doc;
    if (value != NULL) {
        xmlNodePtr tmp;

        cur->children = xmlStringGetNodeList(doc, value);
        if(OOM_FLAG)
            goto OOM;

        cur->last = NULL;

        tmp = cur->children;
        while (tmp != NULL) {
            tmp->parent = (xmlNodePtr) cur;
            if (tmp->next == NULL)
                cur->last = tmp;
            tmp = tmp->next;
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    return(cur);
//---------------------------
OOM:
    xmlFreeProp(cur);
OOM_exit:
    xmlTreeErrMemory(EMBED_ERRTXT("building attribute"));
    return NULL;
}

/**
 * xmlFreePropList:
 * @param cur the first property in the list
 *
 * Free a property and all its siblings, all the children are freed too.
 */

XMLPUBFUNEXPORT void
xmlFreePropList(xmlAttrPtr cur) {
    xmlAttrPtr next;
    // O.K.: What a @#^#&! code
    //REMOVED: if (cur == NULL) return;

    while (cur != NULL) {
        next = cur->next;
        xmlFreeProp(cur);
        cur = next;
    }
}

/**
 * xmlFreeProp:
 * @param cur an attribute
 *
 * Free one attribute, all the content is freed too
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlFreeProp(xmlAttrPtr cur) {
    xmlDictPtr dict = NULL;
    LOAD_GS_SAFE_ATTR(cur)
    if (cur == NULL) return;

    if (cur->doc != NULL) dict = cur->doc->dict;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
    xmlDeregisterNodeDefaultValue((xmlNodePtr)cur);

    

    /* Check for ID removal -> leading to invalid references ! */
    if ((cur->parent != NULL) && (cur->parent->doc != NULL) &&
    ((cur->parent->doc->intSubset != NULL) ||
     (cur->parent->doc->extSubset != NULL))) {
        if (xmlIsID(cur->parent->doc, cur->parent, cur))
        xmlRemoveID(cur->parent->doc, cur);
    }
    if (cur->children != NULL) xmlFreeNodeList(cur->children);
    DICT_FREE(cur->name)

    xmlFree(cur);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlRemoveProp:
 * @param cur an attribute
 *
 * Unlink and free one attribute, all the content is freed too
 * Note this doesn't work for namespace definition attributes
 *
 * Returns 0 if success and -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlRemoveProp(xmlAttrPtr cur) {
    xmlAttrPtr tmp;
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlRemoveProp : cur == NULL\n");
#endif
        return(-1);
    }
    if (cur->parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlRemoveProp : cur->parent == NULL\n");
#endif
        return(-1);
    }
    tmp = cur->parent->properties;
    if (tmp == cur) {
        cur->parent->properties = cur->next;
        xmlFreeProp(cur);
        return(0);
    }
    while (tmp != NULL) {
        if (tmp->next == cur) {
            tmp->next = cur->next;
            if (tmp->next != NULL)
                tmp->next->prev = tmp;
            xmlFreeProp(cur);
            return(0);
        }
        tmp = tmp->next;
    }
#ifdef DEBUG_TREE
    xmlGenericError(xmlGenericErrorContext, "xmlRemoveProp : attribute not owned by its node\n");
#endif
    return(-1);
}
#endif /* LIBXML_TREE_ENABLED */


/**
 * xmlNewDocPI:
 * @param doc the target document
 * @param name the processing instruction name
 * @param content the PI content
 *
 * Creation of a processing instruction element.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocPI(xmlDocPtr doc, const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur;
    LOAD_GS_SAFE_DOC(doc)

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                "xmlNewPI : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlTreeErrMemory("building PI");
        return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_PI_NODE;

    if ((doc != NULL) && (doc->dict != NULL))
        cur->name = xmlDictLookup(doc->dict, name, -1);
    else
        cur->name = xmlStrdup(name);
    if (content != NULL) {
        cur->content = xmlStrdup(content);
    }
    cur->doc = doc;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    return(cur);
}

/**
 * xmlNewPI:
 * @param name the processing instruction name
 * @param content the PI content
 *
 * Creation of a processing instruction element.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL for name!=NULL; OOM flag is set
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewPI(const xmlChar *name, const xmlChar *content) {
	LOAD_GS_DIRECT
    xmlNodePtr cur;

    if (!name) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewPI : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_PI_NODE;

    cur->name = xmlStrdup(name);
    if(!cur->name)
        goto OOM;

    if (content) {
        cur->content = xmlStrdup(content);
        if(!cur->content){
            xmlFree((void*)cur->name);
OOM:
            xmlFree(cur);
OOM_exit:
            xmlTreeErrMemory(EMBED_ERRTXT("building PI"));
            return(NULL);
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
    {   
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    return(cur);
}

/**
 * xmlNewNode:
 * @param ns namespace if any
 * @param name the node name
 *
 * Creation of a new node element. ns is optional (NULL).
 *
 * Returns a pointer to the new node object. Uses xmlStrdup() to make
 * copy of name.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewNode(xmlNsPtr ns, const xmlChar* name)
{
	LOAD_GS_DIRECT
    xmlNodePtr cur;

    if (!name) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewNode : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ELEMENT_NODE;

    cur->name = xmlStrdup(name);
    if(!cur->name){
        xmlFree(cur); // a Node is freed!
OOM_exit:
        xmlTreeErrMemory(EMBED_ERRTXT("building node"));
        return(NULL);
    }

    cur->ns = ns;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue(cur);
    }
    return(cur);
}

/**
 * xmlNewNodeEatName:
 * @param ns namespace if any
 * @param name the node name
 *
 * Creation of a new node element. ns is optional (NULL).
 *
 * Returns a pointer to the new node object, with pointer name as
 * new node's name. Use xmlNewNode() if a copy of name string is
 * is needed as new node's name.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewNodeEatName(xmlNsPtr ns, xmlChar* name) {
	LOAD_GS_DIRECT
    xmlNodePtr cur;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewNode : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("building node"));
        xmlFree(name);
        return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ELEMENT_NODE;

    cur->name = name;
    cur->ns = ns;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    }
    return(cur);
}

/**
 * xmlNewDocNode:
 * @param doc the document
 * @param ns namespace if any
 * @param name the node name
 * @param content the XML text content if any
 *
 * Creation of a new node element within a document. ns and content
 * are optional (NULL).
 * NOTE: content is supposed to be a piece of XML CDATA, so it allow entities
 *       references, but XML special chars need to be escaped first by using
 *       xmlEncodeEntitiesReentrant(). Use xmlNewDocRawNode() if you don't
 *       need entities support.
 *
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocNode(xmlDocPtr doc, xmlNsPtr ns,
              const xmlChar *name, const xmlChar *content)
{
    xmlNodePtr cur;

    cur = xmlNewNode(ns, name);
    if (cur != NULL) {
        cur->doc = doc;
        if (content != NULL) {
            cur->children = xmlStringGetNodeList(doc, content);
            UPDATE_LAST_CHILD_AND_PARENT(cur)
        }
    }

    return(cur);
}

/**
 * xmlNewDocNodeEatName:
 * @param doc the document
 * @param ns namespace if any
 * @param name the node name
 * @param content the XML text content if any
 *
 * Creation of a new node element within a document. ns and content
 * are optional (NULL).
 * NOTE: content is supposed to be a piece of XML CDATA, so it allow entities
 *       references, but XML special chars need to be escaped first by using
 *       xmlEncodeEntitiesReentrant(). Use xmlNewDocRawNode() if you don't
 *       need entities support.
 *
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocNodeEatName(xmlDocPtr doc, xmlNsPtr ns,
              xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewNodeEatName(ns, name);
    if (cur != NULL) {
        cur->doc = doc;
    if (content != NULL) {
        cur->children = xmlStringGetNodeList(doc, content);
        UPDATE_LAST_CHILD_AND_PARENT(cur)
    }
    }
    return(cur);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlNewDocRawNode:
 * @param doc the document
 * @param ns namespace if any
 * @param name the node name
 * @param content the text content if any
 *
 * Creation of a new node element within a document. ns and content
 * are optional (NULL).
 *
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocRawNode(xmlDocPtr doc, xmlNsPtr ns,
                 const xmlChar *name, const xmlChar *content)
{
    xmlNodePtr cur;

    cur = xmlNewNode(ns, name);
    if (cur != NULL) {
        cur->doc = doc;
        if (content != NULL) {
            cur->children = xmlNewDocText(doc, content);
            
            UPDATE_LAST_CHILD_AND_PARENT(cur)
        }
    }
    return(cur);
}

/**
 * xmlNewDocFragment:
 * @param doc the document owning the fragment
 *
 * Creation of a new Fragment node.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL; OOM flag is set
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocFragment(xmlDocPtr doc) {
    xmlNodePtr cur;
    LOAD_GS_SAFE_DOC(doc)

    /*
     * Allocate a new DocumentFragment node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("building fragment")); // may set OOM flag
        return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_DOCUMENT_FRAG_NODE;

    cur->doc = doc;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
        { 
        xmlRegisterNodeDefaultValue(cur);
        }
    return(cur);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNewText:
 * @param content the text content
 *
 * Creation of a new text node.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL and sets OOM flag
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewText(const xmlChar *content)
{
	LOAD_GS_DIRECT
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_TEXT_NODE;

    cur->name = xmlStringText;
    if (content) {
        cur->content = xmlStrdup(content); // may set OOM flag
        if(!cur->content){
            xmlFree(cur);
OOM_exit:
            xmlTreeErrMemory(EMBED_ERRTXT("building text"));
            return(NULL);
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
    {   
        xmlRegisterNodeDefaultValue(cur);
    }
    return(cur);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlNewTextChild:
 * @param parent the parent node
 * @param ns a namespace if any
 * @param name the name of the child
 * @param content the text content of the child if any.
 *
 * Creation of a new child element, added at the end of parent children list.
 * ns and content parameters are optional (NULL). If ns is NULL, the newly
 * created element inherits the namespace of parent. If content is non NULL,
 * a child TEXT node will be created containing the string content.
 * NOTE: Use xmlNewChild() if content will contain entities that need to be
 * preserved. Use this function, xmlNewTextChild(), if you need to ensure that
 * reserved XML chars that might appear in content, such as the ampersand,
 * greater-than or less-than signs, are automatically replaced by their XML
 * escaped entity representations.
 *
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewTextChild(xmlNodePtr parent, xmlNsPtr ns,
            const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur, prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewTextChild : parent == NULL\n");
#endif
        return(NULL);
    }

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewTextChild : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new node
     */
    if (parent->type == XML_ELEMENT_NODE) {
        if (ns == NULL)
            cur = xmlNewDocRawNode(parent->doc, parent->ns, name, content);
        else
            cur = xmlNewDocRawNode(parent->doc, ns, name, content);
    } else if ((parent->type == XML_DOCUMENT_NODE) ||
           (parent->type == XML_HTML_DOCUMENT_NODE))
    {
        if (ns == NULL)
            cur = xmlNewDocRawNode((xmlDocPtr) parent, NULL, name, content);
        else
            cur = xmlNewDocRawNode((xmlDocPtr) parent, ns, name, content);
    } else if (parent->type == XML_DOCUMENT_FRAG_NODE) {
        cur = xmlNewDocRawNode( parent->doc, ns, name, content);
    } else {
        return(NULL);
    }
    if (cur == NULL) return(NULL);

    /*
     * add the new element at the end of the children list.
     */
    cur->type = XML_ELEMENT_NODE;
    cur->parent = parent;
    cur->doc = parent->doc;
    if (parent->children == NULL) {
        parent->children = cur;
        parent->last = cur;
    } else {
        prev = parent->last;
        prev->next = cur;
        cur->prev = prev;
        parent->last = cur;
    }

    return(cur);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNewCharRef:
 * @param doc the document
 * @param name the char ref string, starting with # or "&# ... ;"
 *
 * Creation of a new character reference node.
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewCharRef(xmlDocPtr doc, const xmlChar *name) {
    xmlNodePtr cur;
    LOAD_GS_SAFE_DOC(doc)

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
    xmlTreeErrMemory(EMBED_ERRTXT("building character reference"));
    return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ENTITY_REF_NODE;

    cur->doc = doc;
    if (name[0] == '&') {
        int len;
        name++;
    len = xmlStrlen(name);
    if (name[len - 1] == ';')
        cur->name = xmlStrndup(name, len - 1);
    else
        cur->name = xmlStrndup(name, len);
    } else
    cur->name = xmlStrdup(name);

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
    xmlRegisterNodeDefaultValue(cur);
    return(cur);
}

/**
 * xmlNewReference:
 * @param doc the document
 * @param name the reference name, or the reference string with & and ;
 *
 * Creation of a new reference node.
 * Returns a pointer to the new node object.
 *
 * OOM: returns NULL, sets OOM flag
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewReference(xmlDocPtr doc, const xmlChar *name) {
    xmlNodePtr cur;
    xmlEntityPtr ent;
    LOAD_GS_SAFE_DOC(doc)

    if (!name)
        return(NULL);

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ENTITY_REF_NODE;

    cur->doc = doc;
    if (name[0] == '&') {
        int len;
        name++;
        len = xmlStrlen(name);
        if (name[len - 1] == ';')
            cur->name = xmlStrndup(name, len - 1);
        else
            cur->name = xmlStrndup(name, len);
    } else
        cur->name = xmlStrdup(name);

    if(!cur->name){
        xmlFree(cur);
OOM_exit:
        xmlTreeErrMemory(EMBED_ERRTXT("building reference"));
        return(NULL);
    };

    ent = xmlGetDocEntity(doc, cur->name);
    if (ent) {
        cur->content = ent->content;
        /*
         * The parent pointer in entity is a DTD pointer and thus is NOT
         * updated.  Not sure if this is 100% correct.
         *  -George
         */
        cur->children = (xmlNodePtr) ent;
        cur->last = (xmlNodePtr) ent;
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue(cur);
    }
    return(cur);
}

/**
 * xmlNewDocText:
 * @param doc the document
 * @param content the text content
 *
 * Creation of a new text node within a document.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL and sets OOM flag
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocText(xmlDocPtr doc, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewText(content);
    if (cur != NULL)
        cur->doc = doc;
    return(cur);
}

/**
 * xmlNewTextLen:
 * @param content the text content
 * @param len the text len.
 *
 * Creation of a new text node with an extra parameter for the content's length
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL, sets OOM flag
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewTextLen(const xmlChar *content, int len) {
	LOAD_GS_DIRECT
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_TEXT_NODE;

    cur->name = xmlStringText;

    if (content) {
        cur->content = xmlStrndup(content, len); // sets OOM flag
        
        if(!cur->content){
            xmlFree(cur);
OOM_exit:
            xmlTreeErrMemory(EMBED_ERRTXT("building text")); // sets OOM flag
            return(NULL);
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue(cur);
    }
    return(cur);
}

/**
 * xmlNewDocTextLen:
 * @param doc the document
 * @param content the text content
 * @param len the text len.
 *
 * Creation of a new text node with an extra content length parameter. The
 * text node pertain to a given document.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL and sets OOM flag
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocTextLen(xmlDocPtr doc, const xmlChar *content, int len) {
    xmlNodePtr cur;

    cur = xmlNewTextLen(content, len);
    if (cur != NULL)
        cur->doc = doc;
    return(cur);
}




/**
 * xmlNewComment:
 * @param content the comment content
 *
 * Creation of a new node containing a comment.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL; OOM flag is set
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewComment(const xmlChar *content) {
	LOAD_GS_DIRECT
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));

    cur->type = XML_COMMENT_NODE;
    cur->name = xmlStringComment;

    if (content) {
        cur->content = xmlStrdup(content);
        if(!cur->content){
            xmlFree(cur);
OOM_exit:
            xmlTreeErrMemory(EMBED_ERRTXT("building comment"));
            return(NULL);
         }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
    {   
        xmlRegisterNodeDefaultValue(cur);
    }
    return(cur);
}

// NOTE: CDATABlock accepts length of data, should other "new" methods do the same??
/**
 * xmlNewCDataBlock:
 * @param doc the document
 * @param content the CDATA block content content
 * @param len the length of the block
 *
 * Creation of a new node containing a CDATA block.
 * Returns a pointer to the new node object.
 *
 * OOM: possible --> returns NULL; sets OOM flag
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewCDataBlock(xmlDocPtr doc, const xmlChar *content, int len) {
    xmlNodePtr cur;
    LOAD_GS_SAFE_DOC(doc)

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (!cur)
        goto OOM_exit;

    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_CDATA_SECTION_NODE;
    cur->doc = doc;

    if (content) {
        cur->content = xmlStrndup(content, len);
        if(!cur->content){
            xmlFree(cur);
OOM_exit:
            xmlTreeErrMemory(EMBED_ERRTXT("building CDATA"));
            return(NULL);
        }
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue)){
        
        xmlRegisterNodeDefaultValue(cur);
    }
    return(cur);
}

/**
 * xmlNewDocComment:
 * @param doc the document
 * @param content the comment content
 *
 * Creation of a new node containing a comment within a document.
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewDocComment(xmlDocPtr doc, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewComment(content);
    if (cur != NULL) cur->doc = doc;
    return(cur);
}

/**
 * xmlSetTreeDoc:
 * @param tree the top element
 * @param doc the document
 *
 * update all nodes under the tree to point to the right document
 *
 */
XMLPUBFUNEXPORT void
xmlSetTreeDoc(xmlNodePtr tree, xmlDocPtr doc) {
    xmlAttrPtr prop;
    LOAD_GS_SAFE_DOC(doc)

    if (tree == NULL)
        return;
    if (tree->doc != doc) {
        if(tree->type == XML_ELEMENT_NODE) {
            prop = tree->properties;
            while (prop != NULL) {
                prop->doc = doc;
                xmlSetListDoc(prop->children, doc);
                prop = prop->next;
            }
        }
        if (tree->children != NULL)
            xmlSetListDoc(tree->children, doc);
        if( IS_DATA_NODE(tree) ){
        	xmlRemoveFromDataList(tree, tree->doc);
        	xmlAppendDataList(tree, doc);
        	if(OOM_FLAG)
        		return;	
        }        
        tree->doc = doc;
    }
}

/**
 * xmlSetListDoc:
 * @param list the first element
 * @param doc the document
 *
 * update all nodes in the list to point to the right document
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlSetListDoc(xmlNodePtr list, xmlDocPtr doc) {
    xmlNodePtr cur;

    if (list == NULL)
        return;
    cur = list;
    while (cur != NULL)
    {
        if (cur->doc != doc)
            xmlSetTreeDoc(cur, doc);
        cur = cur->next;
    }
}

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlNewChild:
 * @param parent the parent node
 * @param ns a namespace if any
 * @param name the name of the child
 * @param content the XML content of the child if any.
 *
 * Creation of a new child element, added at the end of parent children list.
 * ns and content parameters are optional (NULL). If ns is NULL, the newly
 * created element inherits the namespace of parent. If content is non NULL,
 * a child list containing the TEXTs and ENTITY_REFs node will be created.
 * NOTE: content is supposed to be a piece of XML CDATA, so it allows entity
 *       references. XML special chars must be escaped first by using
 *       xmlEncodeEntitiesReentrant(), or xmlNewTextChild() should be used.
 *
 * Returns a pointer to the new node object.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlNewChild(xmlNodePtr parent, xmlNsPtr ns,
            const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur, prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewChild : parent == NULL\n");
#endif
    return(NULL);
    }

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNewChild : name == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Allocate a new node
     */
    if (parent->type == XML_ELEMENT_NODE) {
        if (ns == NULL)
            cur = xmlNewDocNode(parent->doc, parent->ns, name, content);
        else
            cur = xmlNewDocNode(parent->doc, ns, name, content);
    } else
    
    if ((parent->type == XML_DOCUMENT_NODE) ||
        (parent->type == XML_HTML_DOCUMENT_NODE))
    {
        /*
        if (ns == NULL)
            cur = xmlNewDocNode((xmlDocPtr) parent, NULL, name, content);
        else
            cur = xmlNewDocNode((xmlDocPtr) parent, ns, name, content);
        */
        // XMLENGINE: REPLACED CODE -- see above and below :)
        cur = xmlNewDocNode((xmlDocPtr) parent, ns, name, content);
    } else
    if (parent->type == XML_DOCUMENT_FRAG_NODE) {
            cur = xmlNewDocNode( parent->doc, ns, name, content);
    } else {
            return(NULL);
    }
    if (cur == NULL) return(NULL);

    /*
     * add the new element at the end of the children list.
     */
    cur->type = XML_ELEMENT_NODE;
    cur->parent = parent;
    cur->doc = parent->doc;
    if (parent->children == NULL) {
        parent->children = cur;
    parent->last = cur;
    } else {
        prev = parent->last;
    prev->next = cur;
    cur->prev = prev;
    parent->last = cur;
    }

    return(cur);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlAddNextSibling:
 * @param cur the child node
 * @param elem the new node
 *
 * Add a new node elem as the next sibling of cur
 * If the new node was already inserted in a document it is
 * first unlinked from its existing context.
 * As a result of text merging elem may be freed.
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed.
 *
 * Returns the new node or NULL in case of error.
 *
 * OOM: possible --> sets OOM flag,  returns NULL
 *                   elem is not freed in OOM
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlAddNextSibling(xmlNodePtr cur, xmlNodePtr elem) {
	LOAD_GS_SAFE_NODE(cur)
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddNextSibling : cur == NULL\n");
#endif
        return(NULL);
    }
    if (elem == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddNextSibling : elem == NULL\n");
#endif
        return(NULL);
    }
    
    xmlUnlinkNode(elem);

    if (elem->type == XML_TEXT_NODE) {
        if (cur->type == XML_TEXT_NODE) {
            xmlNodeAddContent(cur, elem->content);
            if(OOM_FLAG)
                return NULL;
            xmlFreeNode(elem);
            return(cur);
        }
        if ((cur->next != NULL) &&
            (cur->next->type == XML_TEXT_NODE) &&
            (cur->name == cur->next->name))
        {
            xmlChar* tmp;
            xmlChar* tmp2;

            tmp = xmlStrdup(elem->content);
            if(!tmp)
                return NULL;
            tmp2 = xmlStrcat(tmp, cur->next->content);
            if(OOM_FLAG)
            {
                xmlFree(tmp);
                return NULL;
            }
            
            xmlNodeSetContent(cur->next, tmp2);
            xmlFree(tmp2);
            if (OOM_FLAG)
                return NULL;
            xmlFreeNode(elem);
            return(cur->next);
        }
    } else if (elem->type == XML_ATTRIBUTE_NODE) {
        /* check if an attribute with the same name exists */
        xmlAttrPtr attr;

        if (elem->ns == NULL)
            attr = xmlHasProp(cur->parent, elem->name);
        else
            attr = xmlHasNsProp(cur->parent, elem->name, elem->ns->href);

        if(!attr){
            if(OOM_FLAG){
                xmlFreeProp((xmlAttrPtr)elem);
                return NULL;
            }
        }else{
           if (attr != (xmlAttrPtr) elem) {
                /* different instance, destroy it (attributes must be unique) */
                xmlFreeProp(attr);
           }
        }
    }

    if (elem->doc != cur->doc) {
        xmlSetTreeDoc(elem, cur->doc);
    }

    elem->parent = cur->parent;
    elem->prev = cur;
    elem->next = cur->next;
    cur->next = elem;

    if (elem->next != NULL)
        elem->next->prev = elem;
    if ((elem->parent != NULL)      &&
        (elem->parent->last == cur) &&
        (elem->type != XML_ATTRIBUTE_NODE))
    {
        elem->parent->last = elem;
    }
    return(elem);
}

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_HTML_ENABLED)
/**
 * xmlAddPrevSibling:
 * @param cur the child node
 * @param elem the new node
 *
 * Add a new node elem as the previous sibling of cur
 * merging adjacent TEXT nodes (elem may be freed)
 * If the new node was already inserted in a document it is
 * first unlinked from its existing context.
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed.
 *
 * Returns the new node or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlAddPrevSibling(xmlNodePtr cur, xmlNodePtr elem) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlAddPrevSibling : cur == NULL\n");
#endif
    return(NULL);
    }
    if (elem == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlAddPrevSibling : elem == NULL\n");
#endif
    return(NULL);
    }

    xmlUnlinkNode(elem);

    if (elem->type == XML_TEXT_NODE) {
    if (cur->type == XML_TEXT_NODE) {
        xmlChar *tmp;

        tmp = xmlStrdup(elem->content);
        tmp = xmlStrcat(tmp, cur->content);
        xmlNodeSetContent(cur, tmp);
        xmlFree(tmp);
        xmlFreeNode(elem);
        return(cur);
    }
    if ((cur->prev != NULL) && (cur->prev->type == XML_TEXT_NODE) &&
            (cur->name == cur->prev->name)) {
        xmlNodeAddContent(cur->prev, elem->content);
        xmlFreeNode(elem);
        return(cur->prev);
    }
    } else if (elem->type == XML_ATTRIBUTE_NODE) {
        /* check if an attribute with the same name exists */
    xmlAttrPtr attr;

    if (elem->ns == NULL)
        attr = xmlHasProp(cur->parent, elem->name);
    else
        attr = xmlHasNsProp(cur->parent, elem->name, elem->ns->href);
    if ((attr != NULL) && (attr != (xmlAttrPtr) elem)) {
        /* different instance, destroy it (attributes must be unique) */
        xmlFreeProp(attr);
    }
    }

    if (elem->doc != cur->doc) {
    xmlSetTreeDoc(elem, cur->doc);
    }
    elem->parent = cur->parent;
    elem->next = cur;
    elem->prev = cur->prev;
    cur->prev = elem;
    if (elem->prev != NULL)
    elem->prev->next = elem;
    if (elem->parent != NULL) {
    if (elem->type == XML_ATTRIBUTE_NODE) {
        if (elem->parent->properties == (xmlAttrPtr) cur) {
        elem->parent->properties = (xmlAttrPtr) elem;
        }
    } else {
        if (elem->parent->children == cur) {
        elem->parent->children = elem;
        }
    }
    }
    return(elem);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlAddSibling:
 * @param cur the child node
 * @param elem the new node
 *
 * Add a new element elem to the list of siblings of cur
 * merging adjacent TEXT nodes (elem may be freed)
 * If the new element was already inserted in a document it is
 * first unlinked from its existing context.
 *
 * Returns the new element or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlAddSibling(xmlNodePtr cur, xmlNodePtr elem) {
    xmlNodePtr parent;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddSibling : cur == NULL\n");
#endif
        return(NULL);
    }

    if (elem == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddSibling : elem == NULL\n");
#endif
        return(NULL);
    }

    /*
     * Constant time is we can rely on the ->parent->last to find
     * the last sibling.
     */
    
    if ((cur->parent != NULL) &&
        (cur->parent->children != NULL) &&
        (cur->parent->last != NULL) &&
        (cur->parent->last->next == NULL))
    {
        cur = cur->parent->last;
    }
    else {
        while (cur->next != NULL)
            cur = cur->next;
    }

    xmlUnlinkNode(elem);

    if ((cur->type == XML_TEXT_NODE) &&
        (elem->type == XML_TEXT_NODE) &&
        (cur->name == elem->name))
    {
        xmlNodeAddContent(cur, elem->content);
        xmlFreeNode(elem);
        return(cur);
    }

    if (elem->doc != cur->doc) {
        xmlSetTreeDoc(elem, cur->doc);
    }
    parent = cur->parent;
    elem->prev = cur;
    elem->next = NULL;
    elem->parent = parent;
    cur->next = elem;

    if (parent != NULL)
        parent->last = elem;

    return(elem);
}

/**
 * xmlAddChildList:
 * @param parent the parent node
 * @param cur the first node in the list
 *
 * Add a list of node at the end of the child list of the parent
 * merging adjacent TEXT nodes (cur may be freed)
 *
 * Returns the last child or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlAddChildList(xmlNodePtr parent, xmlNodePtr cur) {
    xmlNodePtr prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddChildList : parent == NULL\n");
#endif
        return(NULL);
    }

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddChildList : child == NULL\n");
#endif
        return(NULL);
    }

    if ((cur->doc != NULL) &&
        (parent->doc != NULL) &&
        (cur->doc != parent->doc))
   {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "Elements moved to a different document\n");
#endif
    }

    /*
     * add the first element at the end of the children list.
     */

    if (parent->children == NULL) {
        parent->children = cur;
    } else {
        /*
         * If cur and parent->last both are TEXT nodes, then merge them.
         */
        if ((cur->type == XML_TEXT_NODE) &&
            (parent->last->type == XML_TEXT_NODE) &&
            (cur->name == parent->last->name))
        {
            xmlNodeAddContent(parent->last, cur->content);
            /*
             * if it's the only child, nothing more to be done.
             */
            if (cur->next == NULL) {
                xmlFreeNode(cur);
                return(parent->last);
            }
            prev = cur;
            cur = cur->next;
            xmlFreeNode(prev);
        }
        prev = parent->last;
        prev->next = cur;
        cur->prev = prev;
    }
    while (cur->next != NULL) {
        cur->parent = parent;
        if (cur->doc != parent->doc) {
            xmlSetTreeDoc(cur, parent->doc);
        }
        cur = cur->next;
    }
    cur->parent = parent;
    cur->doc = parent->doc; /* the parent may not be linked to a doc ! */
    parent->last = cur;

    return(cur);
}

/**
 * xmlAddChild:
 * @param parent the parent node
 * @param cur the child node
 *
 * Add a new node to parent, at the end of the child (or property) list
 * merging adjacent TEXT nodes (in which case cur is freed)
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed.
 *
 * Returns the child or NULL in case of error.
 *
 * OOM: possible --> OOM flag is set, NULL is returned
 *      if OOM then 'cur' never freed
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlAddChild(xmlNodePtr parent, xmlNodePtr cur) {
    xmlNodePtr prev;
    LOAD_GS_SAFE_NODE(cur)

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddChild : parent == NULL\n");
#endif
        return(NULL);
    }

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlAddChild : child == NULL\n");
#endif
        return(NULL);
    }

    /*
     * If cur is a TEXT node, merge its content with adjacent TEXT nodes
     * cur is then freed.
     */
    
    if (cur->type == XML_TEXT_NODE) {
        if ((parent->type == XML_TEXT_NODE) &&
            (parent->content != NULL) &&
            (parent->name == cur->name) &&
            (parent != cur))
        {
            xmlNodeAddContent(parent, cur->content);
            xmlFreeNode(cur);
            //
            if(OOM_FLAG)
                return NULL;
            //
            return(parent);
        }
        if ((parent->last != NULL) &&
            (parent->last->type == XML_TEXT_NODE) &&
            (parent->last->name == cur->name) &&
            (parent->last != cur) &&
            (!(TEXT_IS_DATA(cur)))) 
        {
            xmlNodeAddContent(parent->last, cur->content);
            //
            if(OOM_FLAG)
                return NULL;
            //
            xmlFreeNode(cur);
            return(parent->last);
        }
       	if(TEXT_IS_DATA(cur)){
       		xmlAppendDataList(cur, cur->doc);
       	}
    }

    /*
     * add the new element at the end of the children list.
     */
    prev = cur->parent;
    cur->parent = parent;
    if (cur->doc != parent->doc)
    {
        xmlSetTreeDoc(cur, parent->doc);
    }
    /* this check prevents a loop on tree-traversions if a developer
     * tries to add a node to its parent multiple times
     */
    if (prev == parent)
        return(cur);

    /*
     * Coalescing
     */
    if ((parent->type == XML_TEXT_NODE) &&
        (parent->content != NULL) &&
        (parent != cur))
    {
        xmlNodeAddContent(parent, cur->content);
        if(OOM_FLAG)
            return NULL;
        //
        xmlFreeNode(cur);
        //
        return(parent);
    }
    if (cur->type == XML_ATTRIBUTE_NODE) {
        if (parent->properties == NULL) {
            parent->properties = (xmlAttrPtr) cur;
        } else {
            /* check if an attribute with the same name exists */
            xmlAttrPtr lastattr;

            if (cur->ns == NULL)
                lastattr = xmlHasProp(parent, cur->name);
            else
                lastattr = xmlHasNsProp(parent, cur->name, cur->ns->href);
            // OOM was possible --> if lastattr == NULL and xmlOOM is set

            if (lastattr != NULL){
                if (lastattr != (xmlAttrPtr) cur) {
                    /* different instance, destroy it (attributes must be unique) */
                    xmlFreeProp(lastattr);
                }
            }else{
                // lastattr == NULL
                if (OOM_FLAG)
                    return NULL;
            }
            /* find the end */
            lastattr = parent->properties;
            while (lastattr->next != NULL) {
                lastattr = lastattr->next;
            }
            lastattr->next = (xmlAttrPtr) cur;
            ((xmlAttrPtr) cur)->prev = lastattr;
        }
    } else {
        if (parent->children == NULL) {
            parent->children = cur;
            parent->last = cur;
        } else {
            prev = parent->last;
            prev->next = cur;
            cur->prev = prev;
            parent->last = cur;
        }
    }
    return(cur);
}

/**
 * xmlGetLastChild:
 * @param parent the parent node
 *
 * Search the last child of a node.
 * Returns the last child or NULL if none.
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlGetLastChild(xmlNodePtr parent) {
    
    //       Note! only there are two users: areBlanks() and xmlSAX2CDataBlock():
    //       the former always gives argument!=NULL and the latter needs code to
    //       check the argument itself
    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlGetLastChild : parent == NULL\n");
#endif
        return(NULL);
    }
    return(parent->last);
}

/**
 * xmlFreeNodeList:
 * @param cur the first node in the list
 *
 * Free a node and all its siblings, this is a recursive behaviour, all
 * the children are freed too.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlFreeNodeList(xmlNodePtr cur) {
    xmlNodePtr next;
    xmlDictPtr dict = NULL;
    LOAD_GS_SAFE_NODE(cur)

    if (cur == NULL) return;
    if (cur->type == XML_NAMESPACE_DECL) {
        xmlFreeNsList((xmlNsPtr) cur);
        return;
    }

    if ((cur->type == XML_DOCUMENT_NODE) ||
        (cur->type == XML_HTML_DOCUMENT_NODE))
    {
        xmlFreeDoc((xmlDocPtr) cur);
        return;
    }
    if (cur->doc != NULL)
        dict = cur->doc->dict;
    while (cur != NULL) {
        next = cur->next;
        if (cur->type != XML_DTD_NODE)
        {
            if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
                xmlDeregisterNodeDefaultValue(cur);
            
            if ((cur->children != NULL) &&
                (cur->type != XML_ENTITY_REF_NODE))
            {
                xmlFreeNodeList(cur->children);
            }
            
            //  type = cur->type; 
            //  A = (type == XML_ELEMENT_NODE) ||
            //      (type == XML_XINCLUDE_START) ||
            //      (type == XML_XINCLUDE_END)
            //  if(A){
            //      if(cur->properties) {xmlFreePropList(cur->properties);}
            //      if(cur->nsDef) {xmlFreeNsList(cur->nsDef);}
            //  }else
            //    if(type != XML_ENTITY_REF_NODE) {DICT_FREE(cur->content)}


            //1.  if (A && cur->properties != NULL)
            if (((cur->type == XML_ELEMENT_NODE) ||
                (cur->type == XML_XINCLUDE_START) ||
                (cur->type == XML_XINCLUDE_END)) &&
                (cur->properties != NULL))
            {
                xmlFreePropList(cur->properties);
            }

            //2.  if (!A && (cur->type != XML_ENTITY_REF_NODE))
            if ((cur->type != XML_ELEMENT_NODE) &&
                (cur->type != XML_XINCLUDE_START) &&
                (cur->type != XML_XINCLUDE_END) &&
                (cur->type != XML_ENTITY_REF_NODE))
            {
            	if (IS_DATA_NODE(cur)){
					xmlRemoveFromDataList(cur, cur->doc);
					DICT_FREE(cur->ns)
					if (TEXT_IS_BINARY(cur)){
						DICT_FREE(cur->content)	
					}
            	}
				else{
					DICT_FREE(cur->content)
				}
            }

            //3.  if (A && (cur->nsDef != NULL))
            if (((cur->type == XML_ELEMENT_NODE) ||
                (cur->type == XML_XINCLUDE_START) ||
                (cur->type == XML_XINCLUDE_END)) &&
                (cur->nsDef != NULL))
            {
                xmlFreeNsList(cur->nsDef);
            }

            /*
            * When a node is a text node or a comment, it uses a global static
            * variable for the name of the node.
            * Otherwise the node name might come from the document's
            * dictionnary
            */
            if ((cur->name) &&
                (cur->type != XML_TEXT_NODE) &&
                (cur->type != XML_COMMENT_NODE))
            {
                DICT_FREE(cur->name)
            }
            xmlFree(cur);
        }
        cur = next;
    }
}

/**
 * xmlFreeNode:
 * @param cur the node
 *
 * Free a node, this is a recursive behaviour, all the children are freed too.
 * This doesn't unlink the child from the list, use xmlUnlinkNode() first.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlFreeNode(xmlNodePtr cur) {
	xmlDictPtr dict = NULL;
	LOAD_GS_SAFE_NODE(cur)
    
    
    
    
    if (cur == NULL) return;

    /* use xmlFreeDtd for DTD nodes */
    if (cur->type == XML_DTD_NODE) {
        xmlFreeDtd((xmlDtdPtr) cur);
        return;
    }
    if (cur->type == XML_NAMESPACE_DECL) {
        xmlFreeNs((xmlNsPtr) cur);
        return;
    }
    if (cur->type == XML_ATTRIBUTE_NODE) {
        xmlFreeProp((xmlAttrPtr) cur);
        return;
    }
    if (cur->type == XML_DOCUMENT_NODE) {
        xmlFreeDoc((xmlDocPtr) cur);
        return;
    }
    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
    {   
        xmlDeregisterNodeDefaultValue(cur);
    }
    
    //  KO: I think this should be done prior to freeing other node's data

    if (cur->doc != NULL) dict = cur->doc->dict;

    if ((cur->children != NULL) &&
        (cur->type != XML_ENTITY_REF_NODE))
        xmlFreeNodeList(cur->children);

    if (((cur->type == XML_ELEMENT_NODE) ||
        (cur->type == XML_XINCLUDE_START) ||
        (cur->type == XML_XINCLUDE_END)) &&
        (cur->properties != NULL))
    {
        xmlFreePropList(cur->properties);
    }
    if ((cur->type != XML_ELEMENT_NODE) &&
        (cur->content != NULL) &&
        (cur->type != XML_ENTITY_REF_NODE) &&
        (cur->type != XML_XINCLUDE_END) &&
        (cur->type != XML_XINCLUDE_START))
    {
       	if (IS_DATA_NODE(cur)){
			xmlRemoveFromDataList(cur, cur->doc);
			DICT_FREE(cur->ns)
			if (TEXT_IS_BINARY(cur)){
				DICT_FREE(cur->content)	
			}
       	}
		else{
			DICT_FREE(cur->content)
		}      
    }

    /*
     * When a node is a text node or a comment, it uses a global static
     * variable for the name of the node.
     * Otherwise the node name might come from the document's dictionnary
     */
    if ((cur->name != NULL) &&
        (cur->type != XML_TEXT_NODE) &&
        (cur->type != XML_COMMENT_NODE))
    {
        DICT_FREE(cur->name)
    }
    if (((cur->type == XML_ELEMENT_NODE) ||
        (cur->type == XML_XINCLUDE_START) ||
        (cur->type == XML_XINCLUDE_END)) &&
        (cur->nsDef != NULL))
    {
        xmlFreeNsList(cur->nsDef);
    }
    xmlFree(cur);
}

/**
 * xmlUnlinkNode:
 * @param cur the node
 *
 * Unlink a node from it's current context, the node is not freed
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlUnlinkNode(xmlNodePtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlUnlinkNode : node == NULL\n");
#endif
        return;
    }
    if (cur->type == XML_DTD_NODE) {
        xmlDocPtr doc;
        doc = cur->doc;
        if (doc != NULL) {
            if (doc->intSubset == (xmlDtdPtr) cur)
                doc->intSubset = NULL;
            if (doc->extSubset == (xmlDtdPtr) cur)
                doc->extSubset = NULL;
        }
    }
    else if (IS_DATA_NODE(cur)){
    	xmlRemoveFromDataList(cur, cur->doc);
	}    
    if (cur->parent != NULL) {
        xmlNodePtr parent;
        parent = cur->parent;
        if (cur->type == XML_ATTRIBUTE_NODE) {
            if (parent->properties == (xmlAttrPtr) cur)
                parent->properties = ((xmlAttrPtr) cur)->next;
        } else {
            if (parent->children == cur)
                parent->children = cur->next;
            if (parent->last == cur)
                parent->last = cur->prev;
        }
        cur->parent = NULL;
    }
    if (cur->next != NULL)
        cur->next->prev = cur->prev;
    if (cur->prev != NULL)
        cur->prev->next = cur->next;
    cur->next = cur->prev = NULL;
}

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_WRITER_ENABLED)
/**
 * xmlReplaceNode:
 * @param old the old node
 * @param cur the node
 *
 * Unlink the old node from its current context, prune the new one
 * at the same place. If cur was already inserted in a document it is
 * first unlinked from its existing context.
 *
 * Returns the old node
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlReplaceNode(xmlNodePtr old, xmlNodePtr cur) {
    if (old == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlReplaceNode : old == NULL\n");
#endif
        return(NULL);
    }
    if (cur == NULL) {
        xmlUnlinkNode(old);
        return(old);
    }
    if (cur == old) {
        return(old);
    }
    if ((old->type==XML_ATTRIBUTE_NODE) && (cur->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlReplaceNode : Trying to replace attribute node with other node type\n");
#endif
        return(old);
    }
    if ((cur->type==XML_ATTRIBUTE_NODE) && (old->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlReplaceNode : Trying to replace a non-attribute node with attribute node\n");
#endif
        return(old);
    }
    xmlUnlinkNode(cur);
    
	if (IS_DATA_NODE(cur))
	{
		if(xmlReplaceInDataList(old, cur, old->doc) == -1)
		{
			xmlAppendDataList(cur,old->doc);
		}
	}
				
    cur->doc = old->doc;
    cur->parent = old->parent;
    cur->next = old->next;
    if (cur->next != NULL)
        cur->next->prev = cur;
    cur->prev = old->prev;
    if (cur->prev != NULL)
        cur->prev->next = cur;
    if (cur->parent != NULL) {
        if (cur->type == XML_ATTRIBUTE_NODE) {
            if (cur->parent->properties == (xmlAttrPtr)old)
                cur->parent->properties = ((xmlAttrPtr) cur);
        } else {
            if (cur->parent->children == old)
                cur->parent->children = cur;
            if (cur->parent->last == old)
                cur->parent->last = cur;
        }
    }
    old->next = old->prev = NULL;
    old->parent = NULL;
    return(old);
}
#endif /* LIBXML_TREE_ENABLED */

/************************************************************************
 *                                                                      *
 *      Copy operations                                                 *
 *                                                                      *
 ************************************************************************/

/**
 * xmlCopyNamespace:
 * @param cur the namespace
 *
 * Do a copy of the namespace.
 *
 * Returns: a new #xmlNsPtr, or NULL in case of error.
 *
 * OOM: possible --> returns NULL and sets OOM
 */
XMLPUBFUNEXPORT xmlNsPtr
xmlCopyNamespace(xmlNsPtr cur)
{
    if (cur == NULL)
        return(NULL);

    switch (cur->type) {
    case XML_LOCAL_NAMESPACE:
        return xmlNewNs(NULL, cur->href, cur->prefix);
    default:
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
            "xmlCopyNamespace: invalid type %d\n", cur->type);
#endif
        return(NULL);
    }
}

/**
 * xmlCopyNamespaceList:
 * @param cur the first namespace
 *
 * Do a copy of an namespace list.
 *
 * Returns: a new #xmlNsPtr, or NULL in case of error.
 *
 * OOM: possible --> if returns NULL for cur!=NULL (sets OOM flag too)
 */
XMLPUBFUNEXPORT xmlNsPtr
xmlCopyNamespaceList(xmlNsPtr cur) {
	LOAD_GS_DIRECT
    xmlNsPtr ret = NULL;
    xmlNsPtr p = NULL,q;

    while (cur != NULL) {
        q = xmlCopyNamespace(cur);
        if(!q && OOM_FLAG){
            if(ret)
                xmlFreeNsList(ret);
            return NULL;
        }
        if (p == NULL) {
            ret = p = q;
        } else {
            p->next = q;
            p = q;
        }
        cur = cur->next;
    }
    return(ret);
}

//static
XMLPUBFUN xmlNodePtr
xmlStaticCopyNodeList(xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent);
/**
 * xmlCopyProp:
 * @param target the element where the attribute will be grafted
 * @param cur the attribute
 *
 * Do a copy of the attribute.
 *
 * Returns: a new #xmlAttrPtr, or NULL in case of error.
 *
 * OOM: possible --> NULL is returned for cur!=NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlCopyProp(xmlNodePtr target, xmlAttrPtr cur)
{
    xmlAttrPtr ret;
    xmlDocPtr target_doc; // XE: new code
	LOAD_GS_SAFE_ATTR(cur)
	
    if (cur == NULL) return(NULL);

/* XMLENGINE: CHANGED CODE
    if (target != NULL)
        ret = xmlNewDocProp(target->doc, cur->name, NULL);
    else if (cur->parent != NULL)
        ret = xmlNewDocProp(cur->parent->doc, cur->name, NULL);
    else if (cur->children != NULL)
        ret = xmlNewDocProp(cur->children->doc, cur->name, NULL);
    else
        ret = xmlNewDocProp(NULL, cur->name, NULL);
*/
// XMLENGINE: REPLACEMENT CODE
     // replaces target->doc  in the rest of function
    if (target)
        target_doc = target->doc;
    else if (cur->parent)
        target_doc = cur->parent->doc;
    else if (cur->children)
        target_doc = cur->children->doc;
    else
        target_doc = NULL;

    ret = xmlNewDocProp(target_doc, cur->name, NULL);
//   END REPLACEMENT
    if(!ret)
        return ret; // NULL, OOM detected

    ret->parent = target;

    if ((cur->ns != NULL) && (target != NULL))
    {
        xmlNsPtr ns;

        ns = xmlSearchNs(target_doc, target, cur->ns->prefix);
        if (ns == NULL) {
            /*
             * Humm, we are copying an element whose namespace is defined
             * out of the new tree scope. Search it in the original tree
             * and add it at the top of the new tree
             */
            ns = xmlSearchNs(cur->doc, cur->parent, cur->ns->prefix);
            if (ns != NULL) {
              xmlNodePtr root = target;
              xmlNodePtr pred = NULL;

              while (root->parent != NULL) {
                pred = root;
                root = root->parent;
              }
              if (root == (xmlNodePtr) target_doc) {
                /* correct possibly cycling above the document elt */
                root = pred;
              }
              ret->ns = xmlNewNs(root, ns->href, ns->prefix);
            }
          } else {
                /*
                 * we have to find something appropriate here since
                 * we cant be sure, that the namespace we found is identified
                 * by the prefix
                 */
                if (xmlStrEqual(ns->href, cur->ns->href)) {
                  /* this is the nice case */
                  ret->ns = ns;
                } else {
                  /*
                   * we are in trouble: we need a new reconcilied namespace.
                   * This is expensive
                   */
                  ret->ns = xmlNewReconciliedNs(target_doc, target, cur->ns);
                }
          }

    } else
        ret->ns = NULL;
    //-
    if(OOM_FLAG)
        goto OOM;
    //-
    if (cur->children != NULL) {
        xmlNodePtr tmp;

        ret->children = xmlStaticCopyNodeList(cur->children, target_doc /* ret->doc */, (xmlNodePtr) ret);
        //-
        if(OOM_FLAG)
            goto OOM;
        //-
        ret->last = NULL;
        tmp = ret->children;
        while (tmp != NULL) {
            /* tmp->parent = (xmlNodePtr)ret; */
            if (tmp->next == NULL)
                ret->last = tmp;
            tmp = tmp->next;
        }
    }
    /*
     * Try to handle IDs
     */
    if (target && cur &&
        target->doc && cur->doc &&
        cur->doc->ids && cur->parent)
    {
        if (xmlIsID(cur->doc, cur->parent, cur)) {
            xmlChar *id;

            id = xmlNodeListGetString(cur->doc, cur->children, 1);
            if (id) {
                xmlAddID(NULL, target_doc, id, ret);
                xmlFree(id);
            }
            if(OOM_FLAG)
                goto OOM;
        }
    }
    return(ret);
OOM:
    xmlFreeProp(ret);
    return NULL;
}

/**
 * xmlCopyPropList:
 * @param target the element where the attributes will be grafted
 * @param cur the first attribute
 *
 * Do a copy of an attribute list.
 *
 * Returns: a new #xmlAttrPtr, or NULL in case of error.
 *
 * OOM: possible --> returns NULL for cur!=NULL,  sets OOM flag
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlCopyPropList(xmlNodePtr target, xmlAttrPtr cur) {
    xmlAttrPtr ret = NULL;
    xmlAttrPtr p = NULL,q;

    while (cur != NULL) {
        q = xmlCopyProp(target, cur);
        if(!q){ // OOM
            xmlFreePropList(ret);
            return NULL;
        }

        if (p == NULL) {
            ret = p = q;
        } else {
            p->next = q;
            q->prev = p;
            p = q;
        }
        cur = cur->next;
    }
    return(ret);
}

/*
 * NOTE about the CopyNode operations !
 *
 * They are split into external and internal parts for one
 * tricky reason: namespaces. Doing a direct copy of a node
 * say RPM:Copyright without changing the namespace pointer to
 * something else can produce stale links. One way to do it is
 * to keep a reference counter but this doesn't work as soon
 * as one move the element or the subtree out of the scope of
 * the existing namespace. The actual solution seems to add
 * a copy of the namespace at the top of the copied tree if
 * not available in the subtree.
 * Hence two functions, the public front-end call the inner ones
 * The argument "recursive" normally indicates a recursive copy
 * of the node with values 0 (no) and 1 (yes).  For XInclude,
 * however, we allow a value of 2 to indicate copy properties and
 * namespace info, but don't recurse on children.
 *
 * OOM: possible --> OOM flag is set when returns NULL for node!=NULL
 */
//static  // NOTE: XE: Moved to exports -- declared in tree.h now
XMLPUBFUNEXPORT xmlNodePtr
xmlStaticCopyNode(const xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent, int extended)
{
    xmlNodePtr ret;
    LOAD_GS_SAFE_DOC(doc)

    if (node == NULL)
        return(NULL);

    switch (node->type) {
        case XML_ATTRIBUTE_NODE:
            return((xmlNodePtr) xmlCopyProp(parent, (xmlAttrPtr) node));
        case XML_NAMESPACE_DECL:
            return((xmlNodePtr) xmlCopyNamespaceList((xmlNsPtr) node));
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
#ifdef LIBXML_TREE_ENABLED
            return((xmlNodePtr) xmlCopyDoc((xmlDocPtr) node, extended));
#else
            /* NOTE:  FALLTHROUGH */
#endif /* LIBXML_TREE_ENABLED */

        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
            return(NULL); 
        default:    ;
        /* DONE: 
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ELEMENT_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            break;
        */
    }

    /*
     * Allocate a new node and fill the fields.
     */
    ret = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (ret == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("copying node"));
        return(NULL);
    }
    memset(ret, 0, sizeof(xmlNode));
    ret->type = node->type;

    ret->doc = doc;
    ret->parent = parent;
    if (node->name == xmlStringText)
        ret->name = xmlStringText;
    else if (node->name == xmlStringTextNoenc)
        ret->name = xmlStringTextNoenc;
    else if (node->name == xmlStringComment)
        ret->name = xmlStringComment;
    else if (node->name != NULL)
    {
        ret->name = xmlStrdup(node->name);
        if(! ret->name)
            goto OOM;
    }

    if ((node->type != XML_ELEMENT_NODE) &&
        (node->content != NULL) &&
        (node->type != XML_ENTITY_REF_NODE) &&
        (node->type != XML_XINCLUDE_END) &&
        (node->type != XML_XINCLUDE_START))
    {
    	if (IS_DATA_NODE(node))
    		{
		   	xmlAppendDataList(ret, ret->doc);
		    if(OOM_FLAG)
		    	goto OOM;
			ret->psvi = node->psvi;
	   		if (TEXT_IS_BINARY(node)){
		        ret->content = xmlStrndup(node->content, (int)node->properties);
		        if(! ret->content)
		            goto OOM;
	   		}
		   	else{
		   		ret->content = node->content;			
		   		}
    		}
   		else{
	        ret->content = xmlStrdup(node->content);
	        if(node->content && !ret->content)
	            goto OOM;   			
   		}
    }
#ifdef LIBXML_ENABLE_NODE_LINEINFO
    else{
      if (node->type == XML_ELEMENT_NODE)
        ret->line = node->line;
    }
#endif

    if (parent != NULL) {
        xmlNodePtr tmp;

        /*
         * this is a tricky part for the node register thing:
         * in case ret does get coalesced in xmlAddChild
         * the deregister-node callback is called; so we register ret now already
         */
        if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
        {
            
            xmlRegisterNodeDefaultValue((xmlNodePtr)ret);
        }

        tmp = xmlAddChild(parent, ret);
        if(OOM_FLAG)
            goto OOM;
        /* node could have coalesced */
        if (tmp != ret)
            return(tmp);
    }

    if (!extended)
        goto out;
    if (node->nsDef != NULL){
    	if (node->type == XML_TEXT_NODE){
    		ret->nsDef = node->nsDef;
    		}
    	else{
	        ret->nsDef = xmlCopyNamespaceList(node->nsDef);
	        if(! ret->nsDef)
	            goto OOM;     		
    		}
    }
    if (node->ns != NULL) {
    	if (node->type == XML_TEXT_NODE){
    		ret->ns = (xmlNs*)xmlStrdup((const unsigned char *)node->ns);
    		}
    	else{
    		xmlNsPtr ns;
	        ns = xmlSearchNs(doc, ret, node->ns->prefix);
	        if (ns == NULL) {
	            /*
	             * Humm, we are copying an element whose namespace is defined
	             * out of the new tree scope. Search it in the original tree
	             * and add it at the top of the new tree
	             */
	            ns = xmlSearchNs(node->doc, node, node->ns->prefix);
	            if (ns != NULL) {
	                xmlNodePtr root = ret;

	                while (root->parent != NULL)
	                    root = root->parent;
	                ret->ns = xmlNewNs(root, ns->href, ns->prefix);
	            }
	        } else {
	            /*
	             * reference the existing namespace definition in our own tree.
	             */
	            ret->ns = ns;
	        }
	        if(OOM_FLAG)
	            goto OOM;
    	}
    }
    if (node->properties != NULL){
    	if(node->type == XML_TEXT_NODE) {
    		ret->properties = node->properties;
    	}
    	else {
    		ret->properties = xmlCopyPropList(ret, node->properties);	
    	}
        if(! ret->properties)
            goto OOM;
    }
    if (node->type == XML_ENTITY_REF_NODE) {
        if ((doc == NULL) || (node->doc != doc)) {
            /*
             * The copied node will go into a separate document, so
             * to avoid dangling references to the ENTITY_DECL node
             * we cannot keep the reference. Try to find it in the
             * target document.
             */
            ret->children = (xmlNodePtr) xmlGetDocEntity(doc, ret->name);
        } else {
            ret->children = node->children;
        }
        ret->last = ret->children;
    } else if ((node->children != NULL) && (extended != 2)) {
        ret->children = xmlStaticCopyNodeList(node->children, doc, ret);
        if(! ret->children)
            goto OOM;

        UPDATE_LAST_CHILD_AND_PARENT(ret)
    }

out:
    /* if parent != NULL we already registered the node above */
    if (parent == NULL && xmlRegisterNodeDefaultValue){
        
        xmlRegisterNodeDefaultValue((xmlNodePtr)ret);
    }
    return(ret);
//-------------------------
OOM:
    xmlFreeNode(ret);
    return NULL;
}

//static // NOTE: XE: Moved to exports -- declared in tree.h now
/**
    Internal routine for copying nodes lists

    OOM: possible --> sets OOM flag, returns NULL for node!=NULL
*/
XMLPUBFUNEXPORT xmlNodePtr
xmlStaticCopyNodeList(xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent) {
    xmlNodePtr ret = NULL;
    xmlNodePtr p = NULL,q;
	LOAD_GS_SAFE_DOC(doc)
	
    while (node != NULL) {
#ifdef LIBXML_TREE_ENABLED
        if (node->type == XML_DTD_NODE ) {
            if (doc == NULL) {
                node = node->next;
                continue;
            }
            if (doc->intSubset == NULL) {
                q = (xmlNodePtr) xmlCopyDtd( (xmlDtdPtr) node );
                if(!q)
                    goto OOM;
                q->doc = doc;
                q->parent = parent;
                doc->intSubset = (xmlDtdPtr) q;
                xmlAddChild(parent, q);
                if(OOM_FLAG)
                    goto OOM_q;
            } else {
                q = (xmlNodePtr) doc->intSubset;
                xmlAddChild(parent, q);
                if(OOM_FLAG)
                    goto OOM;
            }
        } else
#endif /* LIBXML_TREE_ENABLED */
        {
            q = xmlStaticCopyNode(node, doc, parent, 1);
            if(OOM_FLAG)
                goto OOM;
        }
        if (ret == NULL) {
            q->prev = NULL;
            ret = p = q;
        } else if (p != q) {
            /* the test is required if xmlStaticCopyNode coalesced 2 text nodes */
            p->next = q;
            q->prev = p;
            p = q;
        }
        node = node->next;
    }
    return(ret);
OOM_q:
    doc->intSubset = NULL; // was q
    xmlFreeDtd((xmlDtdPtr)q);
OOM:
    xmlFreeNodeList(ret);
    return NULL;
}

/**
 * xmlCopyNode:
 * @param node the node
 * @param extended if 1 do a recursive copy (properties, namespaces and children
 *          when applicable)
 *      if 2 copy properties and namespaces (when applicable)
 *
 * Do a copy of the node.
 *
 * Returns: a new #xmlNodePtr, or NULL in case of error.
 *
 * OOM: possible --> OOM flag should be checked
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlCopyNode(const xmlNodePtr node, int extended) {
    xmlNodePtr ret;

    ret = xmlStaticCopyNode(node, NULL, NULL, extended);
    return(ret);
}

/**
 * xmlDocCopyNode:
 * @param node the node
 * @param doc the document
 * @param extended if 1 - do a recursive copy (properties, namespaces and children
 *          when applicable)
 *      if 2 - copy properties and namespaces (when applicable)
 *
 * Do a copy of the node to a given document.
 *
 * Returns: a new #xmlNodePtr, or NULL in case of error.
 *
 * OOM: possible --> OOM flag should be checked
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlDocCopyNode(const xmlNodePtr node, xmlDocPtr doc, int extended) {
    xmlNodePtr ret;

    ret = xmlStaticCopyNode(node, doc, NULL, extended);
    return(ret);
}

/**
 * xmlCopyNodeList:
 * @param node the first node in the list.
 *
 * Do a recursive copy of the node list.
 *
 * Returns: a new #xmlNodePtr, or NULL in case of error.
 */
XMLPUBFUNEXPORT xmlNodePtr xmlCopyNodeList(const xmlNodePtr node) {
    xmlNodePtr ret = xmlStaticCopyNodeList(node, NULL, NULL);
    return(ret);
}


#if defined(LIBXML_TREE_ENABLED)
/**
 * xmlCopyDtd:
 * @param dtd the dtd
 *
 * Do a copy of the dtd.
 *
 * Returns: a new #xmlDtdPtr, or NULL in case of error.
 *
 * OOM: possible --> OOM flag is set when returns NULL
 */
XMLPUBFUNEXPORT xmlDtdPtr
xmlCopyDtd(xmlDtdPtr dtd) {
    xmlDtdPtr ret;
    xmlNodePtr cur, p = NULL, q;
    LOAD_GS_SAFE_DTD(dtd)

    if (dtd == NULL)
        return(NULL);

    ret = xmlNewDtd(NULL, dtd->name, dtd->ExternalID, dtd->SystemID); // may set OOM flag

    if (ret == NULL)
        return(NULL);

    if (dtd->entities != NULL){
        ret->entities = (void*) xmlCopyEntitiesTable((xmlEntitiesTablePtr) dtd->entities);
        if(OOM_FLAG) goto oom;
    }
    if (dtd->notations != NULL){
        ret->notations = (void*) xmlCopyNotationTable((xmlNotationTablePtr) dtd->notations);
        if(OOM_FLAG) goto oom;
    }
    if (dtd->elements != NULL){
        ret->elements = (void*) xmlCopyElementTable((xmlElementTablePtr) dtd->elements);
        if(OOM_FLAG) goto oom;
    }
    if (dtd->attributes != NULL){
        ret->attributes = (void*) xmlCopyAttributeTable((xmlAttributeTablePtr) dtd->attributes);
        if(OOM_FLAG) goto oom;
    }
    if (dtd->pentities != NULL){
        ret->pentities = (void*) xmlCopyEntitiesTable((xmlEntitiesTablePtr) dtd->pentities);
        if(OOM_FLAG) goto oom;
    }

    cur = dtd->children;
    while (cur != NULL) {
        q = NULL;

        if (cur->type == XML_ENTITY_DECL) {
            xmlEntityPtr tmp = (xmlEntityPtr) cur;
            switch (tmp->etype) {
            case XML_INTERNAL_GENERAL_ENTITY:
            case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
            case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
                q = (xmlNodePtr) xmlGetEntityFromDtd(ret, tmp->name);
                break;
            case XML_INTERNAL_PARAMETER_ENTITY:
            case XML_EXTERNAL_PARAMETER_ENTITY:
                q = (xmlNodePtr)
                xmlGetParameterEntityFromDtd(ret, tmp->name);
                break;
            case XML_INTERNAL_PREDEFINED_ENTITY:
                break;
            //default:
            //    break; // SHOULD NOT HAPPEN
            }
        } else if (cur->type == XML_ELEMENT_DECL) {
            xmlElementPtr tmp = (xmlElementPtr) cur;
            q = (xmlNodePtr)
            xmlGetDtdQElementDesc(ret, tmp->name, tmp->prefix);
        } else if (cur->type == XML_ATTRIBUTE_DECL) {
            xmlAttributePtr tmp = (xmlAttributePtr) cur;
            q = (xmlNodePtr)
            xmlGetDtdQAttrDesc(ret, tmp->elem, tmp->name, tmp->prefix);
        } else if (cur->type == XML_COMMENT_NODE) {
            q = xmlCopyNode(cur, 0);
        }

        if (q == NULL) {
            cur = cur->next;
            continue;
        }

        if (p == NULL)
            ret->children = q;
        else
                p->next = q;

            q->prev = p;
            q->parent = (xmlNodePtr) ret;
        q->next = NULL;
        ret->last = q;
            p = q;
        cur = cur->next;
    }

    return(ret);
// -------------------
oom:
    xmlFreeDtd(ret);
    return NULL;
}
#endif

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlCopyDoc:
 * @param doc the document
 * @param recursive if not zero do a recursive copy.
 *
 * Do a copy of the document info. If recursive, the content tree will
 * be copied too as well as DTD, namespaces and entities.
 *
 * Returns: a new #xmlDocPtr, or NULL in case of error.
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlDocPtr
xmlCopyDoc(xmlDocPtr doc, int recursive) {
    xmlDocPtr ret;
    LOAD_GS_SAFE_DOC(doc)

    if (doc == NULL)
        return(NULL);

    ret = xmlNewDoc(doc->version); // may set OOM flag
	
    if (ret == NULL)
        return(NULL); // OOM!
    if (doc->name != NULL){
        ret->name = xmlMemStrdup(doc->name); // may set OOM flag
        if(! ret->name)
            goto OOM;
    }
    if (doc->encoding != NULL){
        ret->encoding = xmlStrdup(doc->encoding);
        if(! ret->encoding)
            goto OOM;
    }
    ret->charset = doc->charset;
    ret->compression = doc->compression;
    ret->standalone = doc->standalone;

    if (!recursive)
        return(ret);

    ret->last = NULL;
    ret->children = NULL;
#ifdef LIBXML_TREE_ENABLED
    if (doc->intSubset != NULL) {
        ret->intSubset = xmlCopyDtd(doc->intSubset);
        if(! ret->intSubset)
            goto OOM;
        xmlSetTreeDoc((xmlNodePtr)ret->intSubset, ret);
        ret->intSubset->parent = ret;
    }
#endif
    if (doc->oldNs != NULL){
        ret->oldNs = xmlCopyNamespaceList(doc->oldNs);
        if(OOM_FLAG)
            goto OOM;
    }
    if (doc->children != NULL) {
        xmlNodePtr tmp;

        ret->children = xmlStaticCopyNodeList(doc->children, ret, (xmlNodePtr)ret);
        if(! ret->children)
            goto OOM;

        ret->last = NULL;
        tmp = ret->children;
        while (tmp != NULL) {
            if (tmp->next == NULL)
                ret->last = tmp;
            tmp = tmp->next;
        }
    }   
    return(ret);
OOM:
    xmlFreeDoc(ret);
    return NULL;
}
#endif /* LIBXML_TREE_ENABLED */

/************************************************************************
 *                                                                      *
 *      Content access functions                                        *
 *                                                                      *
 ************************************************************************/

#ifdef LIBXML_ENABLE_NODE_LINEINFO
/**
 * xmlGetLineNo:
 * @param node valid node
 *
 * Get line number of node. This requires activation of this option
 * before invoking the parser by calling xmlLineNumbersDefault(1)
 *
 * Returns the line number if successful, -1 otherwise
 */
long
xmlGetLineNo(xmlNodePtr node)
{
    long result = -1;

    if (!node)
        return result;
    if (node->type == XML_ELEMENT_NODE)
        result = (long) node->line;
    else if ((node->prev != NULL) &&
             ((node->prev->type == XML_ELEMENT_NODE) ||
          (node->prev->type == XML_TEXT_NODE)))
        result = xmlGetLineNo(node->prev);
    else if ((node->parent != NULL) &&
             ((node->parent->type == XML_ELEMENT_NODE) ||
              (node->parent->type == XML_TEXT_NODE)))
        result = xmlGetLineNo(node->parent);

    return result;
}
#endif  // LIBXML_ENABLE_NODE_LINEINFO

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_DEBUG_ENABLED)

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlGetNodePath:
 * @param node a node
 *
 * Build a structure based Path for the given node
 *
 * Returns the new path or NULL in case of error. The caller must free
 *     the returned string
 */
xmlChar *
xmlGetNodePath(xmlNodePtr node)
{
    xmlNodePtr cur, tmp, next;
    xmlChar *buffer = NULL, *temp;
    size_t buf_len;
    xmlChar *buf;
    const char *sep;
    const char *name;
    char nametemp[100];
    int occur = 0;

    if (node == NULL)
        return (NULL);

    buf_len = 500;
    buffer = (xmlChar *) xmlMallocAtomic(buf_len * sizeof(xmlChar));
    if (buffer == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("getting node path"));
        return (NULL);
    }
    buf = (xmlChar *) xmlMallocAtomic(buf_len * sizeof(xmlChar));
    if (buf == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("getting node path"));
        xmlFree(buffer);
        return (NULL);
    }

    buffer[0] = 0;
    cur = node;
    do {
        name = "";
        sep = "?";
        occur = 0;
        if ((cur->type == XML_DOCUMENT_NODE) ||
            (cur->type == XML_HTML_DOCUMENT_NODE)) {
            if (buffer[0] == '/')
                break;
            sep = "/";
            next = NULL;
        } else if (cur->type == XML_ELEMENT_NODE) {
            sep = "/";
            name = (const char *) cur->name;
            if (cur->ns) {
            if (cur->ns->prefix != NULL)
                    snprintf(nametemp, sizeof(nametemp) - 1,
                         EMBED_ERRTXT("%s:%s"), cur->ns->prefix, cur->name);
        else
            snprintf(nametemp, sizeof(nametemp) - 1,
                 EMBED_ERRTXT("%s"), cur->name);
                nametemp[sizeof(nametemp) - 1] = 0;
                name = nametemp;
            }
            next = cur->parent;

            /*
             * Thumbler index computation
             * 
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_ELEMENT_NODE) &&
                    (xmlStrEqual(cur->name, tmp->name)))
                    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
                    if ((tmp->type == XML_ELEMENT_NODE) &&
                        (xmlStrEqual(cur->name, tmp->name)))
                        occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if (cur->type == XML_COMMENT_NODE) {
            sep = "/";
            name = "comment()";
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if (tmp->type == XML_COMMENT_NODE)
            occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
          if (tmp->type == XML_COMMENT_NODE)
            occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if ((cur->type == XML_TEXT_NODE) ||
                   (cur->type == XML_CDATA_SECTION_NODE)) {
            sep = "/";
        name = "text()";
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((cur->type == XML_TEXT_NODE) ||
            (cur->type == XML_CDATA_SECTION_NODE))
            occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
          if ((cur->type == XML_TEXT_NODE) ||
              (cur->type == XML_CDATA_SECTION_NODE))
            occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if (cur->type == XML_PI_NODE) {
            sep = "/";
        snprintf(nametemp, sizeof(nametemp) - 1,
             EMBED_ERRTXT("processing-instruction('%s')"), cur->name);
            nametemp[sizeof(nametemp) - 1] = 0;
            name = nametemp;

        next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_PI_NODE) &&
            (xmlStrEqual(cur->name, tmp->name)))
                    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
                    if ((tmp->type == XML_PI_NODE) &&
            (xmlStrEqual(cur->name, tmp->name)))
                        occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;

        } else if (cur->type == XML_ATTRIBUTE_NODE) {
            sep = "/@";
            name = (const char *) (((xmlAttrPtr) cur)->name);
            next = ((xmlAttrPtr) cur)->parent;
        } else {
            next = cur->parent;
        }

        /*
         * Make sure there is enough room
         */
        if (xmlStrlen(buffer) + sizeof(nametemp) + 20 > buf_len) {
            buf_len =
                2 * buf_len + xmlStrlen(buffer) + sizeof(nametemp) + 20;
            temp = (xmlChar *) xmlRealloc(buffer, buf_len);
            if (temp == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("getting node path"));
                xmlFree(buf);
                xmlFree(buffer);
                return (NULL);
            }
            buffer = temp;
            temp = (xmlChar *) xmlRealloc(buf, buf_len);
            if (temp == NULL) {
        xmlTreeErrMemory(EMBED_ERRTXT("getting node path"));
                xmlFree(buf);
                xmlFree(buffer);
                return (NULL);
            }
            buf = temp;
        }
        if (occur == 0)
            snprintf((char *) buf, buf_len, EMBED_ERRTXT("%s%s%s"),
                     sep, name, (char *) buffer);
        else
            snprintf((char *) buf, buf_len, EMBED_ERRTXT("%s%s[%d]%s"),
                     sep, name, occur, (char *) buffer);
        snprintf((char *) buffer, buf_len, EMBED_ERRTXT("%s"), buf);
        cur = next;
    } while (cur != NULL);
    xmlFree(buf);
    return (buffer);
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlDocGetRootElement:
 * @param doc the document
 *
 * Get the root element of the document (doc->children is a list
 * containing possibly comments, PIs, etc ...).
 *
 * Returns the #xmlNodePtr for the root or NULL
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlDocGetRootElement(xmlDocPtr doc)
{
    xmlNodePtr ret;

    if (doc == NULL) return(NULL);
    ret = doc->children;
    while (ret != NULL)
    {
        if (ret->type == XML_ELEMENT_NODE)
            return(ret);
        ret = ret->next;
    }
    return(ret);
}

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_WRITER_ENABLED)
/**
 * xmlDocSetRootElement:
 * @param doc the document
 * @param root the new document root element
 *
 * Set the root element of the document (doc->children is a list
 * containing possibly comments, PIs, etc ...).
 *
 * Returns the old root element if any was found
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlDocSetRootElement(xmlDocPtr doc, xmlNodePtr root) {
    xmlNodePtr old = NULL;

    if (doc == NULL) return(NULL);
    if (root == NULL)return(NULL);

    xmlUnlinkNode(root);
    root->doc = doc;
    root->parent = (xmlNodePtr) doc;
    old = doc->children;

    while (old != NULL) {
        if (old->type == XML_ELEMENT_NODE)
            break;
        old = old->next;
    }

    if (old == NULL) {
        if (doc->children == NULL) {
            doc->children = root;
            doc->last = root;
        } else {
            xmlAddSibling(doc->children, root);
        }
    } else {
    xmlReplaceNode(old, root);
    }
    return(old);
}
#endif

#if defined(LIBXML_TREE_ENABLED)
/**
 * xmlNodeSetLang:
 * @param cur the node being changed
 * @param lang the language description
 *
 * Set the language of a node, i.e. the values of the xml:lang
 * attribute.
 */
XMLPUBFUNEXPORT void
xmlNodeSetLang(xmlNodePtr cur, const xmlChar *lang) {
    xmlNsPtr ns;

    if (cur == NULL) return;
    
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_NAMESPACE_DECL:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
        return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
        break;
    }
    ns = xmlSearchNsByHref(cur->doc, cur, XML_XML_NAMESPACE);
    if (ns == NULL)
        return;
    xmlSetNsProp(cur, ns, BAD_CAST "lang", lang);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNodeGetLang:
 * @param cur the node being checked
 *
 * Searches the language of a node, i.e. the values of the xml:lang
 * attribute or the one carried by the nearest ancestor.
 *
 * Returns a pointer to the lang value, or NULL if not found
 *     It's up to the caller to free the memory with xmlFree().
 */
XMLPUBFUNEXPORT xmlChar *
xmlNodeGetLang(xmlNodePtr cur) {
    xmlChar *lang;

    while (cur != NULL) {
        lang = xmlGetNsProp(cur, BAD_CAST "lang", XML_XML_NAMESPACE);
        if (lang != NULL)
            return(lang);
        cur = cur->parent;
    }
    return(NULL);
}


#ifdef LIBXML_TREE_ENABLED
/**
 * xmlNodeSetSpacePreserve:
 * @param cur the node being changed
 * @param val the xml:space value ("0": default, 1: "preserve")
 *
 * Set (or reset) the space preserving behaviour of a node, i.e. the
 * value of the xml:space attribute.
 */
XMLPUBFUNEXPORT void
xmlNodeSetSpacePreserve(xmlNodePtr cur, int val) {
    xmlNsPtr ns;

    if (cur == NULL) return;
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
    case XML_NAMESPACE_DECL:
    case XML_XINCLUDE_START:
    case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
    case XML_DOCB_DOCUMENT_NODE:
#endif
        return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
        break;
    }
    ns = xmlSearchNsByHref(cur->doc, cur, XML_XML_NAMESPACE);
    if (ns == NULL)
    return;
    switch (val) {
    case 0:
    xmlSetNsProp(cur, ns, BAD_CAST "space", BAD_CAST "default");
    break;
    case 1:
    xmlSetNsProp(cur, ns, BAD_CAST "space", BAD_CAST "preserve");
    break;
    }
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNodeGetSpacePreserve:
 * @param cur the node being checked
 *
 * Searches the space preserving behaviour of a node, i.e. the values
 * of the xml:space attribute or the one carried by the nearest
 * ancestor.
 *
 * Returns -1 if xml:space is not inherited, 0 if "default", 1 if "preserve"
 */
XMLPUBFUNEXPORT int
xmlNodeGetSpacePreserve(xmlNodePtr cur) {
    xmlChar *space;

    while (cur != NULL) {
    space = xmlGetNsProp(cur, BAD_CAST "space", XML_XML_NAMESPACE);
    if (space != NULL) {
        if (xmlStrEqual(space, BAD_CAST "preserve")) {
        xmlFree(space);
        return(1);
        }
        if (xmlStrEqual(space, BAD_CAST "default")) {
        xmlFree(space);
        return(0);
        }
        xmlFree(space);
    }
    cur = cur->parent;
    }
    return(-1);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlNodeSetName:
 * @param cur the node being changed
 * @param name the new tag name
 *
 * Set (or reset) the name of a node.
 */
XMLPUBFUNEXPORT void
xmlNodeSetName(xmlNodePtr cur, const xmlChar *name) {
    if (cur == NULL) return;
    if (name == NULL) return;
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_NAMESPACE_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
//#ifdef LIBXML_DOCB_ENABLED
//  case XML_DOCB_DOCUMENT_NODE:
//#endif
        return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_DTD_NODE:
        case XML_DOCUMENT_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        break;
    }
    if (cur->name != NULL)
        xmlFree((xmlChar *) cur->name);
    cur->name = xmlStrdup(name);
    
}
#endif

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_XINCLUDE_ENABLED)
/**
 * xmlNodeSetBase:
 * @param cur the node being changed
 * @param uri the new base URI
 *
 * Set (or reset) the base URI of a node, i.e. the value of the
 * xml:base attribute.
 */
XMLPUBFUNEXPORT void
xmlNodeSetBase(xmlNodePtr cur, const xmlChar* uri) {
    xmlNsPtr ns;

    if (cur == NULL) return;
    
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_NAMESPACE_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
            break;
        case XML_DOCUMENT_NODE:
//#ifdef LIBXML_DOCB_ENABLED
//  case XML_DOCB_DOCUMENT_NODE:
//#endif
        case XML_HTML_DOCUMENT_NODE: {
        xmlDocPtr doc = (xmlDocPtr) cur;

        if (doc->URL != NULL)
        xmlFree((xmlChar *) doc->URL);
        if (uri == NULL)
        doc->URL = NULL;
        else
        doc->URL = xmlStrdup(uri);
        return;
    }
    }

    ns = xmlSearchNsByHref(cur->doc, cur, XML_XML_NAMESPACE);
    if (ns == NULL)
        return;
    xmlSetNsProp(cur, ns, BAD_CAST "base", uri);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNodeGetBase:
 * @param doc the document the node pertains to
 * @param cur the node being checked
 *
 * Searches for the BASE URL. The code should work on both XML
 * and HTML document even if base mechanisms are completely different.
 * It returns the base as defined in RFC 2396 sections
 * 5.1.1. Base URI within Document Content
 * and
 * 5.1.2. Base URI from the Encapsulating Entity
 * However it does not return the document base (5.1.3), use
 * xmlDocumentGetBase() for this
 *
 * Returns a pointer to the base URL, or NULL if not found
 *     It's up to the caller to free the memory with xmlFree().
 *
 * OOM: possible --> check OOM flag // NOT REVIEWED
 */
XMLPUBFUNEXPORT xmlChar *
xmlNodeGetBase(xmlDocPtr doc, xmlNodePtr cur) {
    xmlChar* oldbase = NULL;
    xmlChar* base;
    xmlChar* newbase;

    if (!cur && !doc)
        return(NULL);
    if (!doc)
        doc = cur->doc;

#ifdef LIBXML_HTML_ENABLED
    if ((doc != NULL) && (doc->type == XML_HTML_DOCUMENT_NODE)) {
        cur = doc->children;
        while ((cur != NULL) && (cur->name != NULL)) {
            if (cur->type != XML_ELEMENT_NODE) {
                cur = cur->next;
                continue;
            }
            if (!xmlStrcasecmp(cur->name, BAD_CAST "html")) {
                cur = cur->children;
                continue;
            }
            if (!xmlStrcasecmp(cur->name, BAD_CAST "head")) {
                cur = cur->children;
                continue;
            }
            if (!xmlStrcasecmp(cur->name, BAD_CAST "base")) {
                    return(xmlGetProp(cur, BAD_CAST "href"));
            }
            cur = cur->next;
        }
        return(NULL);
    }
#endif /* LIBXML_HTML_ENABLED */

    while (cur) {
        if (cur->type == XML_ENTITY_DECL) {
            xmlEntityPtr ent = (xmlEntityPtr) cur;
            return(xmlStrdup(ent->URI));
        }
        if (cur->type == XML_ELEMENT_NODE) {
            base = xmlGetNsProp(cur, BAD_CAST "base", XML_XML_NAMESPACE);
            if (base) {
                if (oldbase) {
                    newbase = xmlBuildURI(oldbase, base);
                    if (newbase) {
                        xmlFree(oldbase);
                        xmlFree(base);
                        oldbase = newbase;
                    } else {
                        xmlFree(oldbase);
                        xmlFree(base);
                        return(NULL);
                    }
                } else {
                    oldbase = base;
                }
                if ((!xmlStrncmp(oldbase, BAD_CAST "http://", 7)) ||
                    (!xmlStrncmp(oldbase, BAD_CAST "ftp://", 6)) ||
                    (!xmlStrncmp(oldbase, BAD_CAST "urn:", 4)))
                    return(oldbase);
            }
        }
        cur = cur->parent;
    }
    if (doc && doc->URL) {
        if (!oldbase)
            return(xmlStrdup(doc->URL));
        newbase = xmlBuildURI(oldbase, doc->URL);
        xmlFree(oldbase);
        return(newbase);
    }
    return(oldbase);
}

/**
 * xmlNodeBufGetContent:
 * @param buffer a buffer
 * @param cur the node being read
 *
 * Read the value of a node cur, this can be either the text carried
 * directly by this node if it's a TEXT node or the aggregate string
 * of the values carried by this node child's (TEXT and ENTITY_REF).
 * Entity references are substituted.
 * Fills up the buffer buffer with this value
 *
 * Returns 0 in case of success and -1 in case of error.
 *
 * OOM: possible / NOT REVIEWED
 */
XMLPUBFUNEXPORT int
xmlNodeBufGetContent(xmlBufferPtr buffer, xmlNodePtr cur)
{
    if ((cur == NULL) || (buffer == NULL)) return(-1);
    switch (cur->type) {
        case XML_CDATA_SECTION_NODE:
        case XML_TEXT_NODE:
            xmlBufferCat(buffer, cur->content);
            break;
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:{
                xmlNodePtr tmp = cur;

                while (tmp != NULL) {
                    switch (tmp->type) {
                        case XML_CDATA_SECTION_NODE:
                        case XML_TEXT_NODE:
                            if (tmp->content != NULL)
                                xmlBufferCat(buffer, tmp->content);
                            break;
                        case XML_ENTITY_REF_NODE:
                            xmlNodeBufGetContent(buffer, tmp); // was tmp->children
                            break;
                        default:
                            break;
                    }
                    /*
                     * Skip to next node
                     */
                    if (tmp->children != NULL) {
                        if (tmp->children->type != XML_ENTITY_DECL) {
                            tmp = tmp->children;
                            continue;
                        }
                    }
                    if (tmp == cur)
                        break;

                    if (tmp->next != NULL) {
                        tmp = tmp->next;
                        continue;
                    }

                    do {
                        tmp = tmp->parent;
                        if (tmp == NULL)
                            break;
                        if (tmp == cur) {
                            tmp = NULL;
                            break;
                        }
                        if (tmp->next != NULL) {
                            tmp = tmp->next;
                            break;
                        }
                    } while (tmp != NULL);
                }
                break;
            }
        case XML_ATTRIBUTE_NODE:{
                xmlAttrPtr attr = (xmlAttrPtr) cur;
                xmlNodePtr tmp = attr->children;

                while (tmp != NULL) {
                    if (tmp->type == XML_TEXT_NODE)
                        xmlBufferCat(buffer, tmp->content);
                    else
                        xmlNodeBufGetContent(buffer, tmp);
                    tmp = tmp->next;
                }
                break;
            }
        case XML_COMMENT_NODE:
        case XML_PI_NODE:
            xmlBufferCat(buffer, cur->content);
            break;
        case XML_ENTITY_REF_NODE:{
                xmlEntityPtr ent;
                xmlNodePtr tmp;

                /* lookup entity declaration */
                ent = xmlGetDocEntity(cur->doc, cur->name);
                if (ent == NULL)
                    return(-1);

                /* an entity content can be any "well balanced chunk",
                 * i.e. the result of the content [43] production:
                 * http://www.w3.org/TR/REC-xml#NT-content
                 * -> we iterate through child nodes and recursive call
                 * xmlNodeGetContent() which handles all possible node types */
                tmp = ent->children;
                while (tmp) {
                    xmlNodeBufGetContent(buffer, tmp);
                    tmp = tmp->next;
                }
                break;
            }
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            break;
        case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_HTML_DOCUMENT_NODE:
            cur = cur->children;
            while (cur!= NULL) {
                if ((cur->type == XML_ELEMENT_NODE) ||
                    (cur->type == XML_TEXT_NODE) ||
                    (cur->type == XML_CDATA_SECTION_NODE)) {
                    xmlNodeBufGetContent(buffer, cur);
                }
                cur = cur->next;
            }
            break;
        case XML_NAMESPACE_DECL:
            xmlBufferCat(buffer, ((xmlNsPtr) cur)->href);
            break;
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
            break;
    }
    return(0);
}
/**
 * xmlNodeGetContent:
 * @param cur the node being read
 *
 * Read the value of a node, this can be either the text carried
 * directly by this node if it's a TEXT node or the aggregate string
 * of the values carried by this node child's (TEXT and ENTITY_REF).
 * Entity references are substituted.
 * Returns a new xmlChar* or NULL if no content is available.
 *     It's up to the caller to free the memory with xmlFree().
 *
 * OOM: possible --> check OOM flag always // NOT REVIEWED COMPLETELY
 */
XMLPUBFUNEXPORT xmlChar*
xmlNodeGetContent(xmlNodePtr cur)
{
    if (cur == NULL)
        return (NULL);
    
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:{
                xmlBufferPtr buffer;
                xmlChar *ret;

                buffer = xmlBufferCreateSize(64);
                if (buffer == NULL)
                    return (NULL);
                xmlNodeBufGetContent(buffer, cur);
                
                ret = buffer->content;
                buffer->content = NULL;
                xmlBufferFree(buffer);
                return (ret);
            }
        case XML_ATTRIBUTE_NODE:
                /* XMLENGINE: was replaced:
                {
                xmlAttrPtr attr = (xmlAttrPtr) cur;
                if (attr->parent != NULL)
                    return (xmlNodeListGetString(attr->parent->doc, attr->children, 1));
                else
                    return (xmlNodeListGetString(NULL, attr->children, 1));
                }
                */
                return xmlNodeListGetString(
                            (cur->parent ? cur->parent->doc : NULL),
                            cur->children,
                            1);

        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
                /* XMLENGINE: DONE: branches joined with Comment and PI nodes */
        case XML_COMMENT_NODE:
        case XML_PI_NODE:
                /* XMLENGINE: DONE: simplified */
                return cur->content ? xmlStrdup(cur->content) : cur->content /* NULL here */;
        case XML_ENTITY_REF_NODE:{
                xmlEntityPtr ent;
                xmlBufferPtr buffer;
                xmlChar*     ret;

                /* lookup entity declaration */
                ent = xmlGetDocEntity(cur->doc, cur->name);
                if (!ent)
                    return (NULL);

                buffer = xmlBufferCreate();
                if (!buffer)
                    return (NULL); // OOM

                xmlNodeBufGetContent(buffer, cur);
                
                ret = buffer->content;
                buffer->content = NULL;
                xmlBufferFree(buffer);
                return (ret);
            }
        /* XMLENGINE: defaulted
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            return (NULL);
        */
        case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_HTML_DOCUMENT_NODE: {
            xmlBufferPtr buffer;
            xmlChar *ret;

            buffer = xmlBufferCreate();
            if (!buffer)
                return (NULL);

            xmlNodeBufGetContent(buffer, (xmlNodePtr) cur);
            
            ret = buffer->content;
            buffer->content = NULL;
            xmlBufferFree(buffer);
            return (ret);
        }
        case XML_NAMESPACE_DECL:
            /*
            {
            xmlChar *tmp;

            tmp = xmlStrdup(((xmlNsPtr) cur)->href);
            return (tmp);
            }
            */
            return xmlStrdup(((xmlNsPtr) cur)->href);

        /* XMLENGINE: defaulted / not implemented by libxml2
        case XML_ELEMENT_DECL:
            
            return (NULL);
        case XML_ATTRIBUTE_DECL:
            
            return (NULL);
        case XML_ENTITY_DECL:
            
            return (NULL);
        */

        //-- most of node types do not have text content
        default:
            return (NULL);
    }
}

/**
 * xmlNodeSetContent:
 * @param cur the node being modified
 * @param content the new value of the content
 *
 * Replace the content of a node.
 *
 * OOM: possible --> sets OOM flag
 */
XMLPUBFUNEXPORT void
xmlNodeSetContent(xmlNodePtr cur, const xmlChar *content)
{
	LOAD_GS_SAFE_NODE(cur)
    if (!cur) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNodeSetContent : node == NULL\n");
#endif
        return;
    }
    
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:{
            xmlNodePtr tmpN = cur->children;
            cur->children = xmlStringGetNodeList(cur->doc, content); // may set OOM flag
            
            if(OOM_FLAG)
            {
                cur->children = tmpN;
                return;
            }
            if (tmpN)
                xmlFreeNodeList(tmpN);
            UPDATE_LAST_CHILD_AND_PARENT(cur)
            break;
            }
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
            
            if (cur->content != NULL) {
                if (!((cur->doc != NULL) &&
                     (cur->doc->dict != NULL) &&
                      xmlDictOwns(cur->doc->dict, cur->content)))
                {
                    xmlFree(cur->content);
                }
            }
            if (cur->children != NULL)
                xmlFreeNodeList(cur->children);
            cur->last = cur->children = NULL;
            if (content != NULL) {
                cur->content = xmlStrdup(content); // may set OOM flag
                
            } else
                cur->content = NULL;
            break;
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
            break;
        case XML_NOTATION_NODE:
            break;
        case XML_DTD_NODE:
            break;
        case XML_NAMESPACE_DECL:
        break;
        case XML_ELEMENT_DECL:
            
            break;
        case XML_ATTRIBUTE_DECL:
            
            break;
        case XML_ENTITY_DECL:
            
            break;
    }
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlNodeSetContentLen:
 * @param cur the node being modified
 * @param content the new value of the content
 * @param len the size of content
 *
 * Replace the content of a node.
 */
XMLPUBFUNEXPORT void
xmlNodeSetContentLen(xmlNodePtr cur, const xmlChar *content, int len)
{
    
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
        "xmlNodeSetContentLen : node == NULL\n");
#endif
    return;
    }
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
        if (cur->children != NULL) xmlFreeNodeList(cur->children);
        cur->children = xmlStringLenGetNodeList(cur->doc, content, len);
        UPDATE_LAST_CHILD_AND_PARENT(cur)
        break;
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_NOTATION_NODE:
        if (cur->content != NULL) {
        xmlFree(cur->content);
        }
        if (cur->children != NULL) xmlFreeNodeList(cur->children);
        cur->children = cur->last = NULL;
        if (content != NULL) {
        cur->content = xmlStrndup(content, len);
        } else
        cur->content = NULL;
        break;
        case XML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
    case XML_NAMESPACE_DECL:
    case XML_XINCLUDE_START:
    case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
    case XML_DOCB_DOCUMENT_NODE:
#endif
        break;
        case XML_ELEMENT_DECL:
        
        break;
        case XML_ATTRIBUTE_DECL:
        
        break;
        case XML_ENTITY_DECL:
        
        break;
    }
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlNodeAddContentLen:
 * @param cur the node being modified
 * @param content extra content
 * @param len the size of content
 *
 * Append the extra substring to the node content.
 *
 * OOM: possible --> OOM flag is set
 */
XMLPUBFUNEXPORT void
xmlNodeAddContentLen(xmlNodePtr cur, const xmlChar *content, int len)
{
    
    
    if (!cur) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNodeAddContentLen : node == NULL\n");
#endif
        return;
    }
    if (len <= 0)
        return;
    // DONE: OPTIMIZE: switch
    switch (cur->type) {
        case XML_ELEMENT_NODE:
        case XML_DOCUMENT_FRAG_NODE: {
            xmlNodePtr last, newNode, tmp;

            last = cur->last;
            newNode = xmlNewTextLen(content, len); // may set OOM flag
            if (newNode)
            {
                tmp = xmlAddChild(cur, newNode); // may set OOM flag
                if (!tmp)
                   return; // OOM has happened --> OOM flag is already set

                if (tmp != newNode)
                {
                    return; // no need to try to merge
                }
                if (last && (last->next == newNode)) {
                    xmlTextMerge(last, newNode); // may set OOM flag
                }
            }
            break;
        }
        case XML_ATTRIBUTE_NODE: // = default branch, but for faster lookup, let's have it here
            break;
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_NOTATION_NODE:{
            xmlChar* tmpS;
            if (content) {
                if (cur->doc       &&
                    cur->doc->dict &&
                    xmlDictOwns(cur->doc->dict, cur->content))
                {
                    tmpS = xmlStrncatNew(cur->content, content, len); // may set OOM flag
                    if(!tmpS)
                        return;
                    cur->content = tmpS;
                }
                else
                {
                    cur->content = xmlStrncat(cur->content, content, len); // may set OOM flag
                }
            }
            break;
        }
        default:
            break;
/* DONE: this is in the default branch now
        case XML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NAMESPACE_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
            break;
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
            break;
*/
    }
}

/**
 * xmlNodeAddContent:
 * @param cur the node being modified
 * @param content extra content
 *
 * Append the extra substring to the node content.
 *
 * OOM: possible --> OOM flag is set
 */
XMLPUBFUNEXPORT void
xmlNodeAddContent(xmlNodePtr cur, const xmlChar* content)
{
    int len;
    
    if (!cur) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext, "xmlNodeAddContent : node == NULL\n");
#endif
        return;
    }
    if (!content)
        return;
    len = xmlStrlen(content);
    xmlNodeAddContentLen(cur, content, len); // may set OOM flag
}

/**
 * xmlTextMerge:
 * @param first the first text node
 * @param second the second text node being merged
 *
 * Merge two text nodes into one
 * Returns the first text node augmented
 *
 * OOM: possible --> OOM flag is set, returns NULL
 */
XMLPUBFUNEXPORT xmlNodePtr
xmlTextMerge(xmlNodePtr first, xmlNodePtr second)
{
	LOAD_GS_SAFE_NODE(first)
    if (!first)
        return(second);
    if (!second                       ||
        first->type  != XML_TEXT_NODE ||
        second->type != XML_TEXT_NODE ||
        second->name != first->name)
    {
        return(first);
    }
    xmlNodeAddContent(first, second->content);
    if (OOM_FLAG)
        return NULL;
    xmlUnlinkNode(second);
    xmlFreeNode(second);
    return(first);
}

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_XPATH_ENABLED)
/**
 * xmlGetNsList:
 * @param doc the document
 * @param node the current node
 *
 * Search all the namespace applying to a given element.
 * Returns an NULL terminated array of all the #xmlNsPtr found
 *         that need to be freed by the caller or NULL if no
 *         namespace if defined
 *
 * OOM: iif returns NULL and OOM flag is set
 */
XMLPUBFUNEXPORT xmlNsPtr*
xmlGetNsList(xmlDocPtr doc ATTRIBUTE_UNUSED, xmlNodePtr node)
{
    xmlNsPtr  cur;
    xmlNsPtr* ret = NULL;
    int       nbns = 0;
    
    int       maxns = 10;
    int       i;

    while (node)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            cur = node->nsDef;
            while (cur)
            {
                if (!ret)
                {
                    ret = (xmlNsPtr*) xmlMalloc((maxns + 1) * sizeof(xmlNsPtr));
                    if (!ret) {
                        xmlTreeErrMemory(EMBED_ERRTXT("getting namespace list"));
                        return (NULL);
                    }
                    ret[nbns] = NULL;
                }
                for (i = 0; i < nbns; i++) {
                    if (cur->prefix == ret[i]->prefix ||
                        xmlStrEqual(cur->prefix, ret[i]->prefix))
                        break;
                }
                if (i >= nbns)
                {
                    if (nbns >= maxns)
                    {
                        xmlNsPtr* tmpret;
                        maxns *= 2;
                        tmpret = (xmlNsPtr*) xmlRealloc(ret,(maxns + 1) * sizeof(xmlNsPtr)); // may set OOM flag
                        if (!tmpret) {
                            xmlFree(ret);
                            xmlTreeErrMemory(EMBED_ERRTXT("getting namespace list"));// sets OOM flag
                            return (NULL);
                        }
                        ret = tmpret;
                    }
                    ret[nbns++] = cur;
                    ret[nbns] = NULL;
                }
                cur = cur->next;
            } // while (cur)
        } // if (node->type == XML_ELEMENT_NODE)
        node = node->parent;
    } // while (node)
    return (ret);
}
#endif /* LIBXML_TREE_ENABLED */

// DONE: Rename argument nameSpace into nsPrefix
/**
 * xmlSearchNs:
 * @param doc the document
 * @param node the current node
 * @param nsPrefix the namespace prefix
 *
 * Search a Ns registered under a given name space for a document.
 * recurse on the parents until it finds the defined namespace
 * or return NULL otherwise.
 * nsPrefix can be NULL, this is a search for the default namespace.
 * We don't allow to cross entities boundaries. If you don't declare
 * the namespace within those you will be in troubles !!! A warning
 * is generated to cover this case.
 *
 * Returns the namespace pointer or NULL.
 *
 * OOM: possible only for "xml" prefix, when (!doc && node->type==XML_ELEMENT_NODE) || !doc->oldNs ,
 *      NULL is returned then as if "NOT FOUND" result and OOM flag is set
 */
XMLPUBFUNEXPORT xmlNsPtr
xmlSearchNs(xmlDocPtr doc, xmlNodePtr node, const xmlChar* nsPrefix) {
	
    xmlNsPtr cur;
    xmlNodePtr orig = node;
    LOAD_GS_SAFE_DOC(doc)

    if (!node)
        return(NULL);

    if (nsPrefix && xmlStrEqual(nsPrefix, (const xmlChar*) "xml"))
    {
        if (!doc && node->type == XML_ELEMENT_NODE)
        {
            /*
             * The XML-1.0 namespace is normally held on the root
             * element. In this case exceptionally create it on the
             * node element.
             */
            cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
            if (!cur)
                goto OOM_exit;

            memset(cur, 0, sizeof(xmlNs));
            cur->type = XML_LOCAL_NAMESPACE;
            cur->href = xmlStrdup(XML_XML_NAMESPACE);
            cur->prefix = xmlStrdup((const xmlChar *)"xml");

            if(OOM_FLAG){
                xmlFreeNs(cur);
OOM_exit:
                xmlTreeErrMemory(EMBED_ERRTXT("searching namespace"));
                return(NULL);
            }

            cur->next = node->nsDef;
            node->nsDef = cur;
            return(cur);
        }
        
        
        if (!doc->oldNs) {
            /*
             * Allocate a new Namespace and fill the fields.
             */
            doc->oldNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
            if (!doc->oldNs)
                goto OOM_exit;

            memset(doc->oldNs, 0, sizeof(xmlNs));
            doc->oldNs->type = XML_LOCAL_NAMESPACE;
            doc->oldNs->href = xmlStrdup(XML_XML_NAMESPACE);        //
            doc->oldNs->prefix = xmlStrdup((const xmlChar *)"xml"); // may set OOM flag
            if(OOM_FLAG){
                xmlFreeNs(doc->oldNs);
                doc->oldNs = NULL;
                goto OOM_exit;
            }
        }
        return(doc->oldNs);
    } // if nsPrefix is "xml"

    
    
    while (node &&
           node->type != XML_ENTITY_NODE     &&
           node->type != XML_ENTITY_REF_NODE &&
           node->type != XML_ENTITY_DECL)
   {

        if (node->type == XML_ELEMENT_NODE)
        {
            
            
            cur = node->nsDef;
            while (cur)
            {
                if (( !cur->prefix && !nsPrefix && cur->href  )
                        ||
                    (  cur->prefix  &&  nsPrefix && cur->href &&
                       xmlStrEqual(cur->prefix, nsPrefix)     ))
                {
                    return(cur);
                }
                cur = cur->next;
            }
            if (orig != node) {
                cur = node->ns;
                if (cur
                       &&
                       (( !cur->prefix && !nsPrefix && cur->href  )
                            ||
                        (  cur->prefix &&  nsPrefix && cur->href &&
                          xmlStrEqual(cur->prefix, nsPrefix)      ))
                   )
                {
                    return(cur);
                }
            }
        }
        node = node->parent;
    }
    return(NULL);
}

/**
 * xmlNsInScope:
 * @param doc the document
 * @param node the current node
 * @param ancestor the ancestor carrying the namespace
 * @param prefix the namespace prefix
 *
 * Verify that the given namespace held on ancestor is still in scope
 * on node.
 *
 * Returns 1 if true, 0 if false and -1 in case of error.
 *
 * OOM: never
 */
static int
xmlNsInScope(xmlDocPtr doc ATTRIBUTE_UNUSED, xmlNodePtr node,
             xmlNodePtr ancestor, const xmlChar * prefix)
{
    xmlNsPtr tst;

    while (node && (node != ancestor))
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            tst = node->nsDef;
            while (tst)
            {
                if ( (!tst->prefix && !prefix)
                        ||
                     (tst->prefix  &&  prefix &&
                      xmlStrEqual(tst->prefix, prefix)) )
                {
                    return (0);
                }
                tst = tst->next;
            }
        }
        else
        if (node->type == XML_ENTITY_REF_NODE ||
            node->type == XML_ENTITY_NODE     ||
            node->type == XML_ENTITY_DECL)
        {
            return (-1);
        }
        node = node->parent;
    }
    return (node != ancestor) ? -1 : 1;
}

/**
 * xmlSearchNsByHref:
 * @param doc the document
 * @param node the current node
 * @param href the namespace value
 *
 * Search a Ns aliasing a given URI. Recurse on the parents until it finds
 * the defined namespace or return NULL otherwise.
 * Returns the namespace pointer or NULL.
 *
 * OOM: possible iif: href is XML_XML_NAMESPACE && (!doc && node->type == XML_ELEMENT_NODE) || !(doc->oldNs))
 *      --> then OOM flag is set when it returns NULL,
 *      otherwise, OOM never happens
 */
XMLPUBFUNEXPORT xmlNsPtr
xmlSearchNsByHref(xmlDocPtr doc, xmlNodePtr node, const xmlChar* href)
{
    xmlNsPtr cur;
    xmlNodePtr orig = node;
    int is_attr;
    LOAD_GS_SAFE_DOC(doc)

    if (!node || !href)
        return (NULL);

    if (xmlStrEqual(href, XML_XML_NAMESPACE))
    {
        /*
            Maybe in the future we will have global const XML namespace object
            that will be reused everywhere:
            - no need to allocate memory for it (thus no OOM may happen) - it is just
              linked as the last NsDef in the list
            - it is never freed (we should have special routine for deleting xmlNsPtr that
              compares pointer with the global object's pointer)

              O.K. NRC/Helsinki
        */

        /*
         * Only the document can hold the XML spec namespace.
         */
        if (!doc && (node->type == XML_ELEMENT_NODE))
        {
            /*
             * The XML-1.0 namespace is normally held on the root
             * element. In this case exceptionally create it on the
             * node element.
             */
            cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
            if (!cur)
                goto OOM_exit;

            memset(cur, 0, sizeof(xmlNs));
            cur->type = XML_LOCAL_NAMESPACE;
            cur->href = xmlStrdup(XML_XML_NAMESPACE);
            cur->prefix = xmlStrdup((const xmlChar *) "xml");

            if(OOM_FLAG){ // OOM!
                xmlFreeNs(cur);
OOM_exit:
                xmlTreeErrMemory(EMBED_ERRTXT("searching namespace"));
                return (NULL);
            }
            cur->next = node->nsDef;
            node->nsDef = cur;
            return (cur);
        }

        if (!doc->oldNs)
        {
            /*
             * Allocate a new Namespace and fill the fields.
             */
            doc->oldNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
            if (!doc->oldNs)
                goto OOM_exit;

            memset(doc->oldNs, 0, sizeof(xmlNs));
            doc->oldNs->type = XML_LOCAL_NAMESPACE;
            doc->oldNs->href = xmlStrdup(XML_XML_NAMESPACE);
            doc->oldNs->prefix = xmlStrdup((const xmlChar*) "xml");
            if(OOM_FLAG){
                xmlFreeNs(doc->oldNs);
                doc->oldNs = NULL;
                goto OOM_exit;
            }
        }
        return (doc->oldNs);
    } // end if XML namespace

    is_attr = (node->type == XML_ATTRIBUTE_NODE);
    while (node)
    {
        if ((node->type == XML_ENTITY_REF_NODE) ||
            (node->type == XML_ENTITY_NODE)     ||
            (node->type == XML_ENTITY_DECL))
        {
            return (NULL);
        }
        if (node->type == XML_ELEMENT_NODE)
        {
            cur = node->nsDef;
            while (cur)
            {
                if (cur->href &&
                    href      &&
                    xmlStrEqual(cur->href, href))
                {
                    if ((!is_attr || cur->prefix) &&
                        (xmlNsInScope(doc, orig, node, cur->href) == 1))
                        return (cur);
                }
                cur = cur->next;
            }
            if (orig != node)
            {   
                cur = node->ns;
                if (cur                          &&
                    cur->href                    &&
                    href                         && // move up?
                    xmlStrEqual(cur->href, href) &&
                    (!is_attr || cur->prefix)    && // move up?
                    (xmlNsInScope(doc, orig, node, cur->href) == 1))
                {
                        return (cur);
                }

            }
        }
        node = node->parent;
    }
    return (NULL);
}

/**
 * xmlNewReconciliedNs:
 * @param doc the document
 * @param tree a node expected to hold the new namespace
 * @param ns the original namespace
 *
 * This function tries to locate a namespace definition in a tree
 * ancestors, or create a new namespace definition node similar to
 * ns trying to reuse the same prefix. However if the given prefix is
 * null (default namespace) or reused within the subtree defined by
 * tree or on one of its ancestors then a new prefix is generated.
 * Returns the (new) namespace definition or NULL in case of error
 *
 * OOM: possible --> OOM flag is set when returns NULL
 */
xmlNsPtr
xmlNewReconciliedNs(xmlDocPtr doc, xmlNodePtr tree, xmlNsPtr ns) {
    xmlNsPtr def;
    xmlChar prefix[50]; 
    int counter = 1;
    LOAD_GS_SAFE_DOC(doc)

    
    if (!tree) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,"xmlNewReconciliedNs : tree == NULL\n");
#endif
        return(NULL);
    }
    if (!ns) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,"xmlNewReconciliedNs : ns == NULL\n");
#endif
        return(NULL);
    }
    /*
     * Search an existing namespace definition inherited.
     */
    def = xmlSearchNsByHref(doc, tree, ns->href); // OOM is possible
    if (def){
        return(def);
    }else{
        if(OOM_FLAG)
            return NULL;
    }

    /*
     * Find a close prefix which is not already in use.
     * Let's strip namespace prefixes longer than 20 chars !
     */
    if (!ns->prefix)
        snprintf((char*) prefix, sizeof(prefix), "default");
    else
        snprintf((char*) prefix, sizeof(prefix), "%.20s", ns->prefix);

    def = xmlSearchNs(doc, tree, prefix);
    while (def && !OOM_FLAG)
    {
        if (counter > 1000)  
            return(NULL); // giveup
        if (!ns->prefix)
            snprintf((char *) prefix, sizeof(prefix), "default%d", counter++);
        else
            snprintf((char *) prefix, sizeof(prefix), "%.20s%d", ns->prefix, counter++);
        def = xmlSearchNs(doc, tree, prefix);
    }
    if (OOM_FLAG)
          return NULL;
    /*
     * Ok, now we are ready to create a new one.
     */
    def = xmlNewNs(tree, ns->href, prefix); // may set OOM flag
    return(def);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlReconciliateNs:
 * @param doc the document
 * @param tree a node defining the subtree to reconciliate
 *
 * This function checks that all the namespaces declared within the given
 * tree are properly declared. This is needed for example after Copy or Cut
 * and then paste operations. The subtree may still hold pointers to
 * namespace declarations outside the subtree or invalid/masked. As much
 * as possible the function try to reuse the existing namespaces found in
 * the new environment. If not possible the new namespaces are redeclared
 * on tree at the top of the given subtree.
 * Returns the number of namespace declarations created or -1 in case of error.
 *
 * OOM: possible.. / NOT REVIEWED
 */

XMLPUBFUNEXPORT int
xmlReconciliateNs(xmlDocPtr doc, xmlNodePtr tree)
{
    xmlNsPtr*   oldNs = NULL;
    xmlNsPtr*   newNs = NULL;
    int         sizeCache = 0;
    int         nbCache = 0;

    //xmlNsPtr    n; // DONE: moved into inner scope
    xmlNodePtr  node = tree;
    xmlAttrPtr  attr;
    //int         ret = 0; // DONE: not needed
    int         i;

	if(node->type != XML_ELEMENT_NODE) {
		return -1;
	}
    while (node)
    {
        /*
         * Reconciliate the node namespace
         */
        if (node->ns && (node->type != XML_TEXT_NODE))
        {
            /*
             * initialize the cache if needed
             */
            if (sizeCache == 0)
            {
                
                sizeCache = 10;
                
                oldNs = (xmlNsPtr*) xmlMalloc(sizeCache * sizeof(xmlNsPtr));
                if (!oldNs)
                    goto OOM;

                newNs = (xmlNsPtr*) xmlMalloc(sizeCache * sizeof(xmlNsPtr));
                if (!newNs)
                    goto OOM;
            }
            for (i = 0;i < nbCache;i++) {
                if (oldNs[i] == node->ns) {
                    node->ns = newNs[i];
                    break;
                }
            }
            if (i == nbCache) {
                /*
                 * Ok, we need to recreate a new namespace definition
                 */
                xmlNsPtr n = xmlNewReconciliedNs(doc, tree, node->ns);
                if (n) { /* :-( what if else ??? */
                    /*
                     * check if we need to grow the cache buffers.
                     */
                    if (sizeCache <= nbCache)
                    {
                        // DONE: Fix xmlRealloc
                        xmlNsPtr* tmp;
                        sizeCache *= 2;
                        tmp = (xmlNsPtr *) xmlRealloc(oldNs, sizeCache * sizeof(xmlNsPtr));
                        if (!tmp)
                            goto OOM;
                        oldNs = tmp;
                        // DONE: Fix xmlRealloc
                        tmp = (xmlNsPtr*) xmlRealloc(newNs, sizeCache * sizeof(xmlNsPtr));
                        if (!tmp)
                            goto OOM;
                        newNs = tmp;
                    }
                    newNs[nbCache] = n;
                    oldNs[nbCache++] = node->ns;
                    node->ns = n;
                }
            }
        }
        /*
         * now check for namespace hold by attributes on the node.
         */
        if(!(IS_DATA_NODE(node))){
	        attr = node->properties;
	        while (attr)
	        {
	            if (attr->ns)
	            {
	                /*
	                 * initialize the cache if needed
	                 */
	                if (sizeCache == 0)
	                {
	                    sizeCache = 10;
	                    oldNs = (xmlNsPtr*) xmlMalloc(sizeCache * sizeof(xmlNsPtr));
	                    if (!oldNs)
	                        goto OOM;

	                    newNs = (xmlNsPtr*) xmlMalloc(sizeCache * sizeof(xmlNsPtr));
	                    if (!newNs)
	                        goto OOM;
	                }
	                for (i = 0;i < nbCache;i++) {
	                    if (oldNs[i] == attr->ns) {
	                        attr->ns = newNs[i];
	                        break;
	                    }
	                }
	                if (i == nbCache) {
	                    /*
	                     * OK we need to recreate a new namespace definition
	                     */
	                    xmlNsPtr n = xmlNewReconciliedNs(doc, tree, attr->ns);
	                    if (n)
	                    {       /* :-( what if else ??? */
	                        /*
	                         * check if we need to grow the cache buffers.
	                         */
	                        if (sizeCache <= nbCache)
	                        {
	                            // DONE: Fix xmlRealloc
	                            xmlNsPtr* tmp;
	                            sizeCache *= 2;
	                            tmp = (xmlNsPtr*) xmlRealloc(oldNs, sizeCache * sizeof(xmlNsPtr));
	                            if (!tmp)
	                                goto OOM;
	                            oldNs = tmp;
	                            // DONE: Fix xmlRealloc
	                            tmp = (xmlNsPtr*) xmlRealloc(newNs, sizeCache * sizeof(xmlNsPtr));
	                            if (!tmp)
	                                goto OOM;
	                            newNs = tmp;
	                        }
	                        newNs[nbCache] = n;
	                        oldNs[nbCache++] = attr->ns;
	                        attr->ns = n;
	                    }
	                }
	            }
	            attr = attr->next;
        	}
        }

        /*
         * Browse the full subtree, deep first
         */
    if (node->children && node->type != XML_ENTITY_REF_NODE) 
    	{
            /* deep first */
            node = node->children;
        } else if ((node != tree) && node->next) {
            /* then siblings */
            node = node->next;
        } else if (node != tree) {
            /* go up to parents->next if needed */
            while (node != tree)
            {
                if (node->parent)
                    node = node->parent;
                if ((node != tree) && node->next) {
                    node = node->next;
                    break;
                }
                if (!node->parent) {
                    node = NULL;
                    break;
                }
            }
            /* exit condition */
            if (node == tree)
                node = NULL;
        } else
            break;
    }
    if (oldNs)
        xmlFree(oldNs);
    if (newNs)
        xmlFree(newNs);
    return(0);
//------------------------
OOM:
    if (oldNs)
        xmlFree(oldNs);
    if (newNs)
        xmlFree(newNs);
    xmlTreeErrMemory(EMBED_ERRTXT("fixing namespaces"));
    return(-1);
}
#endif /* LIBXML_TREE_ENABLED */

/**
 * xmlHasProp:
 * @param node the node
 * @param name the attribute name
 *
 * Search an attribute associated to a node
 * This function also looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute or the attribute declaration or NULL if
 *         neither was found.
 *
 * OOM: possible --> iif returns NULL and OOM flag is set
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlHasProp(xmlNodePtr node, const xmlChar *name) {
    xmlAttrPtr prop;
    xmlDocPtr doc;
    LOAD_GS_SAFE_NODE(node)

    if (!node || !name)
        return(NULL);
    /*
     * Check on the properties attached to the node
     */
    prop = node->properties;
    while (prop) {
        if (xmlStrEqual(prop->name, name))  {
            return(prop);
        }
        prop = prop->next;
    }
    if (!xmlCheckDTD)
        return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc)
    {
        xmlAttributePtr attrDecl;
        if (doc->intSubset)
        {
            attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name); // OOM is possible
            if (!attrDecl)
            {
                if(OOM_FLAG)
                    return NULL;
                if(doc->extSubset)
                {
                    attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);
                    if(OOM_FLAG)
                        return NULL;
                }
            }
            if (attrDecl &&
                attrDecl->defaultValue)
            {
                /* return attribute declaration only if a default value is given
                (that includes #FIXED declarations) */
                return((xmlAttrPtr) attrDecl);
            }
        }
    }
    return(NULL);
}

/**
 * xmlHasNsProp:
 * @param node the node
 * @param name the attribute name
 * @param nameSpace the URI of the namespace
 *
 * Search for an attribute associated to a node
 * This attribute has to be anchored in the namespace specified.
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute or the attribute declaration or NULL
 *     if neither was found.
 *
 * OOM: possible --> iif returns NULL AND OOM flag is set
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlHasNsProp(xmlNodePtr node, const xmlChar *name, const xmlChar *nameSpace) {
    xmlAttrPtr prop;
	    
#ifdef LIBXML_TREE_ENABLED
    xmlDocPtr doc;
#endif /* LIBXML_TREE_ENABLED */
	LOAD_GS_SAFE_NODE(node)

    if (!node)
        return(NULL);

    if (!nameSpace)
        return(xmlHasProp(node, name)); // may set OOM flag when NULL

    prop = node->properties;
    while (prop)
    {
        /*
        * One need to have
        *   - same attribute names
        *   - and the attribute carrying that namespace
        */
        if (xmlStrEqual(prop->name, name) &&
            prop->ns                      &&   
            xmlStrEqual(prop->ns->href, nameSpace))
        {
            return(prop);
        }
        prop = prop->next;
    }
    if (!xmlCheckDTD)
        return(NULL);

#ifdef LIBXML_TREE_ENABLED
    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc && doc->intSubset)
    {
        xmlAttributePtr attrDecl = NULL;
        xmlNsPtr* nsList;
        xmlNsPtr* cur;
        xmlChar*  ename;

        nsList = xmlGetNsList(node->doc, node); //  may set OOM flag when returns NULL
        if (!nsList)
            return(NULL);

        if (node->ns && node->ns->prefix) {
            // may set OOM flag, the check is later
            ename = xmlStrdup(node->ns->prefix);
            ename = xmlStrcat(ename, BAD_CAST ":");
            ename = xmlStrcat(ename, node->name);
        } else {
            ename = xmlStrdup(node->name);
        }
        if (!ename || OOM_FLAG) {
            xmlFree(nsList);
            if (ename)
                xmlFree(ename); // XMLENGINE added: ename may be !=NULL if OOM_FLAG!=NULL
            return(NULL);
        }

        cur = nsList;
        while (*cur) {
            if (xmlStrEqual((*cur)->href, nameSpace))
            {
                attrDecl = xmlGetDtdQAttrDesc(
                                    doc->intSubset,
                                    ename,
                                    name,
                                    (*cur)->prefix);
                if (!attrDecl &&
                     doc->extSubset)
                {
                    attrDecl = xmlGetDtdQAttrDesc(
                                    doc->extSubset,
                                    ename,
                                    name,
                                    (*cur)->prefix);
                }
            }
            cur++;
        }
        xmlFree(nsList);
        xmlFree(ename);
        return((xmlAttrPtr) attrDecl);
    }
#endif /* LIBXML_TREE_ENABLED */
    return(NULL);
}

/**
 * xmlGetProp:
 * @param node the node
 * @param name the attribute name
 *
 * Search and get the value of an attribute associated to a node
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 * NOTE: this function acts independently of namespaces associated
 *       to the attribute. Use xmlGetNsProp() or xmlGetNoNsProp()
 *       for namespace aware processing.
 *
 * Returns the attribute value or NULL if not found.
 *     It's up to the caller to free the memory with xmlFree().
 */
XMLPUBFUNEXPORT xmlChar*
xmlGetProp(xmlNodePtr node, const xmlChar* name)
{
    xmlAttrPtr prop;
    xmlDocPtr doc;
    LOAD_GS_SAFE_NODE(node)

    if (!node || !name || (node->type != XML_ELEMENT_NODE))
        return(NULL);
    // REPLACED: if ((node == NULL) || (name == NULL)) return(NULL);

    /*
     * Check on the properties attached to the node
     */
    prop = node->properties;
    while (prop) {
        if (xmlStrEqual(prop->name, name))  {
            xmlChar *ret;

            ret = xmlNodeListGetString(node->doc, prop->children, 1);
            if (!ret)
                return(xmlStrdup((xmlChar *)""));
            return(ret);
        }
        prop = prop->next;
    }
    if (!xmlCheckDTD)
        return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc) {
        xmlAttributePtr attrDecl;
        if (doc->intSubset) {
            attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name);
            if (!attrDecl && doc->extSubset)
                attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);
            if (attrDecl && attrDecl->defaultValue)
            {
                /* return attribute declaration only if a default value is given
                   (that includes #FIXED declarations) */
                return(xmlStrdup(attrDecl->defaultValue));
            }
        }
    }
    return(NULL);
}

/**
 * xmlGetNoNsProp:
 * @param node the node
 * @param name the attribute name
 *
 * Search and get the value of an attribute associated to a node
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 * This function is similar to xmlGetProp except it will accept only
 * an attribute in no namespace.
 *
 * Returns the attribute value or NULL if not found.
 *     It's up to the caller to free the memory with xmlFree().
 */
XMLPUBFUNEXPORT xmlChar*
xmlGetNoNsProp(xmlNodePtr node, const xmlChar *name)
{
    xmlAttrPtr prop;
    xmlDocPtr doc;
    LOAD_GS_SAFE_NODE(node)

    if (!node || !name || (node->type != XML_ELEMENT_NODE))
        return(NULL);

    /*
     * Check on the properties attached to the node
     */
    prop = node->properties;
    while (prop) {
        if (!prop->ns && xmlStrEqual(prop->name, name))
        {
            xmlChar *ret;

            ret = xmlNodeListGetString(node->doc, prop->children, 1);
            return ret ? ret : xmlStrdup((xmlChar *)"");
        }
        prop = prop->next;
    }
    if (!xmlCheckDTD)
        return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc) {
        xmlAttributePtr attrDecl;
        if (doc->intSubset) {
            attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name);
            if (!attrDecl && doc->extSubset)
                attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);
            if (attrDecl && attrDecl->defaultValue)
            {
              /* return attribute declaration only if a default value is given
                 (that includes #FIXED declarations) */
                return(xmlStrdup(attrDecl->defaultValue));
            }
        }
    }
    return(NULL);
}

/**
 * xmlGetNsProp:
 * @param node the node
 * @param name the attribute name
 * @param nameSpace the URI of the namespace
 *
 * Search and get the value of an attribute associated to a node
 * This attribute has to be anchored in the namespace specified.
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute value or NULL if not found.
 *     It's up to the caller to free the memory with xmlFree().
 */
XMLPUBFUNEXPORT xmlChar*
xmlGetNsProp(xmlNodePtr node, const xmlChar *name, const xmlChar *nameSpace)
{
	xmlAttrPtr prop;
    xmlDocPtr doc;
    xmlNsPtr ns;
    LOAD_GS_SAFE_NODE(node)

    if (!node || node->type != XML_ELEMENT_NODE)
        return(NULL);

    if (!nameSpace)
        return(xmlGetNoNsProp(node, name));

    prop = node->properties;

    while (prop)
    {
        /*
         * One need to have
         *   - same attribute names
         *   - and the attribute carrying that namespace
         */
        if (xmlStrEqual(prop->name, name)   &&
            prop->ns                        &&  
            xmlStrEqual(prop->ns->href, nameSpace))
        {
            xmlChar* ret;

            ret = xmlNodeListGetString(node->doc, prop->children, 1);
            if (!ret)
                return(xmlStrdup((xmlChar*)""));
            return(ret);
        }
        prop = prop->next;
    }
    if (!xmlCheckDTD)
        return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc && doc->intSubset) {
        xmlAttributePtr attrDecl;

        attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name);
        if (!attrDecl && doc->extSubset)
            attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);

        if (attrDecl && attrDecl->prefix)
        {
            /*
             * The DTD declaration only allows a prefix search
             */
            ns = xmlSearchNs(doc, node, attrDecl->prefix);
            if (ns && xmlStrEqual(ns->href, nameSpace))
                return(xmlStrdup(attrDecl->defaultValue));
        }
    }
    return(NULL);
}

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlUnsetProp:
 * @param node the node
 * @param name the attribute name
 *
 * Remove an attribute carried by a node.
 * Returns 0 if successful, -1 if not found
 */
XMLPUBFUNEXPORT int
xmlUnsetProp(xmlNodePtr node, const xmlChar* name)
{
    xmlAttrPtr prop, prev;

    if (!node || !name)
        return(-1);

    prev = NULL;
    prop = node->properties;
    while (prop)
    {   
        if (!prop->ns && xmlStrEqual(prop->name, name))
        {
            if (prev)
                prev->next = prop->next;
            else
                node->properties = prop->next;
            xmlFreeProp(prop);
            return(0);
        }
        prev = prop;
        prop = prop->next;
    }
    return(-1);
}

/**
 * xmlUnsetNsProp:
 * @param node the node
 * @param ns the namespace definition
 * @param name the attribute name
 *
 * Remove an attribute carried by a node.
 * Returns 0 if successful, -1 if not found
 */
XMLPUBFUNEXPORT int
xmlUnsetNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar* name)
{
    xmlAttrPtr prop;
    xmlAttrPtr prev;

    
    if (!ns) // Note: check it first to avoid double check of 'node' and 'name' arguments
        return(xmlUnsetProp(node, name));

    if (!node || !name || !ns->href)
        return(-1);

    prop = node->properties;
    prev = NULL;

    while (prop)
    {
        if (prop->ns                      &&
            xmlStrEqual(prop->name, name) &&
            xmlStrEqual(prop->ns->href, ns->href))
        {
            if (!prev)
                node->properties = prop->next;
            else
                prev->next = prop->next;
            xmlFreeProp(prop);
            return(0);
        }
        prev = prop;
        prop = prop->next;
    }
    return(-1);
}
#endif

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_XINCLUDE_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED) || defined(LIBXML_HTML_ENABLED)
/**
 * xmlSetProp:
 * @param node the node
 * @param name the attribute name
 * @param value the attribute value
 *
 * Set (or reset) an attribute carried by a node.
 * Returns the attribute pointer.
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlSetProp(xmlNodePtr node, const xmlChar* name, const xmlChar* value)
{
    xmlAttrPtr prop;
    xmlDocPtr doc;

    if (!node || !name || (node->type != XML_ELEMENT_NODE))
        return(NULL);

    doc = node->doc;
    prop = node->properties;
    while (prop)
    {
        if (!prop->ns && xmlStrEqual(prop->name, name)) // ISSUE: need to check ns->href ?
        {
            xmlNodePtr oldprop = prop->children;

            prop->children = NULL;
            prop->last = NULL;
            if (value)
            {
                xmlChar*   buffer;
                xmlNodePtr tmp;

                buffer = xmlEncodeEntitiesReentrant(node->doc, value);
                prop->children = xmlStringGetNodeList(node->doc, buffer);
                prop->last = NULL;
                prop->doc = doc;
                tmp = prop->children;

                while (tmp) {
                    tmp->parent = (xmlNodePtr) prop;
                    tmp->doc = doc;
                    if (!tmp->next)
                        prop->last = tmp;
                    tmp = tmp->next;
                }
                xmlFree(buffer);
            }
            if (oldprop)
                xmlFreeNodeList(oldprop);

            return(prop);
        }
        prop = prop->next;
    }
    prop = xmlNewProp(node, name, value);
    return(prop);
}

/**
 * xmlSetNsProp:
 * @param node the node
 * @param ns the namespace definition
 * @param name the attribute name
 * @param value the attribute value
 *
 * Set (or reset) an attribute carried by a node.
 * The ns structure must be in scope, this is not checked.
 *
 * Returns the attribute pointer.
 */
XMLPUBFUNEXPORT xmlAttrPtr
xmlSetNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar* name, const xmlChar* value)
{
    xmlAttrPtr prop;

    if (!ns) 
        return(xmlSetProp(node, name, value));

    if (!node || !name || !ns->href || (node->type != XML_ELEMENT_NODE))
        return(NULL);

    prop = node->properties;

    while (prop)
    {
        /*
         * One need to have
         *   - same attribute names
         *   - and the attribute carrying that namespace
         */
        if (prop->ns                        &&
            xmlStrEqual(prop->name, name)   &&
            xmlStrEqual(prop->ns->href, ns->href))
        {
            if (prop->children)
                xmlFreeNodeList(prop->children);
            prop->children = NULL;
            prop->last = NULL;
            prop->ns = ns;
            if (value)
            {
                xmlChar*   buffer;
                xmlNodePtr tmp;

                buffer = xmlEncodeEntitiesReentrant(node->doc, value);
                prop->children = xmlStringGetNodeList(node->doc, buffer);
                prop->last = NULL;
                tmp = prop->children;
                while (tmp) {
                    tmp->parent = (xmlNodePtr) prop;
                    if (!tmp->next)
                        prop->last = tmp;
                    tmp = tmp->next;
                }
                xmlFree(buffer);
            }
            return(prop);
        }
        prop = prop->next;
    }
    prop = xmlNewNsProp(node, ns, name, value);
    return(prop);
}

#endif /* LIBXML_TREE_ENABLED */


// #define xmlNodeIsText(node) ((node) && (node)->type == XML_TEXT_NODE)
/**
 * xmlNodeIsText:
 * @param node the node
 *
 * Is this node a Text node ?
 * Returns 1 yes, 0 no
 */
XMLPUBFUNEXPORT int
xmlNodeIsText(xmlNodePtr node)
{
    return (node && node->type == XML_TEXT_NODE) ? 1 : 0; // Do we need exactly 0 and 1 ?
}

/**
 * xmlIsBlankNode:
 * @param node the node
 *
 * Checks whether this node is an empty or whitespace only
 * (and possibly ignorable) text-node.
 *
 * Returns 1 yes, 0 no
 */
XMLPUBFUNEXPORT int
xmlIsBlankNode(xmlNodePtr node)
{
    const xmlChar* cur;
    if (!node ||
          (node->type != XML_TEXT_NODE &&
           node->type != XML_CDATA_SECTION_NODE))
        return(0);

    if (!node->content)
        return(1);

    cur = node->content;
    while (*cur != 0) {
        if (!IS_BLANK_CH(*cur))
            return(0);
        cur++;
    }

    return(1);
}

/**
 * xmlTextConcat:
 * @param node the node
 * @param content the content
 * @param len content length
 *
 * Concat the given string at the end of the existing node content
 *
 * Returns -1 in case of error, 0 otherwise
 */

XMLPUBFUNEXPORT int
xmlTextConcat(xmlNodePtr node, const xmlChar *content, int len)
{
    if (!node)
        return(-1);

    if ((node->type != XML_TEXT_NODE) &&
        (node->type != XML_CDATA_SECTION_NODE))
    {
#ifdef DEBUG_TREE
    xmlGenericError(xmlGenericErrorContext,
        "xmlTextConcat: node is not text nor CDATA\n");
#endif
        return(-1);
    }
    /* need to check if content is currently in the dictionary */
    if (node->doc && node->doc->dict &&
        xmlDictOwns(node->doc->dict, node->content))
    {
        node->content = xmlStrncatNew(node->content, content, len);
    } else {
        node->content = xmlStrncat(node->content, content, len);
    }
    if (!node->content)
        return(-1); // OOM!
    return(0);
}

/************************************************************************
 *                                                                      *
 *          Output : to a FILE or in memory                             *
 *                                                                      *
 ************************************************************************/

/**
 * xmlBufferCreate:
 *
 * routine to create an XML buffer.
 * returns the new structure.
 *
 * OOM: possible --> NULL is returned and OOM flag is set
 */
XMLPUBFUNEXPORT xmlBufferPtr
xmlBufferCreate(void)
{
	LOAD_GS_DIRECT
    xmlBufferPtr ret;

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (!ret) {
        xmlTreeErrMemory(EMBED_ERRTXT("creating buffer"));
        return(NULL);
    }
    ret->use = 0;
    ret->size = xmlDefaultBufferSize;
    ret->alloc = xmlBufferAllocScheme;
    ret->content = (xmlChar*) xmlMallocAtomic(ret->size * sizeof(xmlChar));
    if (!ret->content) {
        xmlTreeErrMemory(EMBED_ERRTXT("creating buffer"));
        xmlFree(ret);
        return(NULL);
    }
    ret->content[0] = 0;
    return(ret);
}

/**
 * xmlBufferCreateSize:
 * @param size initial size of buffer
 *
 * routine to create an XML buffer.
 * returns the new structure.
 *
 * OOM: possible --> NULL is returned && OOM flag is set
 */
XMLPUBFUNEXPORT xmlBufferPtr
xmlBufferCreateSize(size_t size)
{
	LOAD_GS_DIRECT
    xmlBufferPtr ret;
    
    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (!ret)
        goto OOM_exit;
    ret->use = 0;
    ret->alloc = xmlBufferAllocScheme;
    ret->size = (size ? size+2 : 0);         /* +1 for ending null */
    if (ret->size){
        ret->content = (xmlChar*) xmlMallocAtomic(ret->size * sizeof(xmlChar));
        if (!ret->content) {
            xmlFree(ret);
OOM_exit:
            xmlTreeErrMemory(EMBED_ERRTXT("creating buffer")); // sets OOM flag
            return(NULL);
        }
        ret->content[0] = 0;
    } else
        ret->content = NULL;
    return(ret);
}

/**
 * xmlBufferCreateStatic:
 * @param mem the memory area
 * @param size the size in byte
 *
 * routine to create an XML buffer from an immutable memory area.
 * The area won't be modified nor copied, and is expected to be
 * present until the end of the buffer lifetime.
 *
 * returns the new structure.
 */
XMLPUBFUNEXPORT xmlBufferPtr
xmlBufferCreateStatic(void* mem, size_t size)
{
    xmlBufferPtr ret;

    if (!mem || (size == 0))
        return(NULL);

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (!ret) {
        xmlTreeErrMemory(EMBED_ERRTXT("creating buffer"));
        return(NULL);
    }
    ret->use = size;
    ret->size = size;
    ret->alloc = XML_BUFFER_ALLOC_IMMUTABLE;
    ret->content = (xmlChar*) mem;
    return(ret);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlBufferSetAllocationScheme:
 * @param buf the buffer to tune
 * @param scheme allocation scheme to use
 *
 * Sets the allocation scheme for this buffer
 */
void
xmlBufferSetAllocationScheme(xmlBufferPtr buf,
                             xmlBufferAllocationScheme scheme)
{
    if (!buf) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferSetAllocationScheme: buf == NULL\n");
#endif
        return;
    }
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return;

    buf->alloc = scheme;
}

#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlBufferFree:
 * @param buf the buffer to free
 *
 * Frees an XML buffer. It frees both the content and the structure which
 * encapsulate it.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlBufferFree(xmlBufferPtr buf)
{
    if (!buf) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferFree: buf == NULL\n");
#endif
        return;
    }

    if (buf->content &&
        buf->alloc != XML_BUFFER_ALLOC_IMMUTABLE)
    {
        xmlFree(buf->content);
    }
    xmlFree(buf);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlBufferEmpty:
 * @param buf the buffer
 *
 * empty a buffer.
 */
void
xmlBufferEmpty(xmlBufferPtr buf)
{
    if (!buf->content)
        return;

    buf->use = 0;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) {
        buf->content = BAD_CAST "";
    } else {
        memset(buf->content, 0, buf->size);
    }
}

#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlBufferShrink:
 * @param buf the buffer to dump
 * @param len the number of xmlChar to remove
 *
 * Remove the beginning of an XML buffer.
 *
 * Returns the number of #xmlChar removed, or -1 in case of failure.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT int
xmlBufferShrink(xmlBufferPtr buf, unsigned int len)
{
    if (len == 0)
        return(0);
    if (len > buf->use)
        return(-1);

    buf->use -= len;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) {
        buf->content += len;
    } else {
        memmove(buf->content, &buf->content[len], buf->use * sizeof(xmlChar));
        buf->content[buf->use] = 0;
    }
    return(len);
}

/**
 * xmlBufferGrow:
 * @param buf the buffer
 * @param len the minimum free size to allocate
 *
 * Grow the available space of an XML buffer.
 *
 * Returns the new available space or -1 in case of error
 *
 * OOM: possible --> returns -1 && sets OOM flag
 */
XMLPUBFUNEXPORT int
xmlBufferGrow(xmlBufferPtr buf, unsigned int len)
{
    int      size;
    xmlChar* newbuf;

    if ((buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)  ||
        (len + buf->use < buf->size))
    {
         return(0);
    }
    size = buf->use + len + 100;

    newbuf = (xmlChar*) xmlRealloc(buf->content, size);
    if (!newbuf) {
        xmlTreeErrMemory(EMBED_ERRTXT("growing buffer"));
        return(-1);
    }
    buf->content = newbuf;
    buf->size = size;
    return(buf->size - buf->use);
}

#ifndef XMLENGINE_EXCLUDE_FILE_FUNC
/**
 * xmlBufferDump:
 * @param file the file output
 * @param buf the buffer to dump
 *
 * Dumps an XML buffer to  a FILE *.
 * Returns the number of #xmlChar written
 */
int
xmlBufferDump(FILE* file, xmlBufferPtr buf)
{
    int ret;

    if (!buf) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
                        "xmlBufferDump: buf == NULL\n");
#endif
        return(0);
    }
    if (!buf->content) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
                        "xmlBufferDump: buf->content == NULL\n");
#endif
        return(0);
    }
    if (!file)
        file = stdout;

    ret = fwrite(buf->content, sizeof(xmlChar), buf->use, file);
    if(ret == EOF)
    {
        CHECK_ERRNO;
    }
    return(ret);
}
#endif

// XMLENGINE: NOTE: replaced by macro
/**
 * xmlBufferContent:
 * @param buf the buffer
 *
 * Function to extract the content of a buffer
 *
 * Returns the internal content
 */
/*
const xmlChar *
xmlBufferContent(const xmlBufferPtr buf)
{
    if(!buf)
        return NULL;

    return buf->content;
}
*/

/**
 * xmlBufferLength:
 * @param buf the buffer
 *
 * Function to get the length of a buffer
 *
 * Returns the length of data in the internal content
 */
XMLPUBFUNEXPORT int
xmlBufferLength(const xmlBufferPtr buf)
{
    if(!buf)
        return 0;

    return buf->use;
}

/**
 * xmlBufferResize:
 * @param buf the buffer to resize
 * @param size the desired size
 *
 * Resize a buffer to accommodate minimum size of size.
 *
 * Returns  0 in case of problems, 1 otherwise
 *
 * OOM: possible --> OOM flag is set && returns 0
 */
XMLPUBFUNEXPORT int
xmlBufferResize(xmlBufferPtr buf, unsigned int size)
{
    unsigned int newSize;
    xmlChar* rebuf = NULL;

	/*
	 *   SYMBIAN DEF130695 FIX : Ensuring the BUFFER that needs to be re-sized is valid or not.
	 */
	if(buf == NULL)
		return (0);
		 

    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return(0);

    /* Don't resize if we don't have to */
    if (size < buf->size)
        return 1;

    /* figure out new size */
    switch (buf->alloc)
    {
    case XML_BUFFER_ALLOC_DOUBLEIT:
    /*take care of empty case*/
        newSize = (buf->size ? buf->size*2 : size + 10);
        while (size > newSize)
            newSize *= 2;
        break;
    // NOTE: same as default case
    //case XML_BUFFER_ALLOC_EXACT:
    //    newSize = size+10;
    //    break;
    default:
        newSize = size+10;
        break;
    }

    if (!buf->content)
    {
        rebuf = (xmlChar*) xmlMallocAtomic(newSize * sizeof(xmlChar));
    }
    else if (buf->size - buf->use < 100)
    {
        rebuf = (xmlChar*) xmlRealloc(buf->content, newSize * sizeof(xmlChar));
    }
    else
    {
        /*
         * if we are reallocating a buffer far from being full, it's
         * better to make a new allocation and copy only the used range
         * and free the old one.
         */
        rebuf = (xmlChar*) xmlMallocAtomic(newSize * sizeof(xmlChar));
        if (rebuf) {
            memcpy(rebuf, buf->content, buf->use);
            xmlFree(buf->content);
            rebuf[buf->use] = 0;
        }
    }
    if (!rebuf) {
        xmlTreeErrMemory(EMBED_ERRTXT("growing buffer"));
        return 0;
    }
    buf->content = rebuf;
    buf->size = newSize;

    return 1;
}

/**
 * xmlBufferAdd:
 * @param buf the buffer to dump
 * @param str the #xmlChar string
 * @param len the number of #xmlChar to add
 *
 * Add a string range to an XML buffer. if len == -1, the length of
 * str is recomputed.
 *
 * OOM: possible --> check OOM flag
 */
XMLPUBFUNEXPORT void
xmlBufferAdd(xmlBufferPtr buf, const xmlChar* str, int len)
{
    unsigned int needSize;

    if (!str) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferAdd: str == NULL\n");
#endif
        return;
    }
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE || len == 0)
        return;

    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferAdd: len < 0\n");
#endif
        return;
    }

    if (len < 0){
        len = xmlStrlen(str);
        if (len <= 0)
            return;
    }

    needSize = buf->use + len + 2;
    if ( (needSize > buf->size)          &&
         !xmlBufferResize(buf, needSize) )
    {
        xmlTreeErrMemory(EMBED_ERRTXT("growing buffer"));
        return;
    }

    memmove(&buf->content[buf->use], str, len * sizeof(xmlChar));
    buf->use += len;
    buf->content[buf->use] = 0;
}

/**
 * xmlBufferAddHead:
 * @param buf the buffer
 * @param str the #xmlChar string
 * @param len the number of #xmlChar to add
 *
 * Add a string range to the beginning of an XML buffer.
 * if len == -1, the length of str is recomputed.
 *
 * OOM: possible --> OOM flag is set
 */
XMLPUBFUNEXPORT void
xmlBufferAddHead(xmlBufferPtr buf, const xmlChar* str, int len)
{
    unsigned int needSize;
    
    if (len == 0 ||
        buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
    {
        return;
    }

    if (!str) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferAddHead: str == NULL\n");
#endif
        return;
    }
    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferAddHead: len < 0\n");
#endif
        return;
    }

    if (len < 0){  // if (len == -1)
        len = xmlStrlen(str);
        if (len <= 0)
            return;
    }

    needSize = buf->use + len + 2;
    if ( needSize > buf->size            &&
         !xmlBufferResize(buf, needSize) )
    {
        xmlTreeErrMemory(EMBED_ERRTXT("growing buffer"));
        return;
    }

    memmove(&buf->content[len], &buf->content[0], buf->use * sizeof(xmlChar));
    memmove(&buf->content[0], str, len * sizeof(xmlChar));
    buf->use += len;
    buf->content[buf->use] = 0;
}

/**
 * xmlBufferCat:
 * @param buf the buffer to dump
 * @param str the #xmlChar string
 *
 * Append a zero terminated string to an XML buffer.
 *
 * OOM: possible / NOT REVIEWED
 */
XMLPUBFUNEXPORT void
xmlBufferCat(xmlBufferPtr buf, const xmlChar* str)
{
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return;
    if (str)
        xmlBufferAdd(buf, str, -1);
}

/**
 * xmlBufferCCat:
 * @param buf the buffer to dump
 * @param str the C char string
 *
 * Append a zero terminated C string to an XML buffer.
 */
XMLPUBFUNEXPORT void
xmlBufferCCat(xmlBufferPtr buf, const char* str)
{
    const char *cur;

    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return;
    if (!str) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext, "xmlBufferCCat: str == NULL\n");
#endif
        return;
    }
    for (cur = str; *cur != 0; cur++) {
        if (buf->use + 10 >= buf->size   &&
            !xmlBufferResize(buf, buf->use+10))
        {
            xmlTreeErrMemory(EMBED_ERRTXT("growing buffer"));
            return;
        }
        buf->content[buf->use++] = *cur;
    }
    buf->content[buf->use] = 0;
}


/**
 * xmlBufferWriteCHAR:
 * @param buf the XML buffer
 * @param string the string to add
 *
 * routine which manages and grows an output buffer. This one adds
 * xmlChars at the end of the buffer.
 */
XMLPUBFUNEXPORT void
xmlBufferWriteCHAR(xmlBufferPtr buf, const xmlChar* string)
{
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return;
    xmlBufferCat(buf, string);
}

/**
 * xmlBufferWriteChar:
 * @param buf the XML buffer output
 * @param string the string to add
 *
 * routine which manage and grows an output buffer. This one add
 * C chars at the end of the array.
 */
XMLPUBFUNEXPORT void
xmlBufferWriteChar(xmlBufferPtr buf, const char* string)
{
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return;
    xmlBufferCCat(buf, string);
}


/**
 * xmlBufferWriteQuotedString:
 * @param buf the XML buffer output
 * @param string the string to add
 *
 * routine which manage and grows an output buffer. This one writes
 * a quoted or double quoted #xmlChar string, checking first if it holds
 * quote or double-quotes internally
 */
XMLPUBFUNEXPORT void
xmlBufferWriteQuotedString(xmlBufferPtr buf, const xmlChar* string)
{
    const xmlChar* cur;
    const xmlChar* base;

    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE)
        return;
    if (xmlStrchr(string, '\"')) {
        if (xmlStrchr(string, '\'')) {
#ifdef DEBUG_BUFFER
            xmlGenericError(xmlGenericErrorContext,
 "xmlBufferWriteQuotedString: string contains quote and double-quotes !\n");
#endif
            xmlBufferCCat(buf, "\"");
            base = cur = string;
            while(*cur != 0){
                if(*cur == '"'){
                    if (base != cur)
                        xmlBufferAdd(buf, base, cur - base);
                    xmlBufferAdd(buf, BAD_CAST "&quot;", 6);
                    cur++;
                    base = cur;
                } else {
                    cur++;
                }
            }
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
            xmlBufferCCat(buf, "\"");

        } else {
            xmlBufferCCat(buf, "\'");
            xmlBufferCat(buf, string);
            xmlBufferCCat(buf, "\'");
        }
    } else {
        xmlBufferCCat(buf, "\"");
        xmlBufferCat(buf, string);
        xmlBufferCCat(buf, "\"");
    }
}

/**
 * xmlSetDocCompressMode:
 * @param doc the document
 * @param mode the compression ratio
 *
 * set the compression ratio for a document, ZLIB based
 * Correct values: 0 (uncompressed) to 9 (max compression)
 */
XMLPUBFUNEXPORT void
xmlSetDocCompressMode (xmlDocPtr doc, int mode)
{
    if (!doc)
        return;
    if (mode < 0)
        doc->compression = 0;
    else
        if (mode > 9)
            doc->compression = 9;
        else
            doc->compression = mode;
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlGetDocCompressMode:
 * @param doc the document
 *
 * get the compression ratio for a document, ZLIB based
 * Returns 0 (uncompressed) to 9 (max compression)
 */
int
xmlGetDocCompressMode (xmlDocPtr doc)
{
    if (!doc)
        return(-1);
    return(doc->compression);
}

/**
 * xmlGetCompressMode:
 *
 * get the default compression mode used, ZLIB based.
 * Returns 0 (uncompressed) to 9 (max compression)
 */
int
xmlGetCompressMode(void)
{
    return (xmlCompressMode);
}

/**
 * xmlSetCompressMode:
 * @param mode the compression ratio
 *
 * set the default compression mode used, ZLIB based
 * Correct values: 0 (uncompressed) to 9 (max compression)
 */
void
xmlSetCompressMode(int mode)
{
    if (mode < 0)
        xmlCompressMode = 0;
    else if (mode > 9)
        xmlCompressMode = 9;
    else
        xmlCompressMode = mode;
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * Appends data container pointer to the list
 *
 */	
XMLPUBFUNEXPORT int 
xmlAppendDataList(xmlNodePtr node, xmlDocPtr doc)
{
	int max; 
	int i =0;
	int k = 0;
	const int factor = 2;	
	xmlNodePtr* tmp;
	
	if(!doc)
		return -1;
	
	max = doc->dataNodeMax;
	
	while( doc->dataNodeList[i] && i < max )
		{
		if(doc->dataNodeList[i] == node){
			return -1;
			}
		++i;		
		}	
	if( i >= max - 1) //leave out one space for NULL at the end of array
		{
		doc->dataNodeMax = max * factor;
		tmp = (xmlNodePtr*) xmlRealloc(doc->dataNodeList, doc->dataNodeMax * sizeof(xmlNodePtr));
		if(!tmp)
			{
			return -1;
			}
		doc->dataNodeList = tmp;
		for(k = i; k < doc->dataNodeMax; k++)
			{
			doc->dataNodeList[k] = NULL;
			}
		}
	doc->dataNodeList[i] = node;
	return 0;	
}
/**
 * Removes data container from the list
 *
 */
int  
xmlRemoveFromDataList(xmlNodePtr node, xmlDocPtr doc)
{
	int i = 0;
	int j = 0;
	int size = 0;
	int ret = -1; //not found
		
	if(!doc)
		return 0;
	
	while( doc->dataNodeList[i] != NULL)
		{		
		if(doc->dataNodeList[i] == node)
			{
			j = i;
			while( doc->dataNodeList[j] != NULL )
				{
				doc->dataNodeList[j] = doc->dataNodeList[j + 1];
				++j;
				}
			ret = 0;
			}
		else
			{
			++i;	
			}
		}
	return ret;	
}
/**
 * If found, replaces data container in the list with the new one
 *
 */
int  
xmlReplaceInDataList(xmlNodePtr old, xmlNodePtr cur, xmlDocPtr doc)
{
	int i = 0;
		
	if(!doc)
		return 0;
	
	while( doc->dataNodeList[i] != NULL)
		{		
		if(doc->dataNodeList[i] == old)
			{
			doc->dataNodeList[i] = cur;
			return 0;
			}
		++i;	
		}
	return -1;	
}
