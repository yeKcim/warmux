/*
 * libxml2_xmlschemasglobals.h : implementation of the XML Schema Datatypes
 *             definition and validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */


/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLSCHEMAS_GLOBALS_H
#define XMLSCHEMAS_GLOBALS_H

typedef struct _xmlSchemaTypesGlobalData xmlSchemaTypesGlobalData;
typedef xmlSchemaTypesGlobalData* xmlSchemaTypesGlobalDataPtr;

#include <libxml2_schemasinternals.h>

// xmlSchemaTypesInitialized is defined as separate global variable

struct _xmlSchemaTypesGlobalData{
    xmlHashTablePtr xmlSchemaTypesBank;

    /*
     * Basic types
     */
    xmlSchemaTypePtr xmlSchemaTypeStringDef;
    xmlSchemaTypePtr xmlSchemaTypeAnyTypeDef;
    xmlSchemaTypePtr xmlSchemaTypeAnySimpleTypeDef;
    xmlSchemaTypePtr xmlSchemaTypeDecimalDef;
    xmlSchemaTypePtr xmlSchemaTypeDatetimeDef;
    xmlSchemaTypePtr xmlSchemaTypeDateDef;
    xmlSchemaTypePtr xmlSchemaTypeTimeDef;
    xmlSchemaTypePtr xmlSchemaTypeGYearDef;
    xmlSchemaTypePtr xmlSchemaTypeGYearMonthDef;
    xmlSchemaTypePtr xmlSchemaTypeGDayDef;
    xmlSchemaTypePtr xmlSchemaTypeGMonthDayDef;
    xmlSchemaTypePtr xmlSchemaTypeGMonthDef;
    xmlSchemaTypePtr xmlSchemaTypeDurationDef;
    xmlSchemaTypePtr xmlSchemaTypeFloatDef;
    xmlSchemaTypePtr xmlSchemaTypeBooleanDef;
    xmlSchemaTypePtr xmlSchemaTypeDoubleDef;
    xmlSchemaTypePtr xmlSchemaTypeHexBinaryDef;
    xmlSchemaTypePtr xmlSchemaTypeBase64BinaryDef;
    xmlSchemaTypePtr xmlSchemaTypeAnyURIDef;

    /*
     * Derived types
     */
    xmlSchemaTypePtr xmlSchemaTypePositiveIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeNonPositiveIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeNegativeIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeNonNegativeIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeLongDef;
    xmlSchemaTypePtr xmlSchemaTypeIntDef;
    xmlSchemaTypePtr xmlSchemaTypeShortDef;
    xmlSchemaTypePtr xmlSchemaTypeByteDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedLongDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedIntDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedShortDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedByteDef;
    xmlSchemaTypePtr xmlSchemaTypeNormStringDef;
    xmlSchemaTypePtr xmlSchemaTypeTokenDef;
    xmlSchemaTypePtr xmlSchemaTypeLanguageDef;
    xmlSchemaTypePtr xmlSchemaTypeNameDef;
    xmlSchemaTypePtr xmlSchemaTypeQNameDef;
    xmlSchemaTypePtr xmlSchemaTypeNCNameDef;
    xmlSchemaTypePtr xmlSchemaTypeIdDef;
    xmlSchemaTypePtr xmlSchemaTypeIdrefDef;
    xmlSchemaTypePtr xmlSchemaTypeIdrefsDef;
    xmlSchemaTypePtr xmlSchemaTypeEntityDef;
    xmlSchemaTypePtr xmlSchemaTypeEntitiesDef;
    xmlSchemaTypePtr xmlSchemaTypeNotationDef;
    xmlSchemaTypePtr xmlSchemaTypeNmtokenDef;
    xmlSchemaTypePtr xmlSchemaTypeNmtokensDef;
}; // struct _xmlSchemaGlobalData

#endif /* XMLSCHEMAS_GLOBALS_H */

