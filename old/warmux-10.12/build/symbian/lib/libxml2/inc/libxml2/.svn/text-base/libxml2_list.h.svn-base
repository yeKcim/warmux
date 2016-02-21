/*
 * Summary: lists interfaces
 * Description: this module implement the list support used in
 * various place in the library.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Gary Pennington <Gary.Pennington@uk.sun.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/** @file
@publishedAll
@released
*/

#ifndef XML_LIST_H
#define XML_LIST_H

#include <stdapis/libxml2/libxml2_xmlversion.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xmlLink xmlLink;
typedef xmlLink *xmlLinkPtr;

typedef struct _xmlList xmlList;
typedef xmlList *xmlListPtr;

/**
 * xmlListDeallocator:
 * @param lk the data to deallocate
 *
 * Callback function used to free data from a list.
 */
typedef void (*xmlListDeallocator) (xmlLinkPtr lk);
/**
 * xmlListDataCompare:
 * @param data0 the first data
 * @param data1 the second data
 *
 * Callback function used to compare 2 data.
 *
 * Returns 0 is equality, -1 or 1 otherwise depending on the ordering.
 */
typedef int  (*xmlListDataCompare) (const void *data0, const void *data1);
/**
 * xmlListWalker:
 * @param data the data found in the list
 * @param user extra user provided data to the walker
 *
 * Callback function used when walking a list with xmlListWalk().
 *
 * Returns 0 to stop walking the list, 1 otherwise.
 */
typedef int (*xmlListWalker) (const void *data, const void *user);

/* Creation/Deletion */
XMLPUBFUN xmlListPtr XMLCALL
        xmlListCreate       (xmlListDeallocator deallocator, xmlListDataCompare compare);
XMLPUBFUN void XMLCALL
        xmlListDelete       (xmlListPtr l);

/* Basic Operators */
XMLPUBFUN int XMLCALL
        xmlListInsert       (xmlListPtr l, void *data) ;
XMLPUBFUN int XMLCALL
        xmlListAppend       (xmlListPtr l, void *data) ;
XMLPUBFUN int XMLCALL
        xmlListRemoveFirst  (xmlListPtr l, void *data);
XMLPUBFUN void XMLCALL
        xmlListClear        (xmlListPtr l);
XMLPUBFUN int XMLCALL
        xmlListEmpty        (xmlListPtr l);
XMLPUBFUN xmlLinkPtr XMLCALL
        xmlListFront        (xmlListPtr l);

#ifndef XMLENGINE_EXCLUDE_UNUSED
XMLPUBFUN xmlLinkPtr XMLCALL xmlListEnd     (xmlListPtr l);
XMLPUBFUN void  XMLCALL xmlListReverse      (xmlListPtr l);
XMLPUBFUN void  XMLCALL xmlListPopBack      (xmlListPtr l);
XMLPUBFUN void  XMLCALL xmlListSort         (xmlListPtr l);
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

XMLPUBFUN void* XMLCALL xmlListSearch       (xmlListPtr l, void *data);

#ifndef XMLENGINE_EXCLUDE_UNUSED
XMLPUBFUN void* XMLCALL xmlListReverseSearch(xmlListPtr l, void *data);
XMLPUBFUN int   XMLCALL xmlListRemoveLast   (xmlListPtr l, void *data);
XMLPUBFUN int   XMLCALL xmlListRemoveAll    (xmlListPtr l, void *data);
XMLPUBFUN int   XMLCALL xmlListPushBack     (xmlListPtr l, void *data);
XMLPUBFUN void  XMLCALL xmlListReverseWalk  (xmlListPtr l,
                                             xmlListWalker walker,
                                             const void *user);
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

XMLPUBFUN xmlListPtr XMLCALL xmlListDup (const xmlListPtr old);
XMLPUBFUN int   XMLCALL xmlListSize     (xmlListPtr l);
XMLPUBFUN void  XMLCALL xmlListPopFront (xmlListPtr l);
XMLPUBFUN int   XMLCALL xmlListPushFront(xmlListPtr l, void *data);
XMLPUBFUN void  XMLCALL xmlListWalk     (xmlListPtr l, xmlListWalker walker, const void *user);
XMLPUBFUN void  XMLCALL xmlListMerge    (xmlListPtr l1, xmlListPtr l2);
XMLPUBFUN int   XMLCALL xmlListCopy     (xmlListPtr cur, const xmlListPtr old);
/* Link operators */
XMLPUBFUN void* XMLCALL xmlLinkGetData  (xmlLinkPtr lk);

/* xmlListUnique() */
/* xmlListSwap */

#ifdef __cplusplus
}
#endif

#endif /* __XML_LIST_H */

