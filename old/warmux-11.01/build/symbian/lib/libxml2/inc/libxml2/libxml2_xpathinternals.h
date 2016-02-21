/*
 * Summary: internal interfaces for XML Path Language implementation
 * Description: internal interfaces for XML Path Language implementation
 *              used to build new modules on top of XPath like XPointer and
 *              XSLT
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

#ifndef XML_XPATH_INTERNALS_H
#define XML_XPATH_INTERNALS_H

#include <stdapis/libxml2/libxml2_xpath.h>

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 *                                                                      *
 *          Helpers                                                     *
 *                                                                      *
 ************************************************************************/

/*
 * Many of these macros may later turn into functions. They
 * shouldn't be used in #ifdef's preprocessor instructions.
 */
/**
 * xmlXPathSetError:
 * @param ctxt an XPath parser context
 * @param err an xmlXPathError code
 *
 * Raises an error.
 */
#define xmlXPathSetError(ctxt, err)                 \
    { xmlXPatherror((ctxt), __FILE__, __LINE__, (err));         \
      (ctxt)->error = (err); }

/**
 * xmlXPathSetArityError:
 * @param ctxt an XPath parser context
 *
 * Raises an XPATH_INVALID_ARITY error.
 */
#define xmlXPathSetArityError(ctxt)                 \
    xmlXPathSetError((ctxt), XPATH_INVALID_ARITY)

/**
 * xmlXPathSetTypeError:
 * @param ctxt an XPath parser context
 *
 * Raises an XPATH_INVALID_TYPE error.
 */
#define xmlXPathSetTypeError(ctxt)                  \
    xmlXPathSetError((ctxt), XPATH_INVALID_TYPE)

/**
 * xmlXPathGetError:
 * @param ctxt an XPath parser context
 *
 * Get the error code of an XPath context.
 *
 * Returns the context error.
 */
#define xmlXPathGetError(ctxt)    ((ctxt)->error)

/**
 * xmlXPathCheckError:
 * @param ctxt an XPath parser context
 *
 * Check if an XPath error was raised.
 *
 * Returns true if an error has been raised, false otherwise.
 */
#define xmlXPathCheckError(ctxt)  ((ctxt)->error != XPATH_EXPRESSION_OK)

/**
 * xmlXPathGetDocument:
 * @param ctxt an XPath parser context
 *
 * Get the document of an XPath context.
 *
 * Returns the context document.
 */
#define xmlXPathGetDocument(ctxt)   ((ctxt)->context->doc)

/**
 * xmlXPathGetContextNode:
 * @param ctxt an XPath parser context
 *
 * Get the context node of an XPath context.
 *
 * Returns the context node.
 */
#define xmlXPathGetContextNode(ctxt)    ((ctxt)->context->node)

XMLPUBFUN int XMLCALL
            xmlXPathPopBoolean  (xmlXPathParserContextPtr ctxt);
XMLPUBFUN double XMLCALL
            xmlXPathPopNumber   (xmlXPathParserContextPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
            xmlXPathPopString   (xmlXPathParserContextPtr ctxt);
XMLPUBFUN xmlNodeSetPtr XMLCALL
            xmlXPathPopNodeSet  (xmlXPathParserContextPtr ctxt);
XMLPUBFUN void * XMLCALL
            xmlXPathPopExternal (xmlXPathParserContextPtr ctxt);

/**
 * xmlXPathReturnBoolean:
 * @param ctxt an XPath parser context
 * @param val a boolean
 *
 * Pushes the boolean val on the context stack.
 */
#define xmlXPathReturnBoolean(ctxt, val)                \
    valuePush((ctxt), xmlXPathNewBoolean(val))

/**
 * xmlXPathReturnTrue:
 * @param ctxt an XPath parser context
 *
 * Pushes true on the context stack.
 */
#define xmlXPathReturnTrue(ctxt)   xmlXPathReturnBoolean((ctxt), 1)

/**
 * xmlXPathReturnFalse:
 * @param ctxt an XPath parser context
 *
 * Pushes false on the context stack.
 */
#define xmlXPathReturnFalse(ctxt)  xmlXPathReturnBoolean((ctxt), 0)

/**
 * xmlXPathReturnNumber:
 * @param ctxt an XPath parser context
 * @param val a double
 *
 * Pushes the double val on the context stack.
 */
#define xmlXPathReturnNumber(ctxt, val)                 \
    valuePush((ctxt), xmlXPathNewFloat(val))

/**
 * xmlXPathReturnString:
 * @param ctxt an XPath parser context
 * @param str a string
 *
 * Pushes the string str on the context stack.
 */
#define xmlXPathReturnString(ctxt, str)                 \
    valuePush((ctxt), xmlXPathWrapString(str))

/**
 * xmlXPathReturnEmptyString:
 * @param ctxt an XPath parser context
 *
 * Pushes an empty string on the stack.
 */
#define xmlXPathReturnEmptyString(ctxt)                 \
    valuePush((ctxt), xmlXPathNewCString(""))

/**
 * xmlXPathReturnNodeSet:
 * @param ctxt an XPath parser context
 * @param ns a node-set
 *
 * Pushes the node-set ns on the context stack.
 */
#define xmlXPathReturnNodeSet(ctxt, ns)                 \
    valuePush((ctxt), xmlXPathWrapNodeSet(ns))

/**
 * xmlXPathReturnEmptyNodeSet:
 * @param ctxt an XPath parser context
 *
 * Pushes an empty node-set on the context stack.
 */
#define xmlXPathReturnEmptyNodeSet(ctxt)                \
    valuePush((ctxt), xmlXPathNewNodeSet(NULL))

/**
 * xmlXPathReturnExternal:
 * @param ctxt an XPath parser context
 * @param val user data
 *
 * Pushes user data on the context stack.
 */
#define xmlXPathReturnExternal(ctxt, val)               \
    valuePush((ctxt), xmlXPathWrapExternal(val))

/**
 * xmlXPathStackIsNodeSet:
 * @param ctxt an XPath parser context
 *
 * Check if the current value on the XPath stack is a node set or
 * an XSLT value tree.
 *
 * Returns true if the current object on the stack is a node-set.
 */
#define xmlXPathStackIsNodeSet(ctxt)                    \
    (((ctxt)->value != NULL)                        \
     && (((ctxt)->value->type == XPATH_NODESET)             \
         || ((ctxt)->value->type == XPATH_XSLT_TREE)))

/**
 * xmlXPathStackIsExternal:
 * @param ctxt an XPath parser context
 *
 * Checks if the current value on the XPath stack is an external
 * object.
 *
 * Returns true if the current object on the stack is an external
 * object.
 */
#define xmlXPathStackIsExternal(ctxt)                   \
    ((ctxt->value != NULL) && (ctxt->value->type == XPATH_USERS))

/**
 * xmlXPathEmptyNodeSet:
 * @param ns a node-set
 *
 * Empties a node-set.
 */
#define xmlXPathEmptyNodeSet(ns)                    \
    { while ((ns)->nodeNr > 0) (ns)->nodeTab[(ns)->nodeNr--] = NULL; }

/**
 * CHECK_ERROR:
 *
 * Macro to return from the function if an XPath error was detected.
 */
#define CHECK_ERROR                         \
    if (ctxt->error != XPATH_EXPRESSION_OK) return

/**
 * CHECK_ERROR0:
 *
 * Macro to return 0 from the function if an XPath error was detected.
 */
#define CHECK_ERROR0                            \
    if (ctxt->error != XPATH_EXPRESSION_OK) return(0)

/**
 * XP_ERROR:
 * @param X the error code
 *
 * Macro to raise an XPath error and return.
 */
#define XP_ERROR(X)                         \
    { xmlXPathErr(ctxt, X); return; }

/**
 * XP_ERROR0:
 * @param X the error code
 *
 * Macro to raise an XPath error and return 0.
 */
#define XP_ERROR0(X)                            \
    { xmlXPathErr(ctxt, X); return(0); }

/**
 * CHECK_TYPE:
 * @param typeval the XPath type
 *
 * Macro to check that the value on top of the XPath stack is of a given
 * type.
 */
#define CHECK_TYPE(typeval)                     \
    if ((ctxt->value == NULL) || (ctxt->value->type != typeval))    \
        XP_ERROR(XPATH_INVALID_TYPE)

/**
 * CHECK_TYPE0:
 * @param typeval the XPath type
 *
 * Macro to check that the value on top of the XPath stack is of a given
 * type. Return(0) in case of failure
 */
#define CHECK_TYPE0(typeval)                        \
    if ((ctxt->value == NULL) || (ctxt->value->type != typeval))    \
        XP_ERROR0(XPATH_INVALID_TYPE)

/**
 * CHECK_ARITY:
 * @param x the number of expected args
 *
 * Macro to check that the number of args passed to an XPath function matches.
 */
#define CHECK_ARITY(x)                          \
    if (nargs != (x))                           \
        XP_ERROR(XPATH_INVALID_ARITY);

/**
 * CAST_TO_STRING:
 *
 * Macro to try to cast the value on the top of the XPath stack to a string.
 */
#define CAST_TO_STRING                          \
    if ((ctxt->value != NULL) && (ctxt->value->type != XPATH_STRING))   \
        xmlXPathStringFunction(ctxt, 1);

/**
 * CAST_TO_NUMBER:
 *
 * Macro to try to cast the value on the top of the XPath stack to a number.
 */
#define CAST_TO_NUMBER                          \
    if ((ctxt->value != NULL) && (ctxt->value->type != XPATH_NUMBER))   \
        xmlXPathNumberFunction(ctxt, 1);

/**
 * CAST_TO_BOOLEAN:
 *
 * Macro to try to cast the value on the top of the XPath stack to a boolean.
 */
#define CAST_TO_BOOLEAN                         \
    if ((ctxt->value != NULL) && (ctxt->value->type != XPATH_BOOLEAN))  \
        xmlXPathBooleanFunction(ctxt, 1);

/*
 * Variable Lookup forwarding.
 */
/**
 * xmlXPathVariableLookupFunc:
 * @param ctxt an XPath context
 * @param name name of the variable
 * @param ns_uri the namespace name hosting this variable
 *
 * Prototype for callbacks used to plug variable lookup in the XPath
 * engine.
 *
 * Returns the XPath object value or NULL if not found.
 */
typedef xmlXPathObjectPtr (*xmlXPathVariableLookupFunc) (void *ctxt,
                     const xmlChar *name,
                     const xmlChar *ns_uri);

XMLPUBFUN void XMLCALL
    xmlXPathRegisterVariableLookup  (xmlXPathContextPtr ctxt,
                     xmlXPathVariableLookupFunc f,
                     void *data);

/*
 * Function Lookup forwarding.
 */
/**
 * xmlXPathFuncLookupFunc:
 * @param ctxt an XPath context
 * @param name name of the function
 * @param ns_uri the namespace name hosting this function
 *
 * Prototype for callbacks used to plug function lookup in the XPath
 * engine.
 *
 * Returns the XPath function or NULL if not found.
 */
typedef xmlXPathFunction (*xmlXPathFuncLookupFunc) (void *ctxt,
                     const xmlChar *name,
                     const xmlChar *ns_uri);

XMLPUBFUN void XMLCALL
        xmlXPathRegisterFuncLookup  (xmlXPathContextPtr ctxt,
                     xmlXPathFuncLookupFunc f,
                     void *funcCtxt);

/*
 * Error reporting.
 */


// DONE: Replaced with macro
#define xmlXPatherror(ctxt,file,line,no)  xmlXPathErr(ctxt, no)
//XMLPUBFUN void XMLCALL
//      xmlXPatherror   (xmlXPathParserContextPtr ctxt,
//               const char *file,
//               int line,
//               int no);

XMLPUBFUN void XMLCALL
        xmlXPathErr (xmlXPathParserContextPtr ctxt, int error);

#ifdef LIBXML_DEBUG_ENABLED
XMLPUBFUN void XMLCALL
        xmlXPathDebugDumpObject (FILE *output,
                     xmlXPathObjectPtr cur,
                     int depth);
XMLPUBFUN void XMLCALL
        xmlXPathDebugDumpCompExpr(FILE *output,
                     xmlXPathCompExprPtr comp,
                     int depth);
#endif
/**
 * NodeSet handling.
 */
XMLPUBFUN int XMLCALL
        xmlXPathNodeSetContains     (xmlNodeSetPtr cur,
                         xmlNodePtr val);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathDifference      (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathIntersection        (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);

XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathDistinctSorted      (xmlNodeSetPtr nodes);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathDistinct        (xmlNodeSetPtr nodes);

XMLPUBFUN int XMLCALL
        xmlXPathHasSameNodes        (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);

XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathNodeLeadingSorted   (xmlNodeSetPtr nodes,
                         xmlNodePtr node);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathLeadingSorted       (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathNodeLeading     (xmlNodeSetPtr nodes,
                         xmlNodePtr node);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathLeading         (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);

XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathNodeTrailingSorted  (xmlNodeSetPtr nodes,
                         xmlNodePtr node);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathTrailingSorted      (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathNodeTrailing        (xmlNodeSetPtr nodes,
                         xmlNodePtr node);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathTrailing        (xmlNodeSetPtr nodes1,
                         xmlNodeSetPtr nodes2);


/**
 * Extending a context.
 */

XMLPUBFUN int XMLCALL
        xmlXPathRegisterNs      (xmlXPathContextPtr ctxt,
                         const xmlChar *prefix,
                         const xmlChar *ns_uri);
XMLPUBFUN const xmlChar * XMLCALL
        xmlXPathNsLookup        (xmlXPathContextPtr ctxt,
                         const xmlChar *prefix);
XMLPUBFUN void XMLCALL
        xmlXPathRegisteredNsCleanup (xmlXPathContextPtr ctxt);

XMLPUBFUN int XMLCALL
        xmlXPathRegisterFunc        (xmlXPathContextPtr ctxt,
                         const xmlChar *name,
                         xmlXPathFunction f);
XMLPUBFUN int XMLCALL
        xmlXPathRegisterFuncNS      (xmlXPathContextPtr ctxt,
                         const xmlChar *name,
                         const xmlChar *ns_uri,
                         xmlXPathFunction f);
XMLPUBFUN int XMLCALL
        xmlXPathRegisterVariable    (xmlXPathContextPtr ctxt,
                         const xmlChar *name,
                         xmlXPathObjectPtr value);
XMLPUBFUN int XMLCALL
        xmlXPathRegisterVariableNS  (xmlXPathContextPtr ctxt,
                         const xmlChar *name,
                         const xmlChar *ns_uri,
                         xmlXPathObjectPtr value);
XMLPUBFUN xmlXPathFunction XMLCALL
        xmlXPathFunctionLookup      (xmlXPathContextPtr ctxt,
                         const xmlChar *name);
XMLPUBFUN xmlXPathFunction XMLCALL
        xmlXPathFunctionLookupNS    (xmlXPathContextPtr ctxt,
                         const xmlChar *name,
                         const xmlChar *ns_uri);
XMLPUBFUN void XMLCALL
        xmlXPathRegisteredFuncsCleanup  (xmlXPathContextPtr ctxt);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathVariableLookup      (xmlXPathContextPtr ctxt,
                         const xmlChar *name);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathVariableLookupNS    (xmlXPathContextPtr ctxt,
                         const xmlChar *name,
                         const xmlChar *ns_uri);
XMLPUBFUN void XMLCALL
        xmlXPathRegisteredVariablesCleanup(xmlXPathContextPtr ctxt);

/**
 * Utilities to extend XPath.
 */
XMLPUBFUN xmlXPathParserContextPtr XMLCALL
        xmlXPathNewParserContext (const xmlChar *str, xmlXPathContextPtr ctxt);
XMLPUBFUN void XMLCALL
        xmlXPathFreeParserContext(xmlXPathParserContextPtr ctxt);


XMLPUBFUN xmlXPathObjectPtr XMLCALL
        valuePop  (xmlXPathParserContextPtr ctxt);
XMLPUBFUN int XMLCALL
        valuePush (xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr value);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewString   (const xmlChar *val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewCString  (const char *val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathWrapString  (xmlChar *val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathWrapCString (char * val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewFloat    (double val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewBoolean  (int val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewNodeSet  (xmlNodePtr val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewValueTree(xmlNodePtr val);
XMLPUBFUN void XMLCALL
        xmlXPathNodeSetAdd  (xmlNodeSetPtr cur, xmlNodePtr val);
XMLPUBFUN void XMLCALL
        xmlXPathNodeSetAddUnique(xmlNodeSetPtr cur, xmlNodePtr val);
XMLPUBFUN void XMLCALL
        xmlXPathNodeSetAddNs(xmlNodeSetPtr cur, xmlNodePtr node, xmlNsPtr ns);
XMLPUBFUN void XMLCALL
        xmlXPathNodeSetSort (xmlNodeSetPtr set);
XMLPUBFUN void XMLCALL
        xmlXPathRoot        (xmlXPathParserContextPtr ctxt);
XMLPUBFUN void XMLCALL
        xmlXPathEvalExpr    (xmlXPathParserContextPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
        xmlXPathParseName   (xmlXPathParserContextPtr ctxt);
XMLPUBFUN xmlChar * XMLCALL
        xmlXPathParseNCName (xmlXPathParserContextPtr ctxt);

/*
 * Existing functions.
 */
XMLPUBFUN double XMLCALL
        xmlXPathStringEvalNumber (const xmlChar *str);
XMLPUBFUN int XMLCALL
        xmlXPathEvaluatePredicateResult (xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr res);
XMLPUBFUN void XMLCALL
        xmlXPathRegisterAllFunctions(xmlXPathContextPtr ctxt);
XMLPUBFUN xmlNodeSetPtr XMLCALL
        xmlXPathNodeSetMerge        (xmlNodeSetPtr val1, xmlNodeSetPtr val2);
XMLPUBFUN void XMLCALL
        xmlXPathNodeSetDel          (xmlNodeSetPtr cur, xmlNodePtr val);
XMLPUBFUN void XMLCALL
        xmlXPathNodeSetRemove       (xmlNodeSetPtr cur, int val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathNewNodeSetList      (xmlNodeSetPtr val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathWrapNodeSet         (xmlNodeSetPtr val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
        xmlXPathWrapExternal        (void *val);

XMLPUBFUN int  XMLCALL xmlXPathEqualValues(xmlXPathParserContextPtr ctxt);
XMLPUBFUN int  XMLCALL xmlXPathNotEqualValues(xmlXPathParserContextPtr ctxt);
XMLPUBFUN int  XMLCALL xmlXPathCompareValues(xmlXPathParserContextPtr ctxt, int inf, int strict);
XMLPUBFUN void XMLCALL xmlXPathValueFlipSign(xmlXPathParserContextPtr ctxt);
XMLPUBFUN void XMLCALL xmlXPathAddValues(xmlXPathParserContextPtr ctxt);
XMLPUBFUN void XMLCALL xmlXPathSubValues(xmlXPathParserContextPtr ctxt);
XMLPUBFUN void XMLCALL xmlXPathMultValues(xmlXPathParserContextPtr ctxt);
XMLPUBFUN void XMLCALL xmlXPathDivValues(xmlXPathParserContextPtr ctxt);
XMLPUBFUN void XMLCALL xmlXPathModValues(xmlXPathParserContextPtr ctxt);

XMLPUBFUN int XMLCALL xmlXPathIsNodeType(const xmlChar *name);

/*
 * Some of the axis navigation routines.
 */
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextChild(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextDescendant(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextDescendantOrSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextParent(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextAncestorOrSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextFollowingSibling(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextFollowing(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextNamespace(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextAttribute(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextPreceding(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextAncestor(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL xmlXPathNextPrecedingSibling(xmlXPathParserContextPtr ctxt, xmlNodePtr cur);
/*
 * The official core of XPath functions.
 */
XMLPUBFUN void XMLCALL xmlXPathLastFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathPositionFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathCountFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathIdFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathLocalNameFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathNamespaceURIFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathStringFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathStringLengthFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathConcatFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathContainsFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathStartsWithFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathSubstringFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathSubstringBeforeFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathSubstringAfterFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathNormalizeFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathTranslateFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathNotFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathTrueFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathFalseFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathLangFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathNumberFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathSumFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathFloorFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathCeilingFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathRoundFunction(xmlXPathParserContextPtr ctxt, int nargs);
XMLPUBFUN void XMLCALL xmlXPathBooleanFunction(xmlXPathParserContextPtr ctxt, int nargs);

/**
 * Really internal functions
 */
XMLPUBFUN void XMLCALL xmlXPathNodeSetFreeNs(xmlNsPtr ns);

// XMLENGINE: NEW CODE -- XForms extensions support
void addNodeSetsFromStackToDependencyList(xmlXPathParserContextPtr ctxt, int nargs);
// END NEW CODE

#ifdef __cplusplus
}
#endif
#endif /* XML_XPATH_INTERNALS_H */

