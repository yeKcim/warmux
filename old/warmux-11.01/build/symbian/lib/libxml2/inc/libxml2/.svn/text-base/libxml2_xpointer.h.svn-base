/*
 * Summary: API to handle XML Pointers
 * Description: API to handle XML Pointers
 * Base implementation was made accordingly to
 * W3C Candidate Recommendation 7 June 2000
 * http://www.w3.org/TR/2000/CR-xptr-20000607
 *
 * Added support for the element() scheme described in:
 * W3C Proposed Recommendation 13 November 2002
 * http://www.w3.org/TR/2002/PR-xptr-element-20021113/
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

#ifndef XML_XPTR_H
#define XML_XPTR_H

#include <stdapis/libxml2/libxml2_xmlversion.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xpath.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * A Location Set
 */
typedef struct _xmlLocationSet xmlLocationSet;
typedef xmlLocationSet *xmlLocationSetPtr;
struct _xmlLocationSet {
    int locNr;                /* number of locations in the set */
    int locMax;               /* size of the array as allocated */
    xmlXPathObjectPtr *locTab;/* array of locations */
};

/*
 * Handling of location sets.
 */

XMLPUBFUN xmlLocationSetPtr XMLCALL
                    xmlXPtrLocationSetCreate    (xmlXPathObjectPtr val);
XMLPUBFUN void XMLCALL
                    xmlXPtrFreeLocationSet      (xmlLocationSetPtr obj);
XMLPUBFUN xmlLocationSetPtr XMLCALL
                    xmlXPtrLocationSetMerge     (xmlLocationSetPtr val1,
                                                 xmlLocationSetPtr val2);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewRange             (xmlNodePtr start,
                                                 int startindex,
                                                 xmlNodePtr end,
                                                 int endindex);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewRangePoints       (xmlXPathObjectPtr start,
                                                 xmlXPathObjectPtr end);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewRangeNodePoint    (xmlNodePtr start,
                                                 xmlXPathObjectPtr end);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewRangePointNode    (xmlXPathObjectPtr start,
                                                 xmlNodePtr end);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewRangeNodes        (xmlNodePtr start,
                                                 xmlNodePtr end);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewLocationSetNodes  (xmlNodePtr start,
                                                 xmlNodePtr end);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewLocationSetNodeSet(xmlNodeSetPtr set);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewRangeNodeObject   (xmlNodePtr start,
                                                 xmlXPathObjectPtr end);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrNewCollapsedRange    (xmlNodePtr start);
XMLPUBFUN void XMLCALL
                    xmlXPtrLocationSetAdd       (xmlLocationSetPtr cur,
                                                 xmlXPathObjectPtr val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrWrapLocationSet      (xmlLocationSetPtr val);
XMLPUBFUN void XMLCALL
                    xmlXPtrLocationSetDel       (xmlLocationSetPtr cur,
                                                 xmlXPathObjectPtr val);
XMLPUBFUN void XMLCALL
                    xmlXPtrLocationSetRemove    (xmlLocationSetPtr cur,
                                                 int val);

/*
 * Functions.
 */
XMLPUBFUN xmlXPathContextPtr XMLCALL
                    xmlXPtrNewContext           (xmlDocPtr doc,
                                                 xmlNodePtr here,
                                                 xmlNodePtr origin);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
                    xmlXPtrEval                 (const xmlChar *str,
                                                 xmlXPathContextPtr ctx);
XMLPUBFUN void XMLCALL
                    xmlXPtrRangeToFunction      (xmlXPathParserContextPtr ctxt,
                                                 int nargs);
XMLPUBFUN xmlNodePtr XMLCALL
                    xmlXPtrBuildNodeList        (xmlXPathObjectPtr obj);
XMLPUBFUN void XMLCALL
                    xmlXPtrEvalRangePredicate   (xmlXPathParserContextPtr ctxt);
#ifdef __cplusplus
}
#endif
#endif /* XML_XPTR_H */
