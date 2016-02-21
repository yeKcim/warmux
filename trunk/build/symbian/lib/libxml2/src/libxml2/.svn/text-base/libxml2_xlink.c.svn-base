/*
 * libxml2_xlink.c : implementation of the hyperlinks detection module
 *                   This version supports both XML XLinks and HTML simple links
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#ifdef LIBXML_XPTR_ENABLED
#include <string.h> /* for memset() only */
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

#include <libxml2_xmlmemory.h>
#include <libxml2_tree.h>
#include <libxml2_parser.h>
#include <libxml2_valid.h>
#include <libxml2_xlink.h>
#include <libxml2_globals.h>

#define XLINK_NAMESPACE (BAD_CAST "http://www.w3.org/1999/xlink/namespace/")
#define XHTML_NAMESPACE (BAD_CAST "http://www.w3.org/1999/xhtml/")

/****************************************************************
 *                                                              *
 *           Default setting and related functions              *
 *                                                              *
 ****************************************************************/

/**
 * xlinkGetDefaultHandler:
 *
 * Get the default xlink handler.
 *
 * Returns the current xlinkHandlerPtr value.
 */
xlinkHandlerPtr
xlinkGetDefaultHandler(void) {
    return(xlinkDefaultHandler);
}


/**
 * xlinkSetDefaultHandler:
 * @param handler the new value for the xlink handler block
 *
 * Set the default xlink handlers
 */
void
xlinkSetDefaultHandler(xlinkHandlerPtr handler) {
    xlinkDefaultHandler = handler;
}

/**
 * xlinkGetDefaultDetect:
 *
 * Get the default xlink detection routine
 *
 * Returns the current function or NULL;
 */
xlinkNodeDetectFunc
xlinkGetDefaultDetect   (void) {
    return(xlinkDefaultDetect);
}

/**
 * xlinkSetDefaultDetect:
 * @param func pointer to the new detection routine.
 *
 * Set the default xlink detection routine
 */
void
xlinkSetDefaultDetect   (xlinkNodeDetectFunc func) {
    xlinkDefaultDetect = func;
}

/****************************************************************
 *                                                              *
 *                  The detection routines                      *
 *                                                              *
 ****************************************************************/


/**
 * xlinkIsLink:
 * @param doc the document containing the node
 * @param node the node pointer itself
 *
 * Check whether the given node carries the attributes needed
 * to be a link element (or is one of the linking elements issued
 * from the (X)HTML DtDs).
 * This routine don't try to do full checking of the link validity
 * but tries to detect and return the appropriate link type.
 *
 * Returns the xlinkType of the node (XLINK_TYPE_NONE if there is no
 *         link detected.
 */
xlinkType
xlinkIsLink     (xmlDocPtr doc, xmlNodePtr node) {
    xmlChar *type = NULL, *role = NULL;
    xlinkType ret = XLINK_TYPE_NONE;

    if (node == NULL) return(XLINK_TYPE_NONE);
    if (doc == NULL) doc = node->doc;
    if ((doc != NULL) && (doc->type == XML_HTML_DOCUMENT_NODE)) {
        /*
         * This is an HTML document.
         */
    } else if ((node->ns != NULL) &&
               (xmlStrEqual(node->ns->href, XHTML_NAMESPACE))) {
        /*
         * !!!! We really need an IS_XHTML_ELEMENT function from HTMLtree.h @@@
         */
        /*
         * This is an XHTML element within an XML document
         * Check whether it's one of the element able to carry links
         * and in that case if it holds the attributes.
         */
    }

    /*
     * We don't prevent a-priori having XML Linking constructs on
     * XHTML elements
     */
    type = xmlGetNsProp(node, BAD_CAST"type", XLINK_NAMESPACE);
    if (type != NULL) {
        if (xmlStrEqual(type, BAD_CAST "simple")) {
            ret = XLINK_TYPE_SIMPLE;
        } if (xmlStrEqual(type, BAD_CAST "extended")) {
            role = xmlGetNsProp(node, BAD_CAST "role", XLINK_NAMESPACE);
            if (role != NULL) {
                xmlNsPtr xlink;
                xlink = xmlSearchNs(doc, node, XLINK_NAMESPACE);
                if (xlink == NULL) {
                    /* Humm, fallback method */
                    if (xmlStrEqual(role, BAD_CAST"xlink:external-linkset"))
                        ret = XLINK_TYPE_EXTENDED_SET;
                } else {
                    xmlChar buf[200];
                    snprintf((char *) buf, sizeof(buf), "%s:external-linkset",
                             (char *) xlink->prefix);
                    buf[sizeof(buf) - 1] = 0;
                    if (xmlStrEqual(role, buf))
                        ret = XLINK_TYPE_EXTENDED_SET;

                }

            }
            ret = XLINK_TYPE_EXTENDED;
        }
    }

    if (type != NULL) xmlFree(type);
    if (role != NULL) xmlFree(role);
    return(ret);
}
#endif /* LIBXML_XPTR_ENABLED */
