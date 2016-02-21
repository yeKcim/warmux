/*
 * Summary: internals routines exported by the parser.
 * Description: this module exports a number of internal parsing routines
 *              they are not really all intended for applications but
 *              can prove useful doing low level processing.
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

#ifndef XML_PARSER_INTERNALS_H
#define XML_PARSER_INTERNALS_H

#include <stdapis/libxml2/libxml2_parser.h>
#include <stdapis/libxml2/libxml2_chvalid.h>


#ifdef __cplusplus
extern "C" {
#endif

#define SAX_COMPAT_MODE BAD_CAST "SAX compatibility mode document"



 /**
  * XML_MAX_NAMELEN:
  *
  * Identifiers can be longer, but this will be more costly
  * at runtime.
  */
#define XML_MAX_NAMELEN 100

/**
 * INPUT_CHUNK:
 *
 * The parser tries to always have that amount of input ready.
 * One of the point is providing context when reporting errors.
 */
#define INPUT_CHUNK 250

/**
 * MIN_STACK_THRESHOLD:
 *
 * The safty buffer that defines number of bytes from stack overflow.
 */
#define MIN_STACK_THRESHOLD 600

/**
 * MAX_STACK_THRESHOLD:
 *
 * The safty buffer that defines number of bytes from stack overflow.
 * This value is used for SAX parsing - buffer is bigger to account 
 * for stack that might be allocated during user callbacks
 */
#define MAX_STACK_THRESHOLD 1000

/************************************************************************
 *                                                                      *
 * UNICODE version of the macros.                                       *
 *                                                                      *
 ************************************************************************/
/**
 * IS_BYTE_CHAR:
 * @param c an byte value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [2] Char ::= #x9 | #xA | #xD | [#x20...]
 * any byte character in the accepted range
 */
#define IS_BYTE_CHAR(c)  xmlIsChar_ch(c)

/**
 * IS_CHAR:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [2] Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD]
 *                  | [#x10000-#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 */
#define IS_CHAR(c)   xmlIsCharQ(c)

/**
 * IS_CHAR_CH:
 * @param c an xmlChar (usually an unsigned char)
 *
 * Behaves like IS_CHAR on single-byte value
 */
#define IS_CHAR_CH(c)  xmlIsChar_ch(c)

/**
 * IS_BLANK:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [3] S ::= (#x20 | #x9 | #xD | #xA)+
 */
#define IS_BLANK(c)  xmlIsBlankQ(c)

/**
 * IS_BLANK_CH:
 * @param c an xmlChar value (normally unsigned char)
 *
 * Behaviour same as IS_BLANK
 *
 * OOM: never
 */
#define IS_BLANK_CH(c)  xmlIsBlank_ch(c)

/**
 * IS_BASECHAR:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [85] BaseChar ::= ... long list see REC ...
 */
#define IS_BASECHAR(c) xmlIsBaseCharQ(c)

/**
 * IS_DIGIT:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [88] Digit ::= ... long list see REC ...
 */
#define IS_DIGIT(c) xmlIsDigitQ(c)

/**
 * IS_DIGIT_CH:
 * @param c an xmlChar value (usually an unsigned char)
 *
 * Behaves like IS_DIGIT but with a single byte argument
 */
#define IS_DIGIT_CH(c)  xmlIsDigit_ch(c)

/**
 * IS_COMBINING:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [87] CombiningChar ::= ... long list see REC ...
 */
#define IS_COMBINING(c) xmlIsCombiningQ(c)

/**
 * IS_COMBINING_CH:
 * @param c an xmlChar (usually an unsigned char)
 *
 * Always false (all combining chars > 0xff)
 */
#define IS_COMBINING_CH(c) 0

/**
 * IS_EXTENDER:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [89] Extender ::= #x00B7 | #x02D0 | #x02D1 | #x0387 | #x0640 |
 *                   #x0E46 | #x0EC6 | #x3005 | [#x3031-#x3035] |
 *                   [#x309D-#x309E] | [#x30FC-#x30FE]
 */
#define IS_EXTENDER(c) xmlIsExtenderQ(c)

/**
 * IS_EXTENDER_CH:
 * @param c an xmlChar value (usually an unsigned char)
 *
 * Behaves like IS_EXTENDER but with a single-byte argument
 */
#define IS_EXTENDER_CH(c)  xmlIsExtender_ch(c)

/**
 * IS_IDEOGRAPHIC:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [86] Ideographic ::= [#x4E00-#x9FA5] | #x3007 | [#x3021-#x3029]
 */
#define IS_IDEOGRAPHIC(c) xmlIsIdeographicQ(c)

/**
 * IS_LETTER:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [84] Letter ::= BaseChar | Ideographic
 */
#define IS_LETTER(c) (IS_BASECHAR(c) || IS_IDEOGRAPHIC(c))

/**
 * IS_LETTER_CH:
 * @param c an xmlChar value (normally unsigned char)
 *
 * Macro behaves like IS_LETTER, but only check base chars
 *
 */
#define IS_LETTER_CH(c) xmlIsBaseChar_ch(c)
/**
 * IS_PUBIDCHAR:
 * @param c an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [13] PubidChar ::= #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
 */
#define IS_PUBIDCHAR(c) xmlIsPubidCharQ(c)

/**
 * IS_PUBIDCHAR_CH:
 * @param c an xmlChar value (normally unsigned char)
 *
 * Same as IS_PUBIDCHAR but for single-byte value
 */
#define IS_PUBIDCHAR_CH(c) xmlIsPubidChar_ch(c)

/**
 * SKIP_EOL:
 * @param p and UTF8 string pointer
 *
 * Skips the end of line chars.
 */
#define SKIP_EOL(p)                             \
    if (*(p) == 0x13) { p++ ; if (*(p) == 0x10) p++; }          \
    if (*(p) == 0x10) { p++ ; if (*(p) == 0x13) p++; }

/**
 * MOVETO_ENDTAG:
 * @param p and UTF8 string pointer
 *
 * Skips to the next '>' char.
 */
#define MOVETO_ENDTAG(p)                        \
    while ((*p) && (*(p) != '>')) (p)++

/**
 * MOVETO_STARTTAG:
 * @param p and UTF8 string pointer
 *
 * Skips to the next '<' char.
 */
#define MOVETO_STARTTAG(p)                      \
    while ((*p) && (*(p) != '<')) (p)++

/**
 * Global constants used for predefined strings.
 */
#ifndef UNDEF_IMPORT_C_IN_DATA_ParserInternal
XMLPUBVAR const xmlChar xmlStringText[];
XMLPUBVAR const xmlChar xmlStringTextNoenc[];
XMLPUBVAR const xmlChar xmlStringComment[];
#endif
/*
 * Function to finish the work of the macros where needed.
 */
XMLPUBFUN int XMLCALL                   xmlIsLetter     (int c);

/**
 * Parser context.
 */
XMLPUBFUN xmlParserCtxtPtr XMLCALL
            xmlCreateFileParserCtxt (const char *filename);
XMLPUBFUN xmlParserCtxtPtr XMLCALL
            xmlCreateURLParserCtxt  (const char *filename,
                         int options);
XMLPUBFUN xmlParserCtxtPtr XMLCALL
            xmlCreateMemoryParserCtxt(const char *buffer,
                         int size);
XMLPUBFUN xmlParserCtxtPtr XMLCALL
            xmlCreateEntityParserCtxt(const xmlChar *URL,
                         const xmlChar *ID,
                         const xmlChar *base);
XMLPUBFUN int XMLCALL
            xmlSwitchEncoding   (xmlParserCtxtPtr ctxt,
                         xmlCharEncoding enc);
XMLPUBFUN int XMLCALL
            xmlSwitchToEncoding (xmlParserCtxtPtr ctxt,
                     xmlCharEncodingHandlerPtr handler);
XMLPUBFUN int XMLCALL
            xmlSwitchInputEncoding  (xmlParserCtxtPtr ctxt,
                         xmlParserInputPtr input,
                     xmlCharEncodingHandlerPtr handler);

/**
 * Entities
 */
XMLPUBFUN void XMLCALL
            xmlHandleEntity     (xmlParserCtxtPtr ctxt,
                         xmlEntityPtr entity);

/**
 * Input Streams.
 */
XMLPUBFUN xmlParserInputPtr XMLCALL
            xmlNewStringInputStream (xmlParserCtxtPtr ctxt,
                         const xmlChar *buffer);
XMLPUBFUN xmlParserInputPtr XMLCALL
            xmlNewEntityInputStream (xmlParserCtxtPtr ctxt,
                         xmlEntityPtr entity);
XMLPUBFUN void XMLCALL
            xmlPushInput        (xmlParserCtxtPtr ctxt,
                         xmlParserInputPtr input);
XMLPUBFUN xmlChar XMLCALL
            xmlPopInput     (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlFreeInputStream  (xmlParserInputPtr input);
XMLPUBFUN xmlParserInputPtr XMLCALL
            xmlNewInputFromFile (xmlParserCtxtPtr ctxt,
                         const char *filename);
XMLPUBFUN xmlParserInputPtr XMLCALL
            xmlNewInputStream   (xmlParserCtxtPtr ctxt);

/**
 * Namespaces.
 */
XMLPUBFUN xmlChar * XMLCALL
            xmlSplitQName       (xmlParserCtxtPtr ctxt,
                         const xmlChar *name,
                         xmlChar **prefix);
XMLPUBFUN xmlChar * XMLCALL
            xmlNamespaceParseNCName (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlNamespaceParseQName  (xmlParserCtxtPtr ctxt,
                         xmlChar **prefix);
XMLPUBFUN xmlChar * XMLCALL
            xmlNamespaceParseNSDef  (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseQuotedString    (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseNamespace   (xmlParserCtxtPtr ctxt);

/**
 * Generic production rules.
 */
XMLPUBFUN xmlChar * XMLCALL
            xmlScanName     (xmlParserCtxtPtr ctxt);
XMLPUBFUN const xmlChar * XMLCALL
            xmlParseName        (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseNmtoken     (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseEntityValue (xmlParserCtxtPtr ctxt,
                         xmlChar **orig);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseAttValue    (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseSystemLiteral   (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParsePubidLiteral    (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseCharData    (xmlParserCtxtPtr ctxt,
                         int cdata);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseExternalID  (xmlParserCtxtPtr ctxt,
                         xmlChar **publicID,
                         int strict);
XMLPUBFUN void XMLCALL
            xmlParseComment     (xmlParserCtxtPtr ctxt);
XMLPUBFUN const xmlChar * XMLCALL
            xmlParsePITarget    (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParsePI      (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseNotationDecl    (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseEntityDecl  (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
            xmlParseDefaultDecl (xmlParserCtxtPtr ctxt,
                         xmlChar **value);
XMLPUBFUN xmlEnumerationPtr XMLCALL
            xmlParseNotationType    (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlEnumerationPtr XMLCALL
            xmlParseEnumerationType (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
            xmlParseEnumeratedType  (xmlParserCtxtPtr ctxt,
                         xmlEnumerationPtr *tree);
XMLPUBFUN int XMLCALL
            xmlParseAttributeType   (xmlParserCtxtPtr ctxt,
                         xmlEnumerationPtr *tree);
XMLPUBFUN void XMLCALL
            xmlParseAttributeListDecl(xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlElementContentPtr XMLCALL
            xmlParseElementMixedContentDecl
                        (xmlParserCtxtPtr ctxt,
                         int inputchk);
XMLPUBFUN xmlElementContentPtr XMLCALL
            xmlParseElementChildrenContentDecl
                        (xmlParserCtxtPtr ctxt,
                         int inputchk);
XMLPUBFUN int XMLCALL
            xmlParseElementContentDecl(xmlParserCtxtPtr ctxt,
                         const xmlChar *name,
                         xmlElementContentPtr *result);
XMLPUBFUN int XMLCALL
            xmlParseElementDecl (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseMarkupDecl  (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
            xmlParseCharRef     (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlEntityPtr XMLCALL
            xmlParseEntityRef   (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseReference   (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParsePEReference (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseDocTypeDecl (xmlParserCtxtPtr ctxt);
XMLPUBFUN const xmlChar * XMLCALL
            xmlParseAttribute   (xmlParserCtxtPtr ctxt,
                         xmlChar **value);
XMLPUBFUN const xmlChar * XMLCALL
            xmlParseStartTag    (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseEndTag      (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseCDSect      (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseContent     (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseElement     (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseVersionNum  (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseVersionInfo (xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlParseEncName     (xmlParserCtxtPtr ctxt);
XMLPUBFUN const xmlChar * XMLCALL
            xmlParseEncodingDecl    (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
            xmlParseSDDecl      (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseXMLDecl     (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseTextDecl    (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseMisc        (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL
            xmlParseExternalSubset  (xmlParserCtxtPtr ctxt,
                         const xmlChar *ExternalID,
                         const xmlChar *SystemID);
/**
 * XML_SUBSTITUTE_NONE:
 *
 * If no entities need to be substituted.
 */
#define XML_SUBSTITUTE_NONE 0
/**
 * XML_SUBSTITUTE_REF:
 *
 * Whether general entities need to be substituted.
 */
#define XML_SUBSTITUTE_REF  1
/**
 * XML_SUBSTITUTE_PEREF:
 *
 * Whether parameter entities need to be substituted.
 */
#define XML_SUBSTITUTE_PEREF    2
/**
 * XML_SUBSTITUTE_BOTH:
 *
 * Both general and parameter entities need to be substituted.
 */
#define XML_SUBSTITUTE_BOTH     3

XMLPUBFUN xmlChar * XMLCALL
        xmlDecodeEntities       (xmlParserCtxtPtr ctxt,
                         int len,
                         int what,
                         xmlChar end,
                         xmlChar  end2,
                         xmlChar end3);
XMLPUBFUN xmlChar * XMLCALL
        xmlStringDecodeEntities     (xmlParserCtxtPtr ctxt,
                         const xmlChar *str,
                         int what,
                         xmlChar end,
                         xmlChar  end2,
                         xmlChar end3);
XMLPUBFUN xmlChar * XMLCALL
        xmlStringLenDecodeEntities  (xmlParserCtxtPtr ctxt,
                         const xmlChar *str,
                         int len,
                         int what,
                         xmlChar end,
                         xmlChar  end2,
                         xmlChar end3);

/*
 * Generated by MACROS on top of parser.c c.f. PUSH_AND_POP.
 */
XMLPUBFUN int XMLCALL           nodePush        (xmlParserCtxtPtr ctxt,
                         xmlNodePtr value);
XMLPUBFUN xmlNodePtr XMLCALL        nodePop         (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL           inputPush       (xmlParserCtxtPtr ctxt,
                         xmlParserInputPtr value);
XMLPUBFUN xmlParserInputPtr XMLCALL inputPop        (xmlParserCtxtPtr ctxt);
XMLPUBFUN const xmlChar * XMLCALL   namePop         (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL           namePush        (xmlParserCtxtPtr ctxt,
                         const xmlChar *value);

/*
 * other commodities shared between parser.c and parserInternals.
 */
XMLPUBFUN int XMLCALL           xmlSkipBlankChars   (xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL           xmlStringCurrentChar    (xmlParserCtxtPtr ctxt,
                         const xmlChar *cur,
                         int *len);
XMLPUBFUN void XMLCALL          xmlParserHandlePEReference(xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL          xmlParserHandleReference(xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL           xmlCheckLanguageID  (const xmlChar *lang);

/*
 * Really core function shared with HTML parser.
 */
XMLPUBFUN int XMLCALL           xmlCurrentChar      (xmlParserCtxtPtr ctxt,
                         int *len);
XMLPUBFUN int XMLCALL       xmlCopyCharMultiByte    (xmlChar *out,
                         int val);
XMLPUBFUN int XMLCALL           xmlCopyChar     (int len,
                         xmlChar *out,
                         int val);
XMLPUBFUN void XMLCALL          xmlNextChar     (xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL          xmlParserInputShrink    (xmlParserInputPtr in);

#ifdef LIBXML_HTML_ENABLED
/*
 * Actually comes from the HTML parser but launched from the init stuff.
 */
XMLPUBFUN void XMLCALL            htmlInitAutoClose   (void);
//XMLPUBFUN htmlParserCtxtPtr XMLCALL   htmlCreateFileParserCtxt(const char *filename,
//                                           const char *encoding);

#endif

/*
 * Specific function to keep track of entities references
 * and used by the XSLT debugger.
 */
/**
 * xmlEntityReferenceFunc:
 * @param ent the entity
 * @param firstNode the fist node in the chunk
 * @param lastNode the last nod in the chunk
 *
 * Callback function used when one needs to be able to track back the
 * provenance of a chunk of nodes inherited from an entity replacement.
 */
typedef void    (*xmlEntityReferenceFunc)   (xmlEntityPtr ent,
                         xmlNodePtr firstNode,
                         xmlNodePtr lastNode);

XMLPUBFUN void XMLCALL      xmlSetEntityReferenceFunc   (xmlEntityReferenceFunc func);


/*  
 * Macros for identifying data nodes. Data nodes are kept as text nodes
 * but some of the fields are reused to save memory. 
 * ns 				<-> CID
 * content			<-> binary content, or RChunk, or RFile reference
 * properties		<-> size of binary data
 * nsDef			<-> offset of binary data in RChunk
 * psvi				<-> container type
 *
 * Added in S60 3.2 release.
 */
#define IS_DATA_NODE(node) \
	node->type == XML_TEXT_NODE && node->ns
	
#define IS_BINARY_NODE(node) \
	node->type == XML_TEXT_NODE && (int)node->psvi == 30

#define IS_RCHUNK_NODE(node) \
	node->type == XML_TEXT_NODE && (int)node->psvi == 31

#define IS_RFILE_NODE(node) \
	node->type == XML_TEXT_NODE && (int)node->psvi == 32

#define IS_EXTERNAL_NODE(node) \
	node->type == XML_TEXT_NODE && ((int)node->psvi == 31 || (int)node->psvi == 32)

/** given text node, check if it represents data node **/	
#define TEXT_IS_DATA(node) \
	node->ns

/** given text node, check if it represents binary node **/	
#define TEXT_IS_BINARY(node) \
	(int)node->psvi == 30

/** given text node, check if it represents external node **/
#define TEXT_IS_EXTERNAL(node) \
	((int)node->psvi == 31 || (int)node->psvi == 32)

/** fetches data node cid **/	
#define DATA_NODE_CID(node) \
	(xmlChar *)node->ns

#ifdef __cplusplus
}
#endif
#endif /* XML_PARSER_INTERNALS_H */

