/*
 * Summary: Defines which libxml2 modules are compiled in for XML Engine
 * Description: Defines which libxml2 modules are compiled in for XML Engine
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/**
 @file
 @publishedAll
 @released
*/
#ifndef LIBXML2_MODULES_H
#define LIBXML2_MODULES_H

// All new configuration directives are in:
#include <stdapis/libxml2/xmlengconfig.h>

#ifdef XMLENGINE_EXCLUDE_EMBED_MSG
extern const char * const __embedded_errtxt_replacement;
#define  EMBED_ERRTXT(str) NULL
//#define  EMBED_ERRTXT(str) __embedded_errtxt_replacement
#else
#define  EMBED_ERRTXT(str) str
#endif

#ifdef IN_LIBXML
#   define  OOM_FLAG        (xmlOOM)
#else
// DO NOT USE "xmlOOM" outside of Libxml2 code!!!
// (xmlOOM is an internal macro for accessing value via TLS)
#   define  OOM_FLAG        (xmlOOMFlag())
#endif

#   define  SET_OOM_FLAG    xmlSetOOM()
#   define  RESET_OOM_FLAG  xmlResetOOM()

/*
*  _xmlDoc struct has field 'cachedGs'
*  where result of xmlGetGlobalState() is stored
*
*  (cachedGs field is not in the part that is common with _xmlNode and _xmlAttr)
*/
#define LIBXML_ENABLE_GS_CACHING_IN_DOC
#define LIBXML_ENABLE_GS_CACHING_IN_CTXT

/*
*   struct members
*       _xmlNode.line,  _xmlRef.lineno, _xmlID.lineno
*   and all related functions/code
*   are excluded from the current configuration if this is commented out:
*/
//#define LIBXML_ENABLE_NODE_LINEINFO

/**
 * LIBXML_CATALOG_ENABLED:
 *
 * Whether the Catalog support is configured in
 */
//#define LIBXML_CATALOG_ENABLED

/**
 * LIBXML_THREAD_ENABLED:
 *
 * Whether the thread support is configured in
 */
//#define LIBXML_THREAD_ENABLED

/**
 * LIBXML_TREE_ENABLED:
 *
 * Whether the DOM like tree manipulation API support is configured in
 */
#define LIBXML_TREE_ENABLED

/**
 * LIBXML_OUTPUT_ENABLED:
 *
 * Whether the serialization/saving support is configured in
 */
#define LIBXML_OUTPUT_ENABLED

/**
 * LIBXML_PUSH_ENABLED:
 *
 * Whether the push parsing interfaces are configured in
 */
#define LIBXML_PUSH_ENABLED

/**
 * LIBXML_READER_ENABLED:
 *
 * Whether the xmlReader parsing interface is configured in
 */
#define LIBXML_READER_ENABLED

/**
 * LIBXML_PATTERN_ENABLED:
 *
 * Whether the xmlPattern node selection interface is configured in
 */
//#define LIBXML_PATTERN_ENABLED

/**
 * LIBXML_WRITER_ENABLED:
 *
 * Whether the xmlWriter saving interface is configured in
 */
#define LIBXML_WRITER_ENABLED

/**
 * LIBXML_SAX1_ENABLED:
 *
 * Whether the older SAX1 interface is configured in
 */
#define LIBXML_SAX1_ENABLED

/**
 * LIBXML_VALID_ENABLED:
 *
 * Whether the DTD validation support is configured in
 */
//#define LIBXML_VALID_ENABLED

/**
 * LIBXML_HTML_ENABLED:
 *
 * Whether the HTML support is configured in
 */
//#define LIBXML_HTML_ENABLED

/**
 * LIBXML_LEGACY_ENABLED:
 *
 * Whether the deprecated APIs are compiled in for compatibility
 */
//#define LIBXML_LEGACY_ENABLED

/**
 * LIBXML_C14N_ENABLED:
 *
 * Whether the Canonicalization support is configured in
 */
#define LIBXML_C14N_ENABLED

/**
 * LIBXML_XPATH_ENABLED:
 *
 * Whether XPath is configured in
 */
#define LIBXML_XPATH_ENABLED

/**
 * LIBXML_XPTR_ENABLED:
 *
 * Whether XPointer is configured in
 */
#define LIBXML_XPTR_ENABLED

/**
 * LIBXML_XINCLUDE_ENABLED:
 *
 * Whether XInclude is configured in
 */
#define LIBXML_XINCLUDE_ENABLED

/**
 * LIBXML_DEBUG_ENABLED:
 *
 * Whether Debugging module is configured in
 */
//#define LIBXML_DEBUG_ENABLED

/**
 * DEBUG_MEMORY_LOCATION:
 *
 * Whether the memory debugging is configured in
 */
//#define DEBUG_MEMORY_LOCATION

/**
 * LIBXML_UNICODE_ENABLED:
 *
 * Whether the Unicode related interfaces are compiled in
 */
//#define LIBXML_UNICODE_ENABLED

/**
 * LIBXML_REGEXP_ENABLED:
 *
 * Whether the regular expressions interfaces are compiled in
 */
//#define LIBXML_REGEXP_ENABLED

/**
 * LIBXML_AUTOMATA_ENABLED:
 *
 * Whether the automata interfaces are compiled in
 */
//#define LIBXML_AUTOMATA_ENABLED

/**
 * LIBXML_SCHEMAS_ENABLED:
 *
 * Whether the Schemas validation interfaces are compiled in
 */
//#define LIBXML_SCHEMAS_ENABLED

#endif /* LIBXML2_MODULES_H */
