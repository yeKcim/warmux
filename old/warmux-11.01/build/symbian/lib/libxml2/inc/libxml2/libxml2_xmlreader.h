/*
 * Summary: the XMLReader implementation
 * Description: API of the XML streaming API based on C# interfaces.
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

#ifndef XML_XMLREADER_H
#define XML_XMLREADER_H

#include <libxml2_tree.h>
#include <libxml2_xmlio.h>

#ifdef LIBXML_SCHEMAS_ENABLED
#include "libxml2_relaxng.h"
#include "libxml2_xmlschemas.h"
#endif

// XMLENGINE: This set of header was moved here from .c file

#ifdef LIBXML_SCHEMAS_ENABLED
#include "libxml2_relaxng.h"
#endif

#ifdef LIBXML_XINCLUDE_ENABLED
#include "libxml2_xinclude.h"
#endif

#ifdef LIBXML_PATTERN_ENABLED
#include "libxml2_pattern.h"
#endif
//----------------------------------------------------------

#ifdef LIBXML_READER_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlTextReaderMode:
 *
 * Internal state values for the reader.
 */
typedef enum {
    XML_TEXTREADER_MODE_INITIAL = 0,
    XML_TEXTREADER_MODE_INTERACTIVE = 1,
    XML_TEXTREADER_MODE_ERROR = 2,
    XML_TEXTREADER_MODE_EOF =3,
    XML_TEXTREADER_MODE_CLOSED = 4,
    XML_TEXTREADER_MODE_READING = 5
} xmlTextReaderMode;

/**
 * xmlParserProperties:
 *
 * Some common options to use with xmlTextReaderSetParserProp, but it
 * is better to use xmlParserOption and the xmlReaderNewxxx and
 * xmlReaderForxxx APIs now.
 */
typedef enum {
    XML_PARSER_LOADDTD = 1,
    XML_PARSER_DEFAULTATTRS = 2,
    XML_PARSER_VALIDATE = 3,
    XML_PARSER_SUBST_ENTITIES = 4
} xmlParserProperties;

/**
 * xmlParserSeverities:
 *
 * How severe an error callback is when the per-reader error callback API
 * is used.
 */
typedef enum {
    XML_PARSER_SEVERITY_VALIDITY_WARNING = 1,
    XML_PARSER_SEVERITY_VALIDITY_ERROR = 2,
    XML_PARSER_SEVERITY_WARNING = 3,
    XML_PARSER_SEVERITY_ERROR = 4
} xmlParserSeverities;

/**
 * xmlReaderTypes:
 *
 * Predefined constants for the different types of nodes.
 */
typedef enum {
    XML_READER_TYPE_NONE = 0,
    XML_READER_TYPE_ELEMENT = 1,
    XML_READER_TYPE_ATTRIBUTE = 2,
    XML_READER_TYPE_TEXT = 3,
    XML_READER_TYPE_CDATA = 4,
    XML_READER_TYPE_ENTITY_REFERENCE = 5,
    XML_READER_TYPE_ENTITY = 6,
    XML_READER_TYPE_PROCESSING_INSTRUCTION = 7,
    XML_READER_TYPE_COMMENT = 8,
    XML_READER_TYPE_DOCUMENT = 9,
    XML_READER_TYPE_DOCUMENT_TYPE = 10,
    XML_READER_TYPE_DOCUMENT_FRAGMENT = 11,
    XML_READER_TYPE_NOTATION = 12,
    XML_READER_TYPE_WHITESPACE = 13,
    XML_READER_TYPE_SIGNIFICANT_WHITESPACE = 14,
    XML_READER_TYPE_END_ELEMENT = 15,
    XML_READER_TYPE_END_ENTITY = 16,
    XML_READER_TYPE_XML_DECLARATION = 17
} xmlReaderTypes;

/*
 * Error handling extensions
 */
typedef void *  xmlTextReaderLocatorPtr;
typedef void   (XMLCALL *xmlTextReaderErrorFunc)        (void *arg,
                         const char *msg,
                         xmlParserSeverities severity,
                         xmlTextReaderLocatorPtr locator);
/**
 * xmlTextReader:
 *
 * Structure for an xmlReader context.
 */
typedef struct _xmlTextReader xmlTextReader;

/**
 * xmlTextReaderPtr:
 *
 * Pointer to an xmlReader context.
 */
typedef xmlTextReader *xmlTextReaderPtr;
// XMLENGINE: This moved to header from xmlreader.c file
typedef enum {
    XML_TEXTREADER_NONE = -1,
    XML_TEXTREADER_START= 0,
    XML_TEXTREADER_ELEMENT= 1,
    XML_TEXTREADER_END= 2,
    XML_TEXTREADER_EMPTY= 3,
    XML_TEXTREADER_BACKTRACK= 4,
    XML_TEXTREADER_DONE= 5,
    XML_TEXTREADER_ERROR= 6
} xmlTextReaderState;

typedef enum {
    XML_TEXTREADER_NOT_VALIDATE = 0,
    XML_TEXTREADER_VALIDATE_DTD = 1,
    XML_TEXTREADER_VALIDATE_RNG = 2,
    XML_TEXTREADER_VALIDATE_XSD = 4
} xmlTextReaderValidate;

struct _xmlTextReader {
    int                     mode;           /* the parsing mode */
    xmlDocPtr               doc;            /* when walking an existing doc */
    xmlTextReaderValidate   validate;       /* is there any validation */
    int                     allocs;         /* what structure were deallocated */
    xmlTextReaderState      state;
    xmlParserCtxtPtr        ctxt;           /* the parser context */
    xmlSAXHandlerPtr        sax;            /* the parser SAX callbacks */
    xmlParserInputBufferPtr input;          /* the input */
    startElementSAXFunc     startElement;   /* initial SAX callbacks */
    endElementSAXFunc       endElement;     /* idem */
    startElementNsSAX2Func  startElementNs; /* idem */
    endElementNsSAX2Func    endElementNs;   /* idem */
    charactersSAXFunc       characters;
    cdataBlockSAXFunc       cdataBlock;
    unsigned int            base;    /* base of the segment in the input */
    unsigned int            cur;     /* current position in the input */
    xmlNodePtr              node;    /* current node */
    xmlNodePtr              curnode; /* current attribute node */
    int                     depth;   /* depth of the current node */
    xmlNodePtr              faketext;/* fake xmlNs chld */
    int                     preserve;/* preserve the resulting document */
    xmlBufferPtr            buffer;  /* used to return const xmlChar * */
    xmlDictPtr              dict;    /* the context dictionnary */

    /* entity stack when traversing entities content */
    xmlNodePtr              ent;          /* Current Entity Ref Node */
    int                     entNr;        /* Depth of the entities stack */
    int                     entMax;       /* Max depth of the entities stack */
    xmlNodePtr*             entTab;       /* array of entities */

    /* error handling */
    xmlTextReaderErrorFunc  errorFunc;    /* callback function */
    void*                   errorFuncArg; /* callback function user argument */

#ifdef LIBXML_SCHEMAS_ENABLED
    /* Handling of RelaxNG validation */
    xmlRelaxNGPtr           rngSchemas;    /* The Relax NG schemas */
    xmlRelaxNGValidCtxtPtr  rngValidCtxt;  /* The Relax NG validation context */
    int                     rngValidErrors;/* The number of errors detected */
    xmlNodePtr              rngFullNode;   /* the node if RNG not progressive */
    /* Handling of Schemas validation */
    xmlSchemaPtr          xsdSchemas;   /* The Schemas schemas */
    xmlSchemaValidCtxtPtr xsdValidCtxt;/* The Schemas validation context */
    int                  xsdValidErrors;/* The number of errors detected */
    xmlSchemaSAXPlugPtr   xsdPlug;  /* the schemas plug in SAX pipeline */
#endif
#ifdef LIBXML_XINCLUDE_ENABLED
    /* Handling of XInclude processing */
    int                     xinclude;      /* is xinclude asked for */
    const xmlChar*          xinclude_name; /* the xinclude name from dict */
    xmlXIncludeCtxtPtr      xincctxt;      /* the xinclude context */
    int                     in_xinclude;   /* counts for xinclude */
#endif
#ifdef LIBXML_PATTERN_ENABLED
    int                     patternNr;     /* number of preserve patterns */
    int                     patternMax;    /* max preserve patterns */
    xmlPatternPtr*          patternTab;    /* array of preserve patterns */
#endif
    int                     preserves;     /* level of preserves */
    int                     parserFlags;   /* the set of options set */
    /* Structured error handling */
    xmlStructuredErrorFunc  sErrorFunc;    /* callback function */
};

/*
 * Constructors & Destructor
 */
XMLPUBFUN xmlTextReaderPtr XMLCALL
                        xmlNewTextReader        (xmlParserInputBufferPtr input,
                                                 const char *URI);
XMLPUBFUN xmlTextReaderPtr XMLCALL
                        xmlNewTextReaderFilename(const char *URI);
XMLPUBFUN void XMLCALL
                        xmlFreeTextReader       (xmlTextReaderPtr reader);

/*
 * Iterators
 */
XMLPUBFUN int XMLCALL
                        xmlTextReaderRead       (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderReadInnerXml       (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderReadOuterXml       (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderReadString         (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderReadAttributeValue (xmlTextReaderPtr reader);

/*
 * Attributes of the node
 */
XMLPUBFUN int XMLCALL
                        xmlTextReaderAttributeCount(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderDepth      (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderHasAttributes(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderHasValue(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderIsDefault  (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderIsEmptyElement(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderNodeType   (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderQuoteChar  (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                        xmlTextReaderReadState  (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
            xmlTextReaderIsNamespaceDecl(xmlTextReaderPtr reader);

XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstBaseUri   (xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstLocalName (xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstName      (xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstNamespaceUri(xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstPrefix    (xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstXmlLang   (xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstString    (xmlTextReaderPtr reader,
                                                 const xmlChar *str);
XMLPUBFUN const xmlChar * XMLCALL
                    xmlTextReaderConstValue     (xmlTextReaderPtr reader);

/*
 * use the Const version of the routine for
 * better performance and simpler code
 */
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderBaseUri    (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderLocalName  (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderName       (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderNamespaceUri(xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderPrefix     (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderXmlLang    (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                        xmlTextReaderValue      (xmlTextReaderPtr reader);

/*
 * Methods of the XmlTextReader
 */
XMLPUBFUN int XMLCALL
                    xmlTextReaderClose          (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                    xmlTextReaderGetAttributeNo (xmlTextReaderPtr reader,
                                                 int no);
XMLPUBFUN xmlChar * XMLCALL
                    xmlTextReaderGetAttribute   (xmlTextReaderPtr reader,
                                                 const xmlChar *name);
XMLPUBFUN xmlChar * XMLCALL
                    xmlTextReaderGetAttributeNs (xmlTextReaderPtr reader,
                                                 const xmlChar *localName,
                                                 const xmlChar *namespaceURI);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
                    xmlTextReaderGetRemainder   (xmlTextReaderPtr reader);
XMLPUBFUN xmlChar * XMLCALL
                    xmlTextReaderLookupNamespace(xmlTextReaderPtr reader,
                                                 const xmlChar *prefix);
XMLPUBFUN int XMLCALL
                    xmlTextReaderMoveToAttributeNo(xmlTextReaderPtr reader,
                                                 int no);
XMLPUBFUN int XMLCALL
                    xmlTextReaderMoveToAttribute(xmlTextReaderPtr reader,
                                                 const xmlChar *name);
XMLPUBFUN int XMLCALL
                    xmlTextReaderMoveToAttributeNs(xmlTextReaderPtr reader,
                                                 const xmlChar *localName,
                                                 const xmlChar *namespaceURI);
XMLPUBFUN int XMLCALL
                    xmlTextReaderMoveToFirstAttribute(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                    xmlTextReaderMoveToNextAttribute(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                    xmlTextReaderMoveToElement  (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                    xmlTextReaderNormalization  (xmlTextReaderPtr reader);
XMLPUBFUN const xmlChar * XMLCALL
            xmlTextReaderConstEncoding  (xmlTextReaderPtr reader);

/*
 * Extensions
 */
XMLPUBFUN int XMLCALL
                    xmlTextReaderSetParserProp  (xmlTextReaderPtr reader,
                                                 int prop,
                                                 int value);
XMLPUBFUN int XMLCALL
                    xmlTextReaderGetParserProp  (xmlTextReaderPtr reader,
                                                 int prop);
XMLPUBFUN xmlNodePtr XMLCALL
                    xmlTextReaderCurrentNode    (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
            xmlTextReaderGetParserLineNumber(xmlTextReaderPtr reader);

XMLPUBFUN int XMLCALL
            xmlTextReaderGetParserColumnNumber(xmlTextReaderPtr reader);

XMLPUBFUN xmlNodePtr XMLCALL
            xmlTextReaderPreserve   (xmlTextReaderPtr reader);
#ifdef LIBXML_PATTERN_ENABLED
XMLPUBFUN int XMLCALL
                    xmlTextReaderPreservePattern(xmlTextReaderPtr reader,
                                                 const xmlChar *pattern,
                                                 const xmlChar **namespaces);
#endif /* LIBXML_PATTERN_ENABLED */
XMLPUBFUN xmlDocPtr XMLCALL
                    xmlTextReaderCurrentDoc     (xmlTextReaderPtr reader);
XMLPUBFUN xmlNodePtr XMLCALL
                    xmlTextReaderExpand         (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                    xmlTextReaderNext           (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                    xmlTextReaderNextSibling    (xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
                    xmlTextReaderIsValid        (xmlTextReaderPtr reader);
#ifdef LIBXML_SCHEMAS_ENABLED
XMLPUBFUN int XMLCALL
                    xmlTextReaderRelaxNGValidate(xmlTextReaderPtr reader,
                                                 const char *rng);
XMLPUBFUN int XMLCALL
                    xmlTextReaderRelaxNGSetSchema(xmlTextReaderPtr reader,
                                                 xmlRelaxNGPtr schema);
XMLPUBFUN int XMLCALL
            xmlTextReaderSchemaValidate (xmlTextReaderPtr reader,
                             const char *xsd);
XMLPUBFUN int XMLCALL
            xmlTextReaderSetSchema  (xmlTextReaderPtr reader,
                             xmlSchemaPtr schema);
#endif
XMLPUBFUN const xmlChar * XMLCALL
            xmlTextReaderConstXmlVersion(xmlTextReaderPtr reader);
XMLPUBFUN int XMLCALL
            xmlTextReaderStandalone     (xmlTextReaderPtr reader);


/*
 * Index lookup
 */
XMLPUBFUN long XMLCALL
        xmlTextReaderByteConsumed   (xmlTextReaderPtr reader);

/*
 * New more complete APIs for simpler creation and reuse of readers
 */
XMLPUBFUN xmlTextReaderPtr XMLCALL
                xmlReaderWalker         (xmlDocPtr doc);
XMLPUBFUN xmlTextReaderPtr XMLCALL
                xmlReaderForDoc         (const xmlChar * cur,
                                         const char *URL,
                                         const char *encoding,
                                         int options);
XMLPUBFUN xmlTextReaderPtr XMLCALL
                xmlReaderForFile        (const char *filename,
                                         const char *encoding,
                                         int options);
XMLPUBFUN xmlTextReaderPtr XMLCALL
                xmlReaderForMemory      (const char *buffer,
                                         int size,
                                         const char *URL,
                                         const char *encoding,
                                         int options);
XMLPUBFUN xmlTextReaderPtr XMLCALL
                xmlReaderForFd          (int fd,
                                         const char *URL,
                                         const char *encoding,
                                         int options);
XMLPUBFUN xmlTextReaderPtr XMLCALL
                xmlReaderForIO          (xmlInputReadCallback ioread,
                                         xmlInputCloseCallback ioclose,
                                         void *ioctx,
                                         const char *URL,
                                         const char *encoding,
                                         int options);

XMLPUBFUN int XMLCALL
                xmlReaderNewWalker      (xmlTextReaderPtr reader,
                                         xmlDocPtr doc);
XMLPUBFUN int XMLCALL
                xmlReaderNewDoc         (xmlTextReaderPtr reader,
                                         const xmlChar * cur,
                                         const char *URL,
                                         const char *encoding,
                                         int options);
XMLPUBFUN int XMLCALL
                xmlReaderNewFile        (xmlTextReaderPtr reader,
                                         const char *filename,
                                         const char *encoding,
                                         int options);
XMLPUBFUN int XMLCALL
                xmlReaderNewMemory      (xmlTextReaderPtr reader,
                                         const char *buffer,
                                         int size,
                                         const char *URL,
                                         const char *encoding,
                                         int options);
XMLPUBFUN int XMLCALL
                xmlReaderNewFd          (xmlTextReaderPtr reader,
                                         int fd,
                                         const char *URL,
                                         const char *encoding,
                                         int options);
XMLPUBFUN int XMLCALL
                xmlReaderNewIO          (xmlTextReaderPtr reader,
                                         xmlInputReadCallback ioread,
                                         xmlInputCloseCallback ioclose,
                                         void *ioctx,
                                         const char *URL,
                                         const char *encoding,
                                         int options);

// typedefs moved in the beginning of the header

#ifdef LIBXML_ENABLE_NODE_LINEINFO
XMLPUBFUN int XMLCALL
            xmlTextReaderLocatorLineNumber(xmlTextReaderLocatorPtr locator);
#endif /* LIBXML_ENABLE_NODE_LINEINFO */

XMLPUBFUN xmlChar * XMLCALL
            xmlTextReaderLocatorBaseURI (xmlTextReaderLocatorPtr locator);
XMLPUBFUN void XMLCALL
            xmlTextReaderSetErrorHandler(xmlTextReaderPtr reader,
                         xmlTextReaderErrorFunc f,
                         void *arg);
XMLPUBFUN void XMLCALL
            xmlTextReaderSetStructuredErrorHandler(xmlTextReaderPtr reader,
                               xmlStructuredErrorFunc f,
                               void *arg);
XMLPUBFUN void XMLCALL
            xmlTextReaderGetErrorHandler(xmlTextReaderPtr reader,
                         xmlTextReaderErrorFunc *f,
                         void **arg);

#ifdef __cplusplus
}
#endif

#endif /* LIBXML_READER_ENABLED */

#endif /* XML_XMLREADER_H */

