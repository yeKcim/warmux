/*
 * Summary: XML Path Language implementation
 * Description: API for the XML Path Language implementation
 *
 * XML Path Language implementation
 * XPath is a language for addressing parts of an XML document,
 * designed to be used by both XSLT and XPointer
 *     http://www.w3.org/TR/xpath
 *
 * Implements
 * W3C Recommendation 16 November 1999
 *     http://www.w3.org/TR/1999/REC-xpath-19991116
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

#ifndef XML_XPATH_H
#define XML_XPATH_H

#include <stdapis/libxml2/libxml2_hash.h>
#include <stdapis/libxml2/libxml2_dict.h>
#include <stdapis/libxml2/libxml2_xmlerror.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _xmlXPathContext xmlXPathContext;
typedef xmlXPathContext *xmlXPathContextPtr;
typedef struct _xmlXPathParserContext xmlXPathParserContext;
typedef xmlXPathParserContext *xmlXPathParserContextPtr;

/**
 * The set of XPath error codes.
 */
typedef enum {
    XPATH_EXPRESSION_OK = 0,
    XPATH_NUMBER_ERROR,
    XPATH_UNFINISHED_LITERAL_ERROR,
    XPATH_START_LITERAL_ERROR,
    XPATH_VARIABLE_REF_ERROR,
    XPATH_UNDEF_VARIABLE_ERROR,
    XPATH_INVALID_PREDICATE_ERROR,
    XPATH_EXPR_ERROR,
    XPATH_UNCLOSED_ERROR,
    XPATH_UNKNOWN_FUNC_ERROR,
    XPATH_INVALID_OPERAND,
    XPATH_INVALID_TYPE,
    XPATH_INVALID_ARITY,
    XPATH_INVALID_CTXT_SIZE,
    XPATH_INVALID_CTXT_POSITION,
    XPATH_MEMORY_ERROR,
    XPTR_SYNTAX_ERROR,
    XPTR_RESOURCE_ERROR,
    XPTR_SUB_RESOURCE_ERROR,
    XPATH_UNDEF_PREFIX_ERROR,
    XPATH_ENCODING_ERROR,
    XPATH_INVALID_CHAR_ERROR,
    XPATH_XE_EXTENSION_FUNC_ERROR
} xmlXPathError;

/*
 * A node-set (an unordered collection of nodes without duplicates).
 */
typedef struct _xmlNodeSet xmlNodeSet;
typedef xmlNodeSet *xmlNodeSetPtr;
struct _xmlNodeSet {
    int nodeNr;             /* number of nodes in the set */
    int nodeMax;            /* size of the array as allocated */
    xmlNodePtr* nodeTab;    /* array of nodes in no particular order */
    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
};

/*
 * An expression is evaluated to yield an object, which
 * has one of the following four basic types:
 *   - node-set
 *   - boolean
 *   - number
 *   - string
 *
 * @@ XPointer will add more types !
 */

typedef enum {
    XPATH_UNDEFINED = 0,
    XPATH_NODESET = 1,
    XPATH_BOOLEAN = 2,
    XPATH_NUMBER = 3,
    XPATH_STRING = 4,
    XPATH_POINT = 5,
    XPATH_RANGE = 6,
    XPATH_LOCATIONSET = 7,
    XPATH_USERS = 8,
    XPATH_XSLT_TREE = 9  /* An XSLT value tree, non modifiable */
} xmlXPathObjectType;

typedef struct _xmlXPathObject xmlXPathObject;
typedef xmlXPathObject *xmlXPathObjectPtr;
struct _xmlXPathObject {
    xmlXPathObjectType  type;
    xmlNodeSetPtr       nodesetval;
    int                 boolval;
    double              floatval;
    xmlChar*            stringval;
    void*               user;
    int                 index;
    void*               user2;
    int                 index2;
};

/**
 * xmlXPathConvertFunc:
 * @param obj an XPath object
 * @param type the number of the target type
 *
 * A conversion function is associated to a type and used to cast
 * the new type to primitive values.
 *
 * Returns -1 in case of error, 0 otherwise
 */
typedef int (*xmlXPathConvertFunc) (xmlXPathObjectPtr obj, int type);

/*
 * Extra type: a name and a conversion function.
 */

typedef struct _xmlXPathType xmlXPathType;
typedef xmlXPathType *xmlXPathTypePtr;
struct _xmlXPathType {
    const xmlChar       *name;      /* the type name */
    xmlXPathConvertFunc func;       /* the conversion function */
};

/*
 * Extra variable: a name and a value.
 */

typedef struct _xmlXPathVariable xmlXPathVariable;
typedef xmlXPathVariable *xmlXPathVariablePtr;
struct _xmlXPathVariable {
    const xmlChar     *name;        /* the variable name */
    xmlXPathObjectPtr value;        /* the value */
};

/**
 * xmlXPathEvalFunc:
 * @param ctxt an XPath parser context
 * @param nargs the number of arguments passed to the function
 *
 * An XPath evaluation function, the parameters are on the XPath context stack.
 */

typedef void (*xmlXPathEvalFunc)(xmlXPathParserContextPtr ctxt, int nargs);

/*
 * Extra function: a name and a evaluation function.
 */

typedef struct _xmlXPathFunct xmlXPathFunct;
typedef xmlXPathFunct *xmlXPathFuncPtr;
struct _xmlXPathFunct {
    const xmlChar      *name;       /* the function name */
    xmlXPathEvalFunc func;      /* the evaluation function */
};

/**
 * xmlXPathAxisFunc:
 * @param ctxt the XPath interpreter context
 * @param cur the previous node being explored on that axis
 *
 * An axis traversal function. To traverse an axis, the engine calls
 * the first time with cur == NULL and repeat until the function returns
 * NULL indicating the end of the axis traversal.
 *
 * Returns the next node in that axis or NULL if at the end of the axis.
 */

typedef xmlXPathObjectPtr (*xmlXPathAxisFunc) (xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr cur);

/*
 * Extra axis: a name and an axis function.
 */

typedef struct _xmlXPathAxis xmlXPathAxis;
typedef xmlXPathAxis* xmlXPathAxisPtr;
struct _xmlXPathAxis {
    const xmlChar*   name;       /* the axis name */
    xmlXPathAxisFunc func;       /* the search function */
};

// XMLENGINE: NEW CODE -- XForms extensions support
/**
Callback for resolving prefix names into namespace URIs

@return Namespace URI for aNs prefix.

Resolving is made with aCtxt context.

This function is used internally by for implementation of
namespace-resovling feature in XPath API of XML Engine
(MXmlEngNamespaceResolver interface is called by libxml2)
*/
typedef const xmlChar* (*xeXPathNsResolverFunc)(void* aCtxt, const xmlChar* aNs);
//---

/**
 * xmlXPathContext:
 *
 * Expression evaluation occurs with respect to a context.
 * the context consists of:
 *    - a node (the context node)
 *    - a node list (the context node list)
 *    - a set of variable bindings
 *    - a function library
 *    - the set of namespace declarations in scope for the expression
 * Following the switch to hash tables, this need to be trimmed up at
 * the next binary incompatible release.
 */
struct _xmlXPathContext {
    xmlDocPtr doc;          /* The current document */
    xmlNodePtr node;            /* The current node */

    //int nb_variables_unused;        /* unused (hash table) */
    //int max_variables_unused;       /* unused (hash table) */
    xmlHashTablePtr varHash;        /* Hash table of defined variables */

    int nb_types;           /* number of defined types */
    int max_types;          /* max number of types */
    xmlXPathTypePtr types;      /* Array of defined types */

    //int nb_funcs_unused;        /* unused (hash table) */
    //int max_funcs_unused;       /* unused (hash table) */
    xmlHashTablePtr funcHash;       /* Hash table of defined funcs */

    int nb_axis;            /* number of defined axis */
    int max_axis;           /* max number of axis */
    xmlXPathAxisPtr axis;       /* Array of defined axis */

    /* the namespace nodes of the context node */
    xmlNsPtr *namespaces;       /* Array of namespaces */
    int nsNr;               /* number of namespace in scope */
    void *user;             /* function to free */

    /* extra variables */
    int contextSize;            /* the context size */
    int proximityPosition;      /* the proximity position */

    /* extra stuff for XPointer */
    int xptr;               /* it this an XPointer context */
    xmlNodePtr here;            /* for here() */
    xmlNodePtr origin;          /* for origin() */

    /* the set of namespace declarations in scope for the expression */
    xmlHashTablePtr nsHash;         /* The namespaces hash table */
    void* varLookupFunc;            /* variable lookup func */
    void* varLookupData;            /* variable lookup data */

    /* Possibility to link in an extra item */
    void* extra;                    /* needed for XSLT */

    /* The function name and URI when calling a function */
    const xmlChar* function;
    const xmlChar* functionURI;

    /* function lookup function and data */
    void* funcLookupFunc;           /* function lookup func */
    void* funcLookupData;           /* function lookup data */

    /* temporary namespace lists kept for walking the namespace axis */
    xmlNsPtr* tmpNsList;            /* Array of namespaces */
    int       tmpNsNr;              /* number of namespace in scope */

    /* error reporting mechanism */
    void*   userData;               /* user specific data block */
    xmlStructuredErrorFunc error;   /* the callback in case of errors */
    xmlError lastError;             /* the last error */
    xmlNodePtr debugNode;           /* the source node XSLT */

    /* dictionnary */
    xmlDictPtr dict;                /* dictionnary if any */

// XMLENGINE: NEW CODE -- XForms extensions support
    xmlHashTablePtr instanceDocs;  /* hash table that stores instance XForms instance docs,
                                      not owned by the structure, will be freed by the client*/
    xmlNodeSetPtr   dependencyList;/* nodes that the expression depends on */
    xeXPathNsResolverFunc xeResolveNs; /* find namespace URI bound to prefix in the current context */
    void*                 xeResolveNsCtxt; /* resolver-specific context for processing */
// END NEW CODE

};

/*
 * The structure of a compiled expression form has become public in XML ENGINE
 */

typedef enum {
    XPATH_OP_END=0,
    XPATH_OP_AND,
    XPATH_OP_OR,
    XPATH_OP_EQUAL,
    XPATH_OP_CMP,
    XPATH_OP_PLUS,
    XPATH_OP_MULT,
    XPATH_OP_UNION,
    XPATH_OP_ROOT,
    XPATH_OP_NODE,
    XPATH_OP_RESET,
    XPATH_OP_COLLECT,
    XPATH_OP_VALUE,
    XPATH_OP_VARIABLE,
    XPATH_OP_FUNCTION,
    XPATH_OP_ARG,
    XPATH_OP_PREDICATE,
    XPATH_OP_FILTER,
    XPATH_OP_SORT
#ifdef LIBXML_XPTR_ENABLED
    ,XPATH_OP_RANGETO
#endif
} xmlXPathOp;

typedef struct _xmlXPathStepOp xmlXPathStepOp;
typedef xmlXPathStepOp *xmlXPathStepOpPtr;
struct _xmlXPathStepOp {
    xmlXPathOp  op;      /* The identifier of the operation */
    int        ch1;      /* First child */
    int        ch2;      /* Second child */
    int      value;
    int     value2;
    int     value3;
    void*   value4;
    void*   value5;
    void*    cache;
    void* cacheURI;
};


/**
 The initial size of 'steps' table in the precompiled XPath exression

 Double-it policy for growth is used in the code
*/
#define XPATH_STEPS_GRANULARITY 10

typedef struct _xmlXPathCompExpr xmlXPathCompExpr;
typedef xmlXPathCompExpr*   xmlXPathCompExprPtr;
struct _xmlXPathCompExpr {
    int nbStep;             /* Number of steps in this expression */
    int maxStep;            /* Maximum number of steps allocated */
    xmlXPathStepOp* steps;  /* ops for computation of this expression */
    int last;               /* index of last step in expression */
    xmlChar*    expr;       /* the expression being computed */
    xmlDictPtr  dict;       /* the dictionnary to use if any */
#ifdef DEBUG_EVAL_COUNTS
    int nb;
    xmlChar*  string;
#endif
//XMLENGINE: NEW CODE
    void* extendedContext;  /* some data set by API users and available for XPath extension functions */
    void* xeNsResolver;     /* default namespace resolver supplied when compiling expression */
//XMLENGINE: END NEW CODE
};
/**
 * xmlXPathParserContext:
 *
 * An XPath parser context. It contains pure parsing informations,
 * an xmlXPathContext, and the stack of objects.
 */
struct _xmlXPathParserContext {
    const xmlChar *cur;             /* the current char being parsed */
    const xmlChar *base;            /* the full expression */

    int error;                      /* error code */

    xmlXPathContextPtr  context;    /* the evaluation context */
    xmlXPathObjectPtr     value;    /* the current value */
    int                 valueNr;    /* number of values stacked */
    int                valueMax;    /* max number of values stacked */
    xmlXPathObjectPtr *valueTab;    /* stack of values */

    xmlXPathCompExprPtr comp;       /* the precompiled expression */
    int xptr;                       /* it this an XPointer expression */
    xmlNodePtr         ancestor;    /* used for walking preceding axis */
};

/**
 * xmlXPathFunction:
 * @param ctxt the XPath interprestation context
 * @param nargs the number of arguments
 *
 * An XPath function.
 * The arguments (if any) are popped out from the context stack
 * and the result is pushed on the stack.
 */

typedef void (*xmlXPathFunction) (xmlXPathParserContextPtr ctxt, int nargs);

/************************************************************************
 *                                                                      *
 *          Public API                                                  *
 *                                                                      *
 ************************************************************************/

/*
NOTE: Now defined as local inline functions in xpath.c
      It seems that no one uses them from outside..
     --> used by libxslt
*/

#define xmlXPathIsNaN(val) trio_isnan(val)
#define xmlXPathIsInf(val) trio_isinf(val)


/* These macros may later turn into functions */
/**
 * xmlXPathNodeSetGetLength:
 * @param ns a node-set
 *
 * Implements a functionality similar to the DOM NodeList.length.
 *
 * Returns the number of nodes in the node-set.
 */
#define xmlXPathNodeSetGetLength(ns) ((ns) ? (ns)->nodeNr : 0)
/**
 * xmlXPathNodeSetItem:
 * @param ns a node-set
 * @param index index of a node in the set
 *
 * Implements a functionality similar to the DOM NodeList.item().
 *
 * Returns the xmlNodePtr at the given index in ns or NULL if
 *         index is out of range (0 to length-1)
 */
#define xmlXPathNodeSetItem(ns, index)                          \
        (( (ns) && ((index) >= 0) && ((index) < (ns)->nodeNr) ) \
        ? (ns)->nodeTab[(index)]                                \
        : NULL)
/**
 * xmlXPathNodeSetIsEmpty:
 * @param ns a node-set
 *
 * Checks whether ns is empty or not.
 *
 * Returns %TRUE if ns is an empty node-set.
 */
#define xmlXPathNodeSetIsEmpty(ns)                                      \
    (((ns) == NULL) || ((ns)->nodeNr == 0) || ((ns)->nodeTab == NULL))


XMLPUBFUN void XMLCALL
            xmlXPathFreeObject      (xmlXPathObjectPtr obj);
XMLPUBFUN xmlNodeSetPtr XMLCALL
            xmlXPathNodeSetCreate   (xmlNodePtr val);
XMLPUBFUN void XMLCALL
            xmlXPathFreeNodeSetList (xmlXPathObjectPtr obj);
XMLPUBFUN void XMLCALL
            xmlXPathFreeNodeSet     (xmlNodeSetPtr obj);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathObjectCopy      (xmlXPathObjectPtr val);
XMLPUBFUN int XMLCALL
            xmlXPathCmpNodes        (xmlNodePtr node1, xmlNodePtr node2);
/**
 * Conversion functions to basic types.
 */
XMLPUBFUN int XMLCALL
            xmlXPathCastNumberToBoolean (double val);
XMLPUBFUN int XMLCALL
            xmlXPathCastStringToBoolean (const xmlChar * val);
XMLPUBFUN int XMLCALL
            xmlXPathCastNodeSetToBoolean(xmlNodeSetPtr ns);
XMLPUBFUN int XMLCALL
            xmlXPathCastToBoolean   (xmlXPathObjectPtr val);

XMLPUBFUN double XMLCALL
            xmlXPathCastBooleanToNumber (int val);
XMLPUBFUN double XMLCALL
            xmlXPathCastStringToNumber  (const xmlChar * val);
XMLPUBFUN double XMLCALL
            xmlXPathCastNodeToNumber    (xmlNodePtr node);
XMLPUBFUN double XMLCALL
            xmlXPathCastNodeSetToNumber (xmlNodeSetPtr ns);
XMLPUBFUN double XMLCALL
            xmlXPathCastToNumber    (xmlXPathObjectPtr val);

XMLPUBFUN xmlChar* XMLCALL
            xmlXPathCastBooleanToString (int val);
XMLPUBFUN xmlChar* XMLCALL
            xmlXPathCastNumberToString  (double val);
XMLPUBFUN xmlChar* XMLCALL
            xmlXPathCastNodeToString    (xmlNodePtr node);
XMLPUBFUN xmlChar* XMLCALL
            xmlXPathCastNodeSetToString (xmlNodeSetPtr ns);
XMLPUBFUN xmlChar* XMLCALL
            xmlXPathCastToString    (xmlXPathObjectPtr val);

XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathConvertBoolean  (xmlXPathObjectPtr val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathConvertNumber   (xmlXPathObjectPtr val);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathConvertString   (xmlXPathObjectPtr val);

/**
 * Context handling.
 */
XMLPUBFUN void XMLCALL
            xmlXPathInit            (void);
XMLPUBFUN xmlXPathContextPtr XMLCALL
            xmlXPathNewContext      (xmlDocPtr doc);
XMLPUBFUN void XMLCALL
            xmlXPathFreeContext     (xmlXPathContextPtr ctxt);

/**
 * Evaluation functions.
 */
XMLPUBFUN long XMLCALL
            xmlXPathOrderDocElems   (xmlDocPtr doc);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathEval            (const xmlChar *str, xmlXPathContextPtr ctx);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathEvalExpression  (const xmlChar *str, xmlXPathContextPtr ctxt);
XMLPUBFUN int XMLCALL
            xmlXPathEvalPredicate   (xmlXPathContextPtr ctxt, xmlXPathObjectPtr res);
/**
 * Separate compilation/evaluation entry points.
 */
XMLPUBFUN xmlXPathCompExprPtr XMLCALL
            xmlXPathCompile         (const xmlChar *str);
XMLPUBFUN xmlXPathCompExprPtr XMLCALL
            xmlXPathCtxtCompile     (xmlXPathContextPtr ctxt, const xmlChar *str);
XMLPUBFUN xmlXPathObjectPtr XMLCALL
            xmlXPathCompiledEval    (xmlXPathCompExprPtr comp,xmlXPathContextPtr ctx);
XMLPUBFUN void XMLCALL
            xmlXPathFreeCompExpr    (xmlXPathCompExprPtr comp);

#ifdef __cplusplus
}
#endif

#endif /* XML_XPATH_H */

