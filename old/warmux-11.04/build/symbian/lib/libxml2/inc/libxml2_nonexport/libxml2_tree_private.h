/*
 * Summary: tree private header
 * Description: this module describes the structures found in an tree resulting
 *              from an XML or HTML parsing, as well as the API provided for
 *              various processing on that tree
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/**
 @file
 @internalTechnology
 @released
*/
#ifndef LIBXML2_TREE_PRIVATE_H
#define LIBXML2_TREE_PRIVATE_H

#include <libxml2_tree.h>

#ifdef __cplusplus
extern "C" {
#endif

XMLPUBFUN xmlNodePtr XMLCALL xmlStaticCopyNode(const xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent, int extended);

XMLPUBFUN xmlNodePtr XMLCALL xmlStaticCopyNodeList(xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent);

XMLPUBFUN int XMLCALL xmlAppendDataList(xmlNodePtr node, xmlDocPtr doc);

/* Internal to Libxml2 dll functions - start */

xmlNodePtr XMLCALL xmlNewDocPI(xmlDocPtr doc, const xmlChar *name, const xmlChar *content);

/* Handling RChunks */
int XMLCALL xmlReplaceInDataList(xmlNodePtr old, xmlNodePtr cur, xmlDocPtr doc);
int XMLCALL xmlRemoveFromDataList(xmlNodePtr node, xmlDocPtr doc); 

/* Internal to Libxml2 dll functions - end */

#ifdef __cplusplus
}
#endif

#endif /*LIBXML2_TREE_PRIVATE_H*/
