/*
 * Summary: chained hash tables
 * description: this module implement the hash table support used in
 * various place in the library.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Bjorn Reese <bjorn.reese@systematic.dk>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/** @file
@publishedAll
@released
*/

#ifndef XML_HASH_H
#define XML_HASH_H

#include <stdapis/libxml2/libxml2_xmlstring.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The hash table.
 */
typedef struct _xmlHashTable xmlHashTable;
typedef xmlHashTable* xmlHashTablePtr;

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

/*
 * function types:
 */
/**
 * xmlHashDeallocator:
 * @param payload the data in the hash
 * @param name the name associated
 *
 * Callback to free data from a hash.
 */
typedef void (*xmlHashDeallocator)(void *payload, xmlChar *name);
/**
 * xmlHashCopier:
 * @param payload the data in the hash
 * @param name the name associated
 *
 * Callback to copy data from a hash.
 *
 * Returns a copy of the data or NULL in case of error.
 */
typedef void *(*xmlHashCopier)(void *payload, xmlChar *name);
/**
 * xmlHashScanner:
 * @param payload the data in the hash
 * @param data extra scannner data
 * @param name the name associated
 *
 * Callback when scanning data in a hash with the simple scanner.
 */
typedef void (*xmlHashScanner)(void *payload, void *data, xmlChar *name);
/**
 * xmlHashScannerFull:
 * @param payload the data in the hash
 * @param data extra scannner data
 * @param name the name associated
 * @param name2 the second name associated
 * @param name3 the third name associated
 *
 * Callback when scanning data in a hash with the full scanner.
 */
typedef void (*xmlHashScannerFull)(void *payload, void *data,
                                   const xmlChar *name, const xmlChar *name2,
                                   const xmlChar *name3);

/*
 * Constructor and destructor.
 */
XMLPUBFUN xmlHashTablePtr XMLCALL
                        xmlHashCreate   (int size);
XMLPUBFUN void XMLCALL
                        xmlHashFree     (xmlHashTablePtr table,
                                         xmlHashDeallocator f);


/*
 * Add a new entry to the hash table.
 */
XMLPUBFUN int XMLCALL
                        xmlHashAddEntry (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         void *userdata);
XMLPUBFUN int XMLCALL
                        xmlHashUpdateEntry(xmlHashTablePtr table,
                                         const xmlChar *name,
                                         void *userdata,
                                         xmlHashDeallocator f);
XMLPUBFUN int XMLCALL
                        xmlHashAddEntry2(xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         void *userdata);
XMLPUBFUN int XMLCALL
                        xmlHashUpdateEntry2(xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         void *userdata,
                                         xmlHashDeallocator f);
XMLPUBFUN int XMLCALL
                        xmlHashAddEntry3(xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         const xmlChar *name3,
                                         void *userdata);
XMLPUBFUN int XMLCALL
                        xmlHashUpdateEntry3(xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         const xmlChar *name3,
                                         void *userdata,
                                         xmlHashDeallocator f);

/*
 * Remove an entry from the hash table.
 */
XMLPUBFUN int XMLCALL
                        xmlHashRemoveEntry(xmlHashTablePtr table, const xmlChar *name,
                           xmlHashDeallocator f);
XMLPUBFUN int XMLCALL
                        xmlHashRemoveEntry2(xmlHashTablePtr table, const xmlChar *name,
                            const xmlChar *name2, xmlHashDeallocator f);
XMLPUBFUN int  XMLCALL
                        xmlHashRemoveEntry3(xmlHashTablePtr table, const xmlChar *name,
                            const xmlChar *name2, const xmlChar *name3,
                            xmlHashDeallocator f);

/*
 * Retrieve the userdata.
 */
XMLPUBFUN void * XMLCALL
                        xmlHashLookup   (xmlHashTablePtr table,
                                         const xmlChar *name);
XMLPUBFUN void * XMLCALL
                        xmlHashLookup2  (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2);
XMLPUBFUN void * XMLCALL
                        xmlHashLookup3  (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         const xmlChar *name3);

#ifndef XMLENGINE_EXCLUDE_UNUSED
XMLPUBFUN void * XMLCALL
                        xmlHashQLookup  (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *prefix);
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

XMLPUBFUN void * XMLCALL
                        xmlHashQLookup2 (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *prefix,
                                         const xmlChar *name2,
                                         const xmlChar *prefix2);
XMLPUBFUN void * XMLCALL
                        xmlHashQLookup3 (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *prefix,
                                         const xmlChar *name2,
                                         const xmlChar *prefix2,
                                         const xmlChar *name3,
                                         const xmlChar *prefix3);

/*
 * Helpers.
 */
XMLPUBFUN xmlHashTablePtr XMLCALL
                        xmlHashCopy     (xmlHashTablePtr table,
                                         xmlHashCopier f);

#ifndef XMLENGINE_EXCLUDE_UNUSED
XMLPUBFUN int XMLCALL
                        xmlHashSize     (xmlHashTablePtr table);
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

XMLPUBFUN void XMLCALL
                        xmlHashScan     (xmlHashTablePtr table,
                                         xmlHashScanner f,
                                         void *data);
XMLPUBFUN void XMLCALL
                        xmlHashScan3    (xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         const xmlChar *name3,
                                         xmlHashScanner f,
                                         void *data);
XMLPUBFUN void XMLCALL
                        xmlHashScanFull (xmlHashTablePtr table,
                                         xmlHashScannerFull f,
                                         void *data);
XMLPUBFUN void XMLCALL
                        xmlHashScanFull3(xmlHashTablePtr table,
                                         const xmlChar *name,
                                         const xmlChar *name2,
                                         const xmlChar *name3,
                                         xmlHashScannerFull f,
                                         void *data);
#ifdef __cplusplus
}
#endif
#endif /* XML_HASH_H */

