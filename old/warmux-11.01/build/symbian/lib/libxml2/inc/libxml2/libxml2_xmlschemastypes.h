/*
 * Summary: implementation of XML Schema Datatypes
 * Description: module providing the XML Schema Datatypes implementation
 *              both definition and validity checking
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

#ifndef XML_SCHEMA_TYPES_H
#define XML_SCHEMA_TYPES_H

#include <stdapis/libxml2/libxml2_xmlversion.h>

#if defined(LIBXML_SCHEMAS_ENABLED) || defined(XMLENGINE_XMLSCHEMA_DATATYPES)

#include <stdapis/libxml2/libxml2_schemasinternals.h>
#include "libxml2_xmlschemas.h"

#ifdef __cplusplus
extern "C" {
#endif

XMLPUBFUN void XMLCALL
            xmlSchemaInitTypes     (void);
XMLPUBFUN void XMLCALL
        xmlSchemaCleanupTypes      (void);
XMLPUBFUN xmlSchemaTypePtr XMLCALL
        xmlSchemaGetPredefinedType (const xmlChar* name,
                                    const xmlChar *ns);
XMLPUBFUN int XMLCALL
        xmlSchemaValidatePredefinedType (xmlSchemaTypePtr type,
                                    const xmlChar *value,
                                    xmlSchemaValPtr *val);
XMLPUBFUN int XMLCALL
        xmlSchemaValPredefTypeNode (xmlSchemaTypePtr type,
                                    const xmlChar *value,
                                    xmlSchemaValPtr *val,
                         xmlNodePtr node);
XMLPUBFUN int XMLCALL
        xmlSchemaValidateFacet     (xmlSchemaTypePtr base,
                                    xmlSchemaFacetPtr facet,
                                    const xmlChar *value,
                                    xmlSchemaValPtr val);
XMLPUBFUN void XMLCALL
        xmlSchemaFreeValue         (xmlSchemaValPtr val);


#if defined(LIBXML_SCHEMAS_ENABLED)
XMLPUBFUN xmlSchemaFacetPtr XMLCALL
        xmlSchemaNewFacet       (void);

XMLPUBFUN int XMLCALL
        xmlSchemaCheckFacet     (xmlSchemaFacetPtr facet,
                         xmlSchemaTypePtr typeDecl,
                         xmlSchemaParserCtxtPtr ctxt,
                         const xmlChar *name);
#endif

XMLPUBFUN void XMLCALL
        xmlSchemaFreeFacet          (xmlSchemaFacetPtr facet);
XMLPUBFUN int XMLCALL
        xmlSchemaCompareValues      (xmlSchemaValPtr x,
                                     xmlSchemaValPtr y);

#ifdef __cplusplus
}
#endif

#endif /* defined(LIBXML_SCHEMAS_ENABLED) || defined(XMLENGINE_XMLSCHEMA_DATATYPES) */
#endif /* XML_SCHEMA_TYPES_H */
