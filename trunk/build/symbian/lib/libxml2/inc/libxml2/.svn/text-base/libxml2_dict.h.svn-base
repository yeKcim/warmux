/*
 * Summary: string dictionnary
 * Description: dictionary of reusable strings, just used to avoid allocation
 *         and freeing operations.
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

#ifndef XML_DICT_H
#define XML_DICT_H

#include "libxml2_xmlstring.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xmlDictStrings xmlDictStrings;
typedef xmlDictStrings* xmlDictStringsPtr;

struct _xmlDictStrings {
    xmlDictStringsPtr next;
    xmlChar* free;
    xmlChar* end;
    int size;
    int nbStrings;
    xmlChar array[1];
};

/*
 * An entry in the dictionnary
 */
typedef struct _xmlDictEntry xmlDictEntry;
typedef xmlDictEntry* xmlDictEntryPtr;
struct _xmlDictEntry {
    xmlDictEntryPtr next;
    const xmlChar* name;
    int len;
    int valid;
};
/*
 * The dictionnary.
 */
typedef struct _xmlDict xmlDict;
typedef xmlDict* xmlDictPtr;

/*
 * The entire dictionnary
 */
struct _xmlDict {
    int ref_counter;

    xmlDictEntryPtr dict;
    int size;
    int nbElems;
    xmlDictStringsPtr strings;

    xmlDictPtr      subdict;
};

/*
 * Constructor and destructor.
 */
XMLPUBFUN xmlDictPtr XMLCALL
                        xmlDictCreate   (void);

XMLPUBFUN xmlDictPtr XMLCALL xmlDictCreateSub(xmlDictPtr sub);

XMLPUBFUN int XMLCALL
                        xmlDictReference(xmlDictPtr dict);
XMLPUBFUN void XMLCALL
                        xmlDictFree     (xmlDictPtr dict);

/*
 * Lookup of entry in the dictionnary.
 */
XMLPUBFUN const xmlChar* XMLCALL
                        xmlDictLookup   (xmlDictPtr dict,
                                         const xmlChar *name,
                                         int len);

XMLPUBFUN const xmlChar * XMLCALL
                        xmlDictQLookup  (xmlDictPtr dict,
                                         const xmlChar *prefix,
                                         const xmlChar *name);

#ifndef XMLENGINE_EXCLUDE_UNUSED
XMLPUBFUN int XMLCALL
                        xmlDictSize     (xmlDictPtr dict);
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

XMLPUBFUN int XMLCALL
                        xmlDictOwns     (xmlDictPtr dict,
                                         const xmlChar *str);
#ifdef __cplusplus
}
#endif
#endif /* XML_DICT_H */

