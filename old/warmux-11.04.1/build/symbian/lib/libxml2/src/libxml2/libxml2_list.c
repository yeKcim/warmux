/*
 * libxml2_list.c: lists handling implementation
 *
 * Copyright (C) 2000 Gary Pennington and Daniel Veillard.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
 * Author: Gary.Pennington@uk.sun.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#include <stdlib.h>
#include <string.h>
#include <libxml2_globals.h>

/*
 * Type definition are kept internal
 */

struct _xmlLink
{
    struct _xmlLink *next;
    struct _xmlLink *prev;
    void *data;
};

struct _xmlList
{
    xmlLinkPtr sentinel;
    void (*linkDeallocator)(xmlLinkPtr );
    int (*linkCompare)(const void *, const void*);
};

/************************************************************************
 *                                                                      *
 *                Interfaces                                            *
 *                                                                      *
 ************************************************************************/

/**
 * xmlLinkDeallocator:
 * @param l a list
 * @param lk a link
 *
 * Unlink and deallocate lk from list l
 */
static void
xmlLinkDeallocator(xmlListPtr l, xmlLinkPtr lk)
{
    (lk->prev)->next = lk->next;
    (lk->next)->prev = lk->prev;
    if(l->linkDeallocator)
        l->linkDeallocator(lk);
    xmlFree(lk);
}

/**
 * xmlLinkCompare:
 * @param data0 first data
 * @param data1 second data
 *
 * Compares two arbitrary data
 *
 * Returns -1, 0 or 1 depending on whether data1 is greater equal or smaller
 *          than data0
 */
static int
xmlLinkCompare(const void *data0, const void *data1)
{
    if (data0 < data1)
        return (-1);
    else if (data0 == data1)
        return (0);
    return (1);
}

/**
 * xmlListLowerSearch:
 * @param l a list
 * @param data a data
 *
 * Search data in the ordered list walking from the beginning
 *
 * Returns the link containing the data or NULL
 */
static xmlLinkPtr
xmlListLowerSearch(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;

    for(lk = list->sentinel->next;
        lk != list->sentinel && list->linkCompare(lk->data, data) < 0 ;
        lk = lk->next)
    {
        //empty
    }
    return lk;
}

/**
 * xmlListHigherSearch:
 * @param l a list
 * @param data a data
 *
 * Search data in the ordered list walking backward from the end
 *
 * Returns the link containing the data or NULL
 */
static xmlLinkPtr
xmlListHigherSearch(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;

    for(lk = list->sentinel->prev;
        lk != list->sentinel && list->linkCompare(lk->data, data) >0 ;
        lk = lk->prev)
    {
        //empty
    }
    return lk;
}

/**
 * xmlListSearch:
 * @param l a list
 * @param data a data
 *
 * Search data in the list
 *
 * Returns the link containing the data or NULL
 */
static xmlLinkPtr
xmlListLinkSearch(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;
    lk = xmlListLowerSearch(list, data);
    if (lk == list->sentinel)
        return NULL;
    else {
        if (list->linkCompare(lk->data, data) ==0)
            return lk;
        return NULL;
    }
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlListLinkReverseSearch:
 * @param l a list
 * @param data a data
 *
 * Search data in the list processing backward
 *
 * Returns the link containing the data or NULL
 */
static xmlLinkPtr
xmlListLinkReverseSearch(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;
    lk = xmlListHigherSearch(list, data);
    if (lk == list->sentinel)
        return NULL;
    else {
        if (list->linkCompare(lk->data, data) ==0)
            return lk;
        return NULL;
    }
}
#endif /* XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlListCreate:
 * @param deallocator an optional deallocator function
 * @param compare an optional comparison function
 *
 * Create a new list
 *
 * Returns the new list or NULL in case of error
 */
XMLPUBFUNEXPORT xmlListPtr
xmlListCreate(xmlListDeallocator deallocator, xmlListDataCompare compare)
{
    xmlListPtr list;
    if (NULL == (list = (xmlListPtr )xmlMalloc( sizeof(xmlList)))) {
        xmlGenericError(xmlGenericErrorContext,
                EMBED_ERRTXT("Cannot initialize memory for list"));
        return (NULL);
    }
    /* Initialize the list to NULL */
    memset(list, 0, sizeof(xmlList));

    /* Add the sentinel */
    if (NULL ==(list->sentinel = (xmlLinkPtr )xmlMalloc(sizeof(xmlLink)))) {
        xmlGenericError(xmlGenericErrorContext,
                EMBED_ERRTXT("Cannot initialize memory for sentinel"));
    xmlFree(list);
        return (NULL);
    }
    list->sentinel->next = list->sentinel;
    list->sentinel->prev = list->sentinel;
    list->sentinel->data = NULL;

    /* If there is a link deallocator, use it */
    if (deallocator != NULL)
        list->linkDeallocator = deallocator;
    /* If there is a link comparator, use it */
    if (compare != NULL)
        list->linkCompare = compare;
    else /* Use our own */
        list->linkCompare = xmlLinkCompare;
    return list;
}

/**
 * xmlListInsert:
 * @param l a list
 * @param data the data
 *
 * Insert data in the ordered list at the beginning for this value
 *
 * Returns 0 in case of success, 1 in case of failure
 */
XMLPUBFUNEXPORT int
xmlListInsert(xmlListPtr list, void *data)
{
    xmlLinkPtr lkPlace, lkNew;

    lkPlace = xmlListLowerSearch(list, data);
    /* Add the new link */
    lkNew = (xmlLinkPtr) xmlMalloc(sizeof(xmlLink));
    if (lkNew == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("Cannot initialize memory for new link"));
        return (1);
    }
    lkNew->data = data;
    lkPlace = lkPlace->prev;
    lkNew->next = lkPlace->next;
    (lkPlace->next)->prev = lkNew;
    lkPlace->next = lkNew;
    lkNew->prev = lkPlace;
    return 0;
}

/**
 * xmlListAppend:
 * @param l a list
 * @param data the data
 *
 * Insert data in the ordered list at the end for this value
 *
 * Returns 0 in case of success, 1 in case of failure
 */
XMLPUBFUNEXPORT int xmlListAppend(xmlListPtr list, void *data)
{
    xmlLinkPtr lkPlace, lkNew;

    lkPlace = xmlListHigherSearch(list, data);
    /* Add the new link */
    lkNew = (xmlLinkPtr) xmlMalloc(sizeof(xmlLink));
    if (lkNew == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("Cannot initialize memory for new link"));
        return (0);
    }
    lkNew->data = data;
    lkNew->next = lkPlace->next;
    (lkPlace->next)->prev = lkNew;
    lkPlace->next = lkNew;
    lkNew->prev = lkPlace;
    return 1;
}

/**
 * xmlListDelete:
 * @param l a list
 *
 * Deletes the list and its associated data
 */
XMLPUBFUNEXPORT void xmlListDelete(xmlListPtr list)
{
    xmlListClear(list);
    xmlFree(list->sentinel);
    xmlFree(list);
}

/**
 * xmlListRemoveFirst:
 * @param l a list
 * @param data list data
 *
 * Remove the first instance associated to data in the list
 *
 * Returns 1 if a deallocation occured, or 0 if not found
 */
XMLPUBFUNEXPORT int
xmlListRemoveFirst(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;

    /*Find the first instance of this data */
    lk = xmlListLinkSearch(list, data);
    if (lk != NULL) {
        xmlLinkDeallocator(list, lk);
        return 1;
    }
    return 0;
}

/**
 * xmlListClear:
 * @param l a list
 *
 * Remove the all data in the list
 */
XMLPUBFUNEXPORT void
xmlListClear(xmlListPtr list)
{
    xmlLinkPtr  lk = list->sentinel->next;

    while(lk != list->sentinel) {
        xmlLinkPtr next = lk->next;

        xmlLinkDeallocator(list, lk);
        lk = next;
    }
}

/**
 * xmlListEmpty:
 * @param l a list
 *
 * Is the list empty ?
 *
 * Returns 1 if the list is empty, 0 otherwise
 */
XMLPUBFUNEXPORT int
xmlListEmpty(xmlListPtr list)
{
    return (list->sentinel->next == list->sentinel);
}

/**
 * xmlListFront:
 * @param l a list
 *
 * Get the first element in the list
 *
 * Returns the first element in the list, or NULL
 */
XMLPUBFUNEXPORT xmlLinkPtr
xmlListFront(xmlListPtr list)
{
    return (list->sentinel->next);
}


/**
 * xmlListSearch:
 * @param l a list
 * @param data a search value
 *
 * Search the list for an existing value of data
 *
 * Returns the value associated to data or NULL in case of error
 */
XMLPUBFUNEXPORT void*
xmlListSearch(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;
    lk = xmlListLinkSearch(list, data);
    if (lk)
        return (lk->data);
    return NULL;
}


#ifndef XMLENGINE_EXCLUDE_UNUSED

/**
 * xmlListReverseSearch:
 * @param l a list
 * @param data a search value
 *
 * Search the list in reverse order for an existing value of data
 *
 * Returns the value associated to data or NULL in case of error
 */
void *
xmlListReverseSearch(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;
    lk = xmlListLinkReverseSearch(list, data);
    if (lk)
        return (lk->data);
    return NULL;
}

/**
 * xmlListRemoveLast:
 * @param l a list
 * @param data list data
 *
 * Remove the last instance associated to data in the list
 *
 * Returns 1 if a deallocation occured, or 0 if not found
 */
int
xmlListRemoveLast(xmlListPtr list, void *data)
{
    xmlLinkPtr lk;

    /*Find the last instance of this data */
    lk = xmlListLinkReverseSearch(list, data);
    if (lk != NULL) {
    xmlLinkDeallocator(list, lk);
        return 1;
    }
    return 0;
}

/**
 * xmlListRemoveAll:
 * @param l a list
 * @param data list data
 *
 * Remove the all instance associated to data in the list
 *
 * Returns the number of deallocation, or 0 if not found
 */
int
xmlListRemoveAll(xmlListPtr list, void *data)
{
    int count=0;

    while(xmlListRemoveFirst(list, data))
        count++;
    return count;
}

/**
 * xmlListEnd:
 * @param l a list
 *
 * Get the last element in the list
 *
 * Returns the last element in the list, or NULL
 */
xmlLinkPtr
xmlListEnd(xmlListPtr list)
{
    return (list->sentinel->prev);
}

/**
 * xmlListPopBack:
 * @param l a list
 *
 * Removes the last element in the list
 */
void
xmlListPopBack(xmlListPtr list)
{
    if(!xmlListEmpty(list))
        xmlLinkDeallocator(list, list->sentinel->prev);
}


/**
 * xmlListPushBack:
 * @param l a list
 * @param data new data
 *
 * add the new data at the end of the list
 *
 * Returns 1 if successful, 0 otherwise
 */
int
xmlListPushBack(xmlListPtr list, void *data)
{
    xmlLinkPtr lkPlace, lkNew;

    lkPlace = list->sentinel->prev;
    /* Add the new link */
    if (NULL ==(lkNew = (xmlLinkPtr )xmlMalloc(sizeof(xmlLink)))) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("Cannot initialize memory for new link"));
        return (0);
    }
    lkNew->data = data;
    lkNew->next = lkPlace->next;
    (lkPlace->next)->prev = lkNew;
    lkPlace->next = lkNew;
    lkNew->prev = lkPlace;
    return 1;
}


/**
 * xmlListReverse:
 * @param l a list
 *
 * Reverse the order of the elements in the list
 */
void
xmlListReverse(xmlListPtr list) {
  xmlLinkPtr lk;
  xmlLinkPtr lkPrev = list->sentinel;

  for(lk = list->sentinel->next; lk != list->sentinel; lk = lk->next) {
    lkPrev->next = lkPrev->prev;
    lkPrev->prev = lk;
    lkPrev = lk;
  }
  /* Fix up the last node */
  lkPrev->next = lkPrev->prev;
  lkPrev->prev = lk;
}

/**
 * xmlListSort:
 * @param l a list
 *
 * Sort all the elements in the list
 */
void
xmlListSort(xmlListPtr list)
{
    xmlListPtr lTemp;

    if(xmlListEmpty(list))
        return;

    /* I think that the real answer is to implement quicksort, the
     * alternative is to implement some list copying procedure which
     * would be based on a list copy followed by a clear followed by
     * an insert. This is slow...
     */

    if (NULL ==(lTemp = xmlListDup(list)))
        return;
    xmlListClear(list);
    xmlListMerge(list, lTemp);
    xmlListDelete(lTemp);
    return;
}


/**
 * xmlListReverseWalk:
 * @param l a list
 * @param walker a processing function
 * @param user a user parameter passed to the walker function
 *
 * Walk all the element of the list in reverse order and
 * apply the walker function to it
 */
void
xmlListReverseWalk(xmlListPtr list, xmlListWalker walker, const void *user) {
    xmlLinkPtr lk;

    for(lk = list->sentinel->prev; lk != list->sentinel; lk = lk->prev) {
        if((walker(lk->data, user)) == 0)
                break;
    }
}

#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlListSize:
 * @param l a list
 *
 * Get the number of elements in the list
 *
 * Returns the number of elements in the list
 */
XMLPUBFUNEXPORT int
xmlListSize(xmlListPtr list)
{
    xmlLinkPtr lk;
    int count=0;

    
    for(lk = list->sentinel->next; lk != list->sentinel; lk = lk->next, count++)
    {
     // empty
    }
    return count;
}

/**
 * xmlListPopFront:
 * @param l a list
 *
 * Removes the first element in the list
 */
XMLPUBFUNEXPORT void
xmlListPopFront(xmlListPtr list)
{
    if(!xmlListEmpty(list))
        xmlLinkDeallocator(list, list->sentinel->next);
}

/**
 * xmlListPushFront:
 * @param l a list
 * @param data new data
 *
 * add the new data at the beginning of the list
 *
 * Returns 1 if successful, 0 otherwise
 */
XMLPUBFUNEXPORT int
xmlListPushFront(xmlListPtr list, void *data)
{
    xmlLinkPtr lkPlace, lkNew;

    lkPlace = list->sentinel;
    /* Add the new link */
    lkNew = (xmlLinkPtr) xmlMalloc(sizeof(xmlLink));
    if (lkNew == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                       EMBED_ERRTXT("Cannot initialize memory for new link"));
        return (0);
    }
    lkNew->data = data;
    lkNew->next = lkPlace->next;
    (lkPlace->next)->prev = lkNew;
    lkPlace->next = lkNew;
    lkNew->prev = lkPlace;
    return 1;
}


/**
 * xmlLinkGetData:
 * @param lk a link
 *
 * See Returns.
 *
 * Returns a pointer to the data referenced from this link
 */
XMLPUBFUNEXPORT void *
xmlLinkGetData(xmlLinkPtr lk)
{
    return lk->data;
}

/**
 * xmlListWalk:
 * @param l a list
 * @param walker a processing function
 * @param user a user parameter passed to the walker function
 *
 * Walk all the element of the first from first to last and
 * apply the walker function to it
 */
XMLPUBFUNEXPORT void
xmlListWalk(xmlListPtr list, xmlListWalker walker, const void *user) {
    xmlLinkPtr lk;

    for(lk = list->sentinel->next; lk != list->sentinel; lk = lk->next) {
        if((walker(lk->data, user)) == 0)
                break;
    }
}

/**
 * xmlListMerge:
 * @param l1 the original list
 * @param l2 the new list
 *
 * include all the elements of the second list in the first one and
 * clear the second list
 */
XMLPUBFUNEXPORT void
xmlListMerge(xmlListPtr list1, xmlListPtr list2)
{
    xmlListCopy(list1, list2);
    xmlListClear(list2);
}

/**
 * xmlListDup:
 * @param old the list
 *
 * Duplicate the list
 *
 * Returns a new copy of the list or NULL in case of error
 */
XMLPUBFUNEXPORT xmlListPtr
xmlListDup(const xmlListPtr oldList)
{
    xmlListPtr cur;
    /* Hmmm, how to best deal with allocation issues when copying
     * lists. If there is a de-allocator, should responsibility lie with
     * the new list or the old list. Surely not both. I'll arbitrarily
     * set it to be the old list for the time being whilst I work out
     * the answer
     */
    if (NULL ==(cur = xmlListCreate(NULL, oldList->linkCompare)))
        return (NULL);
    if (0 != xmlListCopy(cur, oldList))
        return NULL;
    return cur;
}

/**
 * xmlListCopy:
 * @param cur the new list
 * @param old the old list
 *
 * Move all the element from the old list in the new list
 *
 * Returns 0 in case of success 1 in case of error
 */
XMLPUBFUNEXPORT int
xmlListCopy(xmlListPtr cur, const xmlListPtr old)
{
    /* Walk the old tree and insert the data into the new one */
    xmlLinkPtr lk;

    for(lk = old->sentinel->next; lk != old->sentinel; lk = lk->next) {
        if (0 !=xmlListInsert(cur, lk->data)) {
            xmlListDelete(cur);
            return (1);
        }
    }
    return (0);
}
/* xmlListUnique() */
/* xmlListSwap */
